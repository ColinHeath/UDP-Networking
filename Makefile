all: clean client server

client: 
	g++ -g -Wall -o client zmqSpeedTestClient.cpp -std=c++11 -lzmq

server: 
	g++ -g -Wall -o server zmqSpeedTestServer.cpp -std=c++11 -lzmq

clean:
	-@rm -rf ./client
	-@rm -rf ./server