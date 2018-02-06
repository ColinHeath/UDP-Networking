#include <zmq.hpp>
#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>

using namespace std;

float* constructFloatSet(int seed);

int main (int argc, char* argv[])
{
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);

    std::cout << "Connecting to test server..." << std::endl;
    socket.connect ("tcp://localhost:5555");

    //  Do 10 requests, waiting each time for a response
    cout << "Creating Arrays" << endl;
    float* positionArray;

    int frequencyArray[10000];
    cout << "Arrays Created" << endl;
    cout << "Attempting to Print argv[1]" << endl;
    cout << atoi(argv[1]) << endl;
    cout << "Success" << endl;

    for (int request_nbr = 0; request_nbr < atoi(argv[1]); request_nbr++)
    {
    	srand(request_nbr);
    	cout << "Sending Message" << endl;
    	
    	positionArray = constructFloatSet(request_nbr);

        zmq::message_t request (28);
        memcpy(request.data(), positionArray, 28);

        clock_t startClock = clock();

        socket.send(request);

        //  Get the reply.
        zmq::message_t reply;
        socket.recv(&reply);

        clock_t endClock = clock();

        cout << "Response Received" << endl;
	    frequencyArray[(int)(((double)(endClock - startClock))/(double)CLOCKS_PER_SEC)]++;
    }

    return 0;
}

float* constructFloatSet(int seed)
{
	srand(seed);

	float* set = new float;

	for(int i = 0; i < 7; i++)
	{
		set[i] = rand();
	}

	return set;
}