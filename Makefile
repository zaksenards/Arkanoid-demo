cc:= g++
LD:= -lraylib -lgdi32 -lwinmm
ID:= -I C:\raylib\raylib\src

all:
	$(cc) main.cpp $(LD) $(ID) -I . -Wall -g -o Hitnbreak.exe
