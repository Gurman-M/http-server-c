IDIR =../include
CC=gcc
CFLAGS=-Wall -I$(IDIR)

ODIR=obj

_DEPS = client_connection.h file_utils.h network.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o client_connection.o file_utils.o network.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
