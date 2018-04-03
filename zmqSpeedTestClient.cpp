#include <zmq.hpp>
#include <zmq.h>
#include <zmq_addon.hpp>
#include <zmq_draft.h>
#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <chrono>
#include <thread>
using namespace std;

float* constructFloatSet(int seed);
void recordTime(clock_t startClock);
int msg_send(zmq_msg_t *msg_, void *s_, const char* body_);
int msg_recv_cmp (zmq_msg_t *msg_, void *s_, const char* body_);

int main (int argc, char* argv[])
{
	if(argc == 2)
	{
		void *context = zmq_ctx_new();
		assert(context);

		void *radio = zmq_socket(context, ZMQ_RADIO);
		void *dish = zmq_socket(context, ZMQ_DISH);

		int rc = zmq_connect(radio, "udp://127.0.0.1:29900");
		assert(rc == 0);

		rc = zmq_bind(dish, "udp://127.0.0.1:29901");		
		assert(rc == 0);

	    ofstream output("clientOutput.txt");

	    cout << "Sleep Started" << endl;

	    this_thread::sleep_until(chrono::system_clock::now() + chrono::seconds(1));

	    cout << "Initialization Complete" << endl;

	    for (int request_nbr = 0; request_nbr < atoi(argv[1]); request_nbr++)
	    {
	    	float* positionArray = constructFloatSet(request_nbr);
		    zmq_msg_t message;

	    	cout << "Preparing Message" << endl;
	        clock_t startClock = clock();

	        cout << "Clock Started: " << startClock << endl;

	        //Send Message
	        std::string content = "Hello";
			assert(msg_send(&message, radio, content.c_str()) == (int)content.size());

	        std::cout << "Message Sent" << std::endl;
	        //Message Sent

	        zmq_msg_t receivedMessage;

	        std::cout << "Receive Initialized" <<std::endl;

	        rc = msg_recv_cmp(&receivedMessage, dish, 0);
	        if(rc != -1)
	        {
	        	std::cout << "Proper Return Code" << std::endl;
	        }
	        else
	        {
	        	output << "Message Receive Failed" << endl;
	        }

       	    delete [] positionArray;
	    }

	    std::cout << "Outside of for" << std::endl;

	    zmq_close(dish);
	    zmq_close(radio);

	    zmq_ctx_term(context);

	    return 0;
	}
	else
	{
		cout << "Please Enter the Proper Number of Arguments" << endl;
		return -1;
	}
}

float* constructFloatSet(int seed)
{
	srand(seed);
	std::cout << "Seed Planted" << std::endl;
	float* set = new float[7];
	std::cout << "Set declared" << std::endl;

	for(int i = 0; i < 7; i++)
	{
		set[i] = rand();
	}
	std::cout << "Set Constructed" << std::endl;
	return set;
}

void recordTime(clock_t startClock)
{
	clock_t endClock = clock();

	int duration = (endClock - startClock)/CLOCKS_PER_SEC;

	ofstream output("clientOutput.txt");

	output << duration << endl;
}

int msg_send (zmq_msg_t *msg_, void *s_, const char* body_)
{
    int rc = zmq_msg_init_size (msg_, strlen (body_));
    if (rc != 0)
        return rc;

    memcpy (zmq_msg_data (msg_), body_, strlen (body_));

    rc = zmq_msg_send (msg_, s_, 0);

    zmq_msg_close (msg_);

    return rc;
}

int msg_recv_cmp (zmq_msg_t *msg_, void *s_, const char* body_)
{
    int rc = zmq_msg_init (msg_);
 
 	std::cout << "Receive Initialized" << std::endl;

    if (rc != 0)
        return -1;

    int recv_rc = zmq_msg_recv (msg_, s_, 0);

    std::cout << "Message Received" << std::endl;
    if (recv_rc == -1) {
        zmq_msg_close(msg_);
        return -1;
    }

    char * body = (char*) malloc (sizeof(char) * (zmq_msg_size (msg_) + 1));
    memcpy (body, zmq_msg_data (msg_), zmq_msg_size (msg_));
    body [zmq_msg_size (msg_)] = '\0';

    if (strcmp (body, body_) != 0)
    {
        zmq_msg_close (msg_);
        free(body);
        return -1;
    }

    zmq_msg_close (msg_);
    free (body);
    return recv_rc;
}

