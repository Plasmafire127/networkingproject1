all: 
	g++ webserver1.cpp -o webserver1
	g++ -std=c++17 webserver2.cpp -o webserver2

clean:
	rm webserver1 webserver2
