#target entry to build program executable named wrapP from optimise.cpp
#defining the compiler to be used	
CC=g++
#compiler flags:
#-g is used here to add dubugging info to the executable file
#we us -Wall to turn on complier warning
CFLAGS=-Wall
#our exicutable file 
EXCICUTABLE=optimise
#our opencv library
LI=`pkg-config --cflags --libs opencv`

all: $(EXCICUTABLE).cpp
	$(CC) $(EXCICUTABLE).cpp -o $(EXCICUTABLE) $(CFLAGS) -pthread -std=c++11 $(LI)

.PHONY:clean
clean:
	$(RM) $(EXCICUTABLE)		
#g++ cam.cpp -o video -pthread -std=c++11 `pkg-config --cflags --libs opencv`
