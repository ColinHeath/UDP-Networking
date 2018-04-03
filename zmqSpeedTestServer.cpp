
//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <zmq.hpp>
#include <zmq.h>
#include <zmq_addon.hpp>
#include <zmq_draft.h>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>

int msg_send (zmq_msg_t *msg_, void *s_, const char* body_);
int msg_recv_cmp (zmq_msg_t *msg_, void *s_, const char* body_);

int main(int argc, char* argv[]) {
    //  Prepare our context and socket
    std::string expectedBody = "Hello";
    void *context = zmq_ctx_new();

    void *radio = zmq_socket(context, ZMQ_RADIO);
    void *dish = zmq_socket(context, ZMQ_DISH);

    int rc = zmq_connect(radio, "udp://127.0.0.1:29901");
    assert(rc == 0);

    rc = zmq_bind(dish, "udp://127.0.0.1:29900");
    assert(rc == 0);

    std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));

    bool isRunning = true;
    while (isRunning) {
        std::cout << "Waiting for Message" << std::endl;
        //Receive Message from Client
        zmq_msg_t receivedMessage;

        std::cout << "Message Initialized" << std::endl;
        
        rc = msg_recv_cmp(&receivedMessage, dish, expectedBody.c_str());

        std::cout << "Message Received" << std::endl;
        //Add in confirmation of floats

        //Send reply to client
        zmq_msg_t sendMessage;

        std::string content = "World";
        msg_send(&sendMessage, radio, content.c_str());
    }
    return 0;
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