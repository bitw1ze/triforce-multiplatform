# The following two macros set up the compiler flags and compiler command

DFLAGS = -DDEBUG 
CFLAGS = -ggdb -Wall
CC = gcc
LFLAGS = -lGL -lGLU -lglut

# Default rule to compile files using the same CC command

PROG = triforce
# add Xbox.o
# Start of the target section

all:	$(PROG)

$(PROG): 2DGraphics.o block.o bonus.o buttons.o combo.o cursor.o fall.o gameplay.o grid.o hud.o input.o main.o triforce.o
	$(CC) -o $(PROG) 2DGraphics.o block.o bonus.o buttons.o combo.o cursor.o fall.o gameplay.o grid.o hud.o input.o main.o triforce.o $(LFLAGS) $(CFLAGS) 

2DGraphics.o:	2DGraphics.cpp 2DGraphics.h
	$(CC) -c 2DGraphics.cpp $(CFLAGS)

block.o:	block.cpp game.h
	$(CC) -c block.cpp $(CFLAGS) 

bonus.o:	bonus.cpp game.h
	$(CC) -c bonus.cpp $(CFLAGS)

buttons.o:	buttons.cpp buttons.h
	$(CC) -c buttons.cpp $(CFLAGS) 

combo.o:	combo.cpp game.h
	$(CC) -c combo.cpp $(CFLAGS) 

cursor.o:	cursor.cpp game.h input.h
	$(CC) -c cursor.cpp $(CFLAGS) 

fall.o:	fall.cpp game.h
	$(CC) -c fall.cpp $(CFLAGS) 

gameplay.o:	gameplay.cpp game.h input.h
	$(CC) -c gameplay.cpp $(CFLAGS) 

grid.o:	grid.cpp game.h input.h
	$(CC) -c grid.cpp $(CFLAGS) 

hud.o:	hud.cpp game.h 
	$(CC) -c hud.cpp $(CFLAGS) 

input.o:	input.cpp input.h
	$(CC) -c input.cpp $(CFLAGS) 

main.o:	main.cpp triforce.h
	$(CC) -c main.cpp $(CFLAGS) 

triforce.o: triforce.cpp triforce.h
	$(CC) -c triforce.cpp $(CFLAGS) 

clean:
	rm -f $(PROG) *.o
