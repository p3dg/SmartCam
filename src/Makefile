# It's a makefile!
DEBUG=-g

OBJECTS= image_conversion.o capture_blackmagic.o DeckLinkAPIDispatch.o

CFLAGS= $(DEBUG) -I./include -I./include/decklink 
#-Wall -Wextra
LIBS=-L./lib
LIBS_BLACKMAGIC=-ldl -lm -lpthread

all: $(OBJECTS)

image_conversion.o: image_conversion.c image_conversion.h
	$(CC) $(CFLAGS) -c $< -o $@

capture_blackmagic.o: capture_blackmagic.cpp capture_blackmagic.h
	g++ $(CFLAGS) -c $< -o $@ -fno-rtti 

DeckLinkAPIDispatch.o: include/decklink/DeckLinkAPIDispatch.cpp
	g++ $(CFLAGS) -c $< -o $@ -fno-rtti 

test: all
	cd tests; make all

vision_code: all
	cd vision_code; make all

visca_handler:
	cd visca_handler; make all

website:
	cd website; make all

clean:
	rm -rf *.o
	cd tests; make clean;
	cd vision_code; make clean;
	cd visca_handler; make clean;
	cd website; make clean;

#g++ capture_blackmagic.cpp include/decklink/DeckLinkAPIDispatch.cpp -I include/decklink/ -ldl -lm -lpthread -fno-rtti

