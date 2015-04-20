DEBUG= -g

CFLAGS= $(DEBUG) -I../include -I.. -I.
CFLAGS_OPENCV= $(DEBUG) `pkg-config --cflags opencv` -Wall -Wextra
LIBS_OPENCV= `pkg-config --libs opencv`
#-Wall -Wextra

LIBS=-L.. -L../lib -lhiredis
LIBS_BLACKMAGIC=-ldl -lm -lpthread -pthread -lstdc++ 
LIBS_OPENCV=  `pkg-config --libs opencv`
LIBS_IMG_CONVERSION= -lpixfc-sse

OBJECTS=fsm_mover.o linked_list.o movement_logic.o cJSON.o
DEPS=fsm_mover.h linked_list.h movement_logic.h cJSON.h

OBJECTS_WEBCAM=main_webcam.o $(OBJECTS)
OBJECTS_BLACKMAGIC=main_blackmagic.o $(OBJECTS) 

all: $(OBJECTS_WEBCAM)
	make vision_code_webcam; make vision_code_blackmagic

vision_code_webcam: $(OBJECTS_WEBCAM)
	g++ $(OBJECTS_WEBCAM) $(LIBS_OPENCV) -o vision_code_webcam $(LIBS)

vision_code_blackmagic: $(OBJECTS_BLACKMAGIC) 
	g++ $(OBJECTS_BLACKMAGIC) ../capture_blackmagic.o ../DeckLinkAPIDispatch.o ../image_conversion.o $(LIBS_OPENCV) -o vision_code_blackmagic $(LIBS_BLACKMAGIC) $(LIBS_IMG_CONVERSION) $(LIBS)

%.o: %.cpp $(DEPS)
	g++ $(CFLAGS_OPENCV) $(CFLAGS) -c $< -o $@ 

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

main_blackmagic.o: main.cpp $(DEPS)
	g++ $(CFLAGS_OPENCV) $(CFLAGS) -DBLACKMAGIC_CAPTURE -c $< -o $@ 

main_webcam.o: main.cpp $(DEPS)
	g++ $(CFLAGS_OPENCV) $(CFLAGS) -c $< -o $@ 

clean:
	rm -rf $(OBJECTS_WEBCAM) $(OBJECTS_BLACKMAGIC) vision_code_webcam vision_code_blackmagic

