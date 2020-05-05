all:
	g++ -std=c++11 -lpthread elevator.cpp -o Elevator
run:
	./elevator inp.txt
run2:
	./elevator inp2.txt