CPP=g++
FLAGS=-Wall -O3

.PHONY: clean
msolver: main.cpp main.h
	$(CPP) $(FLAGS) $< -o $@

clean:
	rm msolver
