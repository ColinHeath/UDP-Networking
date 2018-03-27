
//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <zmq.hpp>
#include <zmq_draft.h>
#include <string>
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif

int main(int argc, char* argv[]) {
    //  Prepare our context and socket
    void *context = zmq_ctx_new();

    void *radio = zmq_socket(context, ZMQ_RADIO);
    void *dish = zmq_socket(context, ZMQ_DISH);

    int rc = zmq_connect(radio, "udp://127.0.0.1:5557");
    assert(rc == 0);

    rc = zmq_bind(dish, "udp://127.0.0.1:5556");
    assert(rc == 0);

    bool isRunning = true;
    while (isRunning) {
        std::cout << "Waiting for Message" << std::endl;
        //Receive Message from Client
        zmq_msg_t receivedMessage;

        rc = zmq_msg_init(&receivedMessage);
        assert(rc == 0);

        std::cout << "Message Initialized" << std::endl;
        rc = zmq_msg_recv(&receivedMessage, dish, 0);
        if(rc == -1)
        {
            zmq_msg_close(&receivedMessage);
        }

        std::cout << "Message Received" << std::endl;
        //Add in confirmation of floats

        //Send reply to client
        zmq_msg_t sendMessage;
        zmq_msg_init_size(&sendMessage, sizeof(rc));

        memcpy(zmq_msg_data(&sendMessage), &rc, sizeof(rc));

        zmq_msg_send(&sendMessage, radio, 0);
    }
    return 0;
}