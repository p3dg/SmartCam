DEBUG= -g

CFLAGS= $(DEBUG) -I../include -I.. -I.

LIBS=-L.. -L../lib ../lib/libvisca.a -lm -lhiredis -static

OBJECTS=handler.o redir.o
DEPS=

all: visca_handler redir

visca_handler: handler.o
	$(CC) $< -o visca_handler $(LIBS)

redir: redir.o
	$(CC) $< -o redir

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@ 

clean:
	rm -rf $(OBJECTS) visca_handler redir
