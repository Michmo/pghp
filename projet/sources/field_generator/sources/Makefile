ROOT=../..

CC= g++
CFLAGS= -g -W -Wall -Wextra -I$(ROOT)/include

OBJ=field_generator.o PgmReader.o
BIN=field_generator

all: $(BIN)

field_generator: field_generator.o PgmReader.o
	$(CC) $(CFLAGS) $(OBJ) -o $(BIN)

field_generator.o: field_generator.cpp ../includes/PgmReader.h

PgmReader.o: PgmReader.cpp ../includes/PgmReader.h

clean:
	rm -f $(OBJ) *~

distclean: clean
	rm -f $(BIN)
