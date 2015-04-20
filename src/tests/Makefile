# Tests Makefile
# Author: Idan Warsawski
DEBUG=-g
OBJECTS=t_image_conversion.o t_capture_blackmagic.o t_generate_ipl_image.o

CFLAGS= $(DEBUG) -I../include -I.. -I. -I../include/decklink
CFLAGS_OPENCV= `pkg-config --cflags opencv`
#-Wall -Wextra

LIBS=-L.. -L../lib
LIBS_BLACKMAGIC=-ldl -lm -lpthread -pthread -lstdc++
LIBS_OPENCV= `pkg-config --libs opencv`
LIBS_IMG_CONVERSION= -lpixfc-sse

all: $(OBJECTS)
	$(CC) t_image_conversion.o ../image_conversion.o -o t_image_conversion $(LIBS_IMG_CONVERSION) $(LIBS)
	$(CC) t_capture_blackmagic.o ../capture_blackmagic.o ../DeckLinkAPIDispatch.o -o t_capture_blackmagic  $(LIBS) $(LIBS_BLACKMAGIC)
	$(CC) t_generate_ipl_image.o ../capture_blackmagic.o ../DeckLinkAPIDispatch.o ../image_conversion.o -o t_generate_ipl_image  $(LIBS) $(LIBS_BLACKMAGIC) $(LIBS_OPENCV) $(LIBS_IMG_CONVERSION)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

t_capture_blackmagic.o: t_capture_blackmagic.c
	$(CC) $(CFLAGS) -c $< -o $@

t_generate_ipl_image.o: t_generate_ipl_image.c
	$(CC) $(CFLAGS) $(CFLAGS_OPENCV) -c $< -o $@

clean:
	rm -rf *.o t_image_conversion t_capture_blackmagic t_generate_ipl_image
