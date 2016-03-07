CC=g++

INC=../include

CFLAGS = -W -Wall -pedantic -std=c++11
LDFLAGS =
SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

all: ps3event_clientd

ps3event_clientd: $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

%.o:	%.cpp
	$(CC) $(CFLAGS) -I $(INC) -c $< -o $@

clean:
	rm -f *.o
