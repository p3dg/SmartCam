/* Blackmagic Capture Code
 *
 * Author: Idan Warsawski
 */


#include "capture_blackmagic.h"
#include "DeckLinkAPI.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

class DeckLinkCaptureDelegate : public IDeckLinkInputCallback
{
public:
  DeckLinkCaptureDelegate();
  ~DeckLinkCaptureDelegate();

  virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID *ppv) { return E_NOINTERFACE; }
  virtual ULONG STDMETHODCALLTYPE AddRef(void);
  virtual ULONG STDMETHODCALLTYPE  Release(void);
  virtual HRESULT STDMETHODCALLTYPE VideoInputFormatChanged(BMDVideoInputFormatChangedEvents, IDeckLinkDisplayMode*, BMDDetectedVideoInputFormatFlags);
  virtual HRESULT STDMETHODCALLTYPE VideoInputFrameArrived(IDeckLinkVideoInputFrame*, IDeckLinkAudioInputPacket*);

  //private:
  struct capture_blackmagic * cbm;
  int frameCount;
};

ULONG DeckLinkCaptureDelegate::AddRef()
{
  return 0;
}

ULONG DeckLinkCaptureDelegate::Release()
{
  return 0;
}

DeckLinkCaptureDelegate::DeckLinkCaptureDelegate() 
{
  frameCount = 0;
}

DeckLinkCaptureDelegate::~DeckLinkCaptureDelegate() {}

HRESULT DeckLinkCaptureDelegate::VideoInputFrameArrived(IDeckLinkVideoInputFrame* videoFrame, IDeckLinkAudioInputPacket* audioFrame)
{
  uint8_t * frameBytes;
  BMDTimeValue timestamp, duration;

  // Handle Video Frame
  if(videoFrame)
    {	
      if (videoFrame->GetFlags() & bmdFrameHasNoInputSource)
        {
          fprintf(stderr, "Frame received - No input signal detected\n");
        }
      else
        {		

          /*
            2.4.11.2 IDeckLinkVideoInputFrame::GetHardwareReferenceTimestamp method
            
            The GetHardwareReferenceTimestamp method returns frame time and frame duration for a given timescale.
            
            Syntax
            HRESULT                      GetHardwareReferenceTimestamp
            (BMDTimeScale timeScale, BMDTimeValue *frameTime, BMDTimeValue *frameDuration);
            
            Parameters
            Name                      Direction Description
            frameTime                    out     Frame time (in units of timeScale)
            frameDuration                out     Frame duration (in units of timeScale)
            timeScale                     in     Time scale for output parameters
            
          
            Return Values
            Value
            Description
            E_FAIL                                    Failure
            S_OK                                      Success
            
          */
            
          if(cbm->video_frame_callback)
            {
              videoFrame->GetBytes((void **) &frameBytes);
              videoFrame->GetHardwareReferenceTimestamp(1000, &timestamp, &duration);
              cbm->video_frame_callback((uint8_t *) frameBytes, (uint32_t) videoFrame->GetRowBytes() * videoFrame->GetHeight(), (uint64_t) timestamp, cbm->callback_priv);
            }
          /*if (cbm->fp)
            {
              videoFrame->GetBytes(&frameBytes);
              fwrite(frameBytes, videoFrame->GetRowBytes() * videoFrame->GetHeight(), 1, cbm->fp);
              //write((int) cbm->fp, frameBytes, videoFrame->GetRowBytes() * videoFrame->GetHeight());	
              }*/
        }		
    }

  return S_OK;
}

struct capture_bm_priv
{
  IDeckLink *decklink;
  IDeckLinkInput *decklink_input;
  IDeckLinkDisplayModeIterator * display_mode_iterator;
  IDeckLinkConfiguration * decklink_config;

  IDeckLinkIterator *decklink_iterator;
  DeckLinkCaptureDelegate * delegate;
  IDeckLinkDisplayMode * display_mode;
  BMDVideoInputFlags input_flags;
  BMDDisplayMode selected_display_mode;
  BMDPixelFormat pixel_format;  

  BMDVideoConnection vid_conn;


  //pthread stuff
  pthread_mutex_t sleep_mutex;
  pthread_cond_t sleep_cond;
};

HRESULT DeckLinkCaptureDelegate::VideoInputFormatChanged(BMDVideoInputFormatChangedEvents events, IDeckLinkDisplayMode *mode, BMDDetectedVideoInputFormatFlags)
{
  return S_OK;
}

void capture_blackmagic_init(struct capture_blackmagic * cbm)
{
  struct capture_bm_priv * cbp;
  HRESULT result;


  cbm->priv = (void *) malloc(sizeof(struct capture_bm_priv));
  if(!cbm->priv)
    {
      fprintf(stderr, "Could not allocate memory for blackmagic private structures\n");
      exit(1);
    }


  cbp = (struct capture_bm_priv *) cbm->priv;
  cbp->decklink_iterator = CreateDeckLinkIteratorInstance();
  cbp->selected_display_mode = bmdModeNTSC;
  cbp->pixel_format = bmdFormat8BitYUV;
  cbp->input_flags = bmdVideoInputFlagDefault;


  //Initialize our pthread conditional signals
  pthread_mutex_init(&cbp->sleep_mutex, NULL);
  pthread_cond_init(&cbp->sleep_cond, NULL);


  if(!cbp->decklink_iterator)
    {
      fprintf(stderr, "capture blackmagic: Error: Could not create decklink iterator\n");
      exit(1);
    }
  
  result = cbp->decklink_iterator->Next(&cbp->decklink);
  if(result != S_OK)
   { 
     fprintf(stderr, "Error: No Blackmagic cards found\n");
     exit(1);
   }

  if(cbp->decklink->QueryInterface(IID_IDeckLinkInput, (void **) &cbp->decklink_input) != S_OK) //XXX void**?
    {    
      fprintf(stderr, "Error: no inputs found\n");
      exit(1);
    }

  cbp->delegate = new DeckLinkCaptureDelegate();
  cbp->delegate->cbm = cbm; //very meta
  cbp->decklink_input->SetCallback(cbp->delegate);

  result = cbp->decklink_input->GetDisplayModeIterator(&cbp->display_mode_iterator);
  if (result != S_OK)
    {
      fprintf(stderr, "Could not obtain the video output display mode iterator - result = %08x\n", result);
      exit(1);
    }

  if(cbp->decklink->QueryInterface(IID_IDeckLinkConfiguration, (void**)&(cbp->decklink_config)) != S_OK )
    {
      fprintf(stderr, "Blackmagic: Failed to get configuration interface\n" );
      exit(1);
    }

  if(cbm->video_source == BLACKMAGIC_COMPOSITE)
    cbp->vid_conn = bmdVideoConnectionComposite;
  else
    cbp->vid_conn = bmdVideoConnectionSVideo;

  result = cbp->decklink_config->SetInt( bmdDeckLinkConfigVideoInputConnection, cbp->vid_conn );

  if(result != S_OK)
    {
      fprintf(stderr, "Blackmagic: Failed to set video input connection\n" );
      exit(1);
    }
	
  bool found_display_mode = false;

  int display_mode_count = 0;

  while(cbp->display_mode_iterator->Next(&cbp->display_mode) == S_OK)
    {
      if(cbp->selected_display_mode == cbp->display_mode->GetDisplayMode())
        {
          BMDDisplayModeSupport foobar;
          cbp->decklink_input->DoesSupportVideoMode(cbp->selected_display_mode, cbp->pixel_format, bmdVideoInputFlagDefault, &foobar, NULL);
          if(foobar == bmdDisplayModeNotSupported)
            {
              fprintf(stderr, "The display mode requested is not supported\n");
              exit(1);
            }
          else
            {
              found_display_mode = true;
            }
        }
      /*if(cbp->bmd_display_mode == display_mode_count)
        {
          BMDDisplayModeSupport foobar;
          cbp->selected_display_mode = cbp->display_mode->GetDisplayMode();
          cbp->decklink_input->DoesSupportVideoMode(cbp->selected_display_mode, cbp->pixel_format, bmdVideoInputFlagDefault, &foobar, NULL);
          if(foobar == bmdDisplayModeNotSupported)
            {
              fprintf(stderr, "The display mode requested is not supported\n");
              exit(1);
            }
        }
      */
      display_mode_count++;
      cbp->display_mode->Release();
    }

  if(!found_display_mode)
    {
      fprintf(stderr, "No valid display modes found\n");
      exit(1);
    }
}

void capture_blackmagic_start(struct capture_blackmagic * cbm)
{
  struct capture_bm_priv * cbp;
  HRESULT result;

  cbp = (struct capture_bm_priv *) cbm->priv;


  result = cbp->decklink_input->EnableVideoInput(cbp->selected_display_mode, cbp->pixel_format, cbp->input_flags);
  if(result != S_OK)
    {
      fprintf(stderr, "Unable to start video input -- the card may be in use by another application\n");
      exit(1);
    }

  result = cbp->decklink_input->StartStreams();
  if(result != S_OK)
    {
      fprintf(stderr, "Unable to start streaming\n");
      exit(1);
    }
}

void capture_blackmagic_block(struct capture_blackmagic * cbm)
{
  struct capture_bm_priv * cbp = (struct capture_bm_priv *) cbm->priv;

  pthread_mutex_lock(&cbp->sleep_mutex);
  pthread_cond_wait(&cbp->sleep_cond, &cbp->sleep_mutex);
  pthread_mutex_unlock(&cbp->sleep_mutex);
}


void capture_blackmagic_unblock(struct capture_blackmagic * cbm)
{
  struct capture_bm_priv * cbp = (struct capture_bm_priv *) cbm->priv;

  pthread_mutex_lock(&cbp->sleep_mutex);
  pthread_cond_signal(&cbp->sleep_cond);
  pthread_mutex_unlock(&cbp->sleep_mutex);
}
