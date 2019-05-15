CC = g++
CFLAGS = `pkg-config --libs --cflags opencv4` -Wall
SRC = ar.cpp

build: 
	@$(CC) $(SRC) -o $@ -std=c++11 -I$(INCLUDE) $(CFLAGS) 
	
clean:
	@rm -f build
