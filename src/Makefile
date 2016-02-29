CC=g++

INC=../include

CFLAGS = -W -Wall -pedantic -std=c++11
LDFLAGS =
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)
# SERVER = $(INC)/serveur.h
# COMD = $(INC)/catalogue.h $(INC)/lire.h $(INC)/stocker.h $(INC)/detruire.h
# UTILS = $(INC)/utils.h $(INC)/log.h
# LOG = $(INC)/log.h

all: ps3event_clientd

ps3event_clientd: $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

# main.o:	$(SERVER) $(LOG)

# serveur.o:	$(COMD) $(UTILS)

# catalogue.o:	$(UTILS)

# lire.o:	$(UTILS)

# stocker.o:	$(UTILS)

# detruire.o:	$(UTILS)

# utils.o:	$(LOG)

%.o:	%.cpp
	$(CC) $(CFLAGS) -I $(INC) -c $< -o $@

clean:
	rm -f *.o
