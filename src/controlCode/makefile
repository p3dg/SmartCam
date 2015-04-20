# rudimentary makefile for "controlSys.c"

CFLAGS = -Wall -std=c99
OBJECTS = loadInfo.o subFuncs.o controlFuncs.o

controlSys : main.c $(OBJECTS)	
	gcc $< $(OBJECTS) -lm -lhiredis -o $@ 

loadInfo.o : loadInfo.c controlSys.h
	gcc $< -c -o $@

subFuncs.o : subFuncs.c controlSys.h
	gcc $< -c -o $@

controlFuncs.o : controlFuncs.c subFuncs.o controlSys.h
	gcc $< -c -o $@
