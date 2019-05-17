CC = g++
CFLAGS = `pkg-config --libs --cflags opencv4` -Wall -w -std=c++11
SRC = gl.cpp
all:
	@$(CC) $(SRC) -I$(INCLUDE) $(CFLAGS) -framework GLUT -framework OpenGL -o gl
clean:
	@rm -f gl
