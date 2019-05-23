CC = g++
CFLAGS = `pkg-config --libs --cflags opencv4` -Wall -w -std=c++11
INCLUDE = include/
SRC = src/marker.cpp src/calibration.cpp src/main.cpp
LINK = /

ifeq ($(shell uname -s),Darwin)
	LINK = -framework GLUT -framework OpenGL
else
	LINK = -lGL -lGLU -lglut
endif

all:
	@$(CC) $(SRC) -I$(INCLUDE) $(CFLAGS) $(LINK) -o gl
	
clean:
	@rm -f gl
