CC = g++
CFLAGS = `pkg-config --libs --cflags opencv4` -Wall -w -std=c++11
INCLUDE = include/
SRC = src/marker.cpp src/calibration.cpp src/main.cpp

all:
	@$(CC) $(SRC) -I$(INCLUDE) $(CFLAGS) -framework GLUT -framework OpenGL -o gl
	
clean:
	@rm -f gl
