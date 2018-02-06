#include <zmq.hpp>
#include <zmq_addon.hpp>
#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <array>

using namespace std;

float* constructFloatSet(int seed);
void recordTime(clock_t startClock);
//int msg_send(zmq_msg_t *msg_, void *s_, const char* group_, const char* body_);
//int msg_recv_cmp (zmq_msg_t *msg_, void *s_, const char* group_, const char* body_);

int main (int argc, char* argv[])
{
	if(argc == 2)
	{
		void *context = zmq_ctx_new();

		void *radio = zmq_socket(context, 14);
		void *dish = zmq_socket(context, 15);

		int rc = zmq_connect(radio, "udp://127.0.0.1:5556");
		rc = zmq_bind(dish, "udp://127.0.0.1:5557");		

	    float* positionArray;

	    ofstream output("clientOutput.txt");

	    cout << "Initialization Complete" << endl;

	    for (int request_nbr = 0; request_nbr < atoi(argv[1]); request_nbr++)
	    {
	    	cout << "Sending Message" << endl;
	    	
	    	positionArray = constructFloatSet(request_nbr);
	        clock_t startClock = clock();

	        cout << "Clock Started: " << startClock << endl;

	        //Send Message
	        zmq_msg_t message;
	        zmq_msg_init_size(&message, sizeof(*positionArray));
	        memcpy(zmq_msg_data(&message), positionArray, sizeof(*positionArray));

	        zmq_msg_send(&message, radio, 0);

	        zmq_msg_close(&message);
	        //Message Sent

	        zmq_msg_t receivedMessage;
	        zmq_msg_init(&receivedMessage);
	        int returnCode = zmq_msg_recv(&receivedMessage, dish, 0);

	        if(rc != -1)
	        {
	        	recordTime(startClock);
	        }
	        else
	        {
	        	output << "Failed" << endl;
	        }
	    }


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

	float* set = new float;

	for(int i = 0; i < 7; i++)
	{
		set[i] = rand();
	}

	return set;
}

void recordTime(clock_t startClock)
{
	clock_t endClock = clock();

	int duration = (endClock - startClock)/CLOCKS_PER_SEC;

	ofstream output("clientOutput.txt");

	output << duration << endl;
}

/*int msg_send (zmq_msg_t *msg_, void *s_, const char* group_, const char* body_)
{
    int rc = zmq_msg_init_size (msg_, strlen (body_));
    if (rc != 0)
        return rc;

    memcpy (zmq_msg_data (msg_), body_, strlen (body_));

    rc = zmq_msg_set_group (msg_, group_);
    if (rc != 0) {
        zmq_msg_close (msg_);
        return rc;
    }

    rc = zmq_msg_send (msg_, s_, 0);

    zmq_msg_close (msg_);

    return rc;
}

int msg_recv_cmp (zmq_msg_t *msg_, void *s_, const char* group_, const char* body_)
{
    int rc = zmq_msg_init (msg_);
    if (rc != 0)
        return -1;

    int recv_rc = zmq_msg_recv (msg_, s_, 0);
    if (recv_rc == -1) {
        zmq_msg_close(msg_);
        return -1;
    }

    if (strcmp (zmq_msg_group (msg_), group_) != 0)
    {
        zmq_msg_close (msg_);
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
}*/

