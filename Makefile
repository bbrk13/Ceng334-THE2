all:
	g++ -std=c++11 -lpthread elevator.cpp -o elevator
run:
	./elevator inp.txt