all:compile executable execute

compile:
	g++ -I./include -c ./src/test.cpp -o ./lib/test.o
	
executable:
	g++ ./lib/test.o -o ./bin/test
execute:
	./bin/test