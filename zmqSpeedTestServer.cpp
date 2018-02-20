
//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <zmq.hpp>
#include <string>
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif

int main () {
    //  Prepare our context and socket
    void *context = zmq_ctx_new();

    void *radio = zmq_socket(context, 14);
    void *dish = zmq_socket(context, 15);

    int setupCode = zmq_connect(radio, "udp://127.0.0.1:5557");
    setupCode = zmq_connect(dish, "udp://127.0.0.1:5556");
    

    bool isRunning = true;
    while (isRunning) {
        //Receive Message from Client
        zmq_msg_t receivedMessage;

        zmq_msg_init(&receivedMessage);
        int returnCode = zmq_msg_recv(&receivedMessage, dish, 0);

        //Add in confirmation of floats

        //Send reply to client
        zmq_msg_t sendMessage;
        zmq_msg_init_size(&sendMessage, sizeof(returnCode));

        memcpy(zmq_msg_data(&sendMessage), &returnCode, sizeof(returnCode));

        zmq_msg_send(&sendMessage, radio, 0);
    }
    return 0;
}