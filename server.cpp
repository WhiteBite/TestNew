#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <thread>

#include <iostream>
#include <vector>
#include <unistd.h>

const int BUFFER_SIZE = 256;
const int CLIENTS = 4;
const int port = 25560;

struct client {
    int sock;
    std::string msg;
    int place;

    client(std::string _recv, int _place) {
        msg = _recv;
        place = _place;
    }

    ~client() {
        msg = "";
        place = 0;
    }
};

std::vector<client> myQueue;
int socket_type;



void mySender(int place, int msgsock) {
    int retval = 0;
    std::string to_send = "Your place is [";
    to_send += std::to_string(place);
    to_send += "]";
    retval = send(msgsock, to_send.c_str(), 128, 0);
    if (retval == -1) {
        fprintf(stderr, "Server: send() failed: error \n");
    } else
        printf("Server: send() retval =%d || msg=%s OK.\n",retval,to_send.c_str());
}

void que(int msgsock) {

    printf("que func()\n");
    while (1) {
        if (myQueue.size() > 0) {
            printf("Click to pop queue\n");
            std::string go = "no";
            std::getline(std::cin, go);
            if (go == "go") {
                for (int i = 0; i < myQueue.size(); i++) {
                    mySender(i - 1, msgsock);
                }
                myQueue.erase(myQueue.begin());
            } else if (go == "list") {
                for (int i = 0; i < myQueue.size(); i++) {
                    printf("[%d] %s\n", i, myQueue[i].msg.c_str());
                }
            }
        }
    }
}
int recvall(int s, char* buf, int len = BUFFER_SIZE, int flags = 0)
{
    int total = 0;
    int n;
    while (total < len)
    {
        n = recv(s, buf + total, len - total, flags);
        if (!n)
            break;
        total += n;
    }
    return (!n ? 0 : total);
}
void work(int listen_socket) {
    printf("work func()\n");
    int retval;
    char Buffer[128];
    int msgsock;

    int place = 0;
    int i=0;
    while (1) {

        //for(int i =0;i<128;i++){
        //    Buffer[i]='\0';
        //}
        int AcceptSocket;
        printf("Waiting for a client to connect...\n");
        while (1) {
            AcceptSocket = accept(listen_socket, NULL, NULL);
            if (AcceptSocket < 0)
                break;
            printf("Client Connected.\n");
            listen_socket = AcceptSocket;
            break;
        }
        printf("\n====Listen %d=====\n",i++);
        retval = recv(listen_socket, Buffer, 128, 0);

        if (retval == -1) {
            fprintf(stderr, "Server: recv() failed: error \n");
            printf("Server: recv() %s\n",Buffer);
            close(listen_socket);
            //	closesocket(msgsock);
            return;
        } else {
            printf("Server: recv() is OK.\n");
            printf("recv: %s\n", Buffer);
            std::string msg = Buffer;
            bool need_add = true;
            for (int i = 0; i < myQueue.size(); i++) {
                if (myQueue[i].msg == msg)
                    need_add = false;
            }
            if (need_add) {
                myQueue.push_back(client(msg, myQueue.size()));
                place = myQueue.size() - 1;
                mySender(place, listen_socket);
            }

        }
        if (retval == 0) {
            printf("Server: Client closed connection.\n");
            close(listen_socket);
            return;
        }
        printf("Server: Received %d bytes, data \"%s\" from client\n", retval, Buffer);
        printf("Server: Echoing the same data back to client...\n");
        //	mySender();
    }
}

int main(int argc, char **argv) {
    // Create a socket.
    int m_socket;
    int clients[CLIENTS];
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    if (m_socket < 0) {
        printf("Error at socket()\n");
        return 1;
    }
    // Bind the socket.

    struct sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = htonl(INADDR_ANY);
    service.sin_port = htons(port);


    if (bind(m_socket, (struct sockaddr *) &service, sizeof(service)) < 0) {
        printf("bind() failed.\n");
        //close(m_socket);
        return 1;
    }

//    // Listen on the socket.
    listen(m_socket, 1);
//    // Accept connections.
//    int AcceptSocket;
//    printf("Waiting for a client to connect...\n");
//    while (1) {
//        AcceptSocket = accept(m_socket, NULL, NULL);
//        if (AcceptSocket < 0)
//            break;
//        printf("Client Connected.\n");
//        m_socket = AcceptSocket;
//        break;
//    }

//std::thread q(que,m_socket);
    // Send and receive data.
    int bytesSent;
    int bytesRecv;

    int Buffer[128] ={0};
    //recv(m_socket, Buffer, 128, 0);
    printf("buf = %s\n",Buffer);
    printf("BEGIN\n");
    //  std::thread(que,m_socket);
    std::thread thr(work,m_socket);
    std::thread kek(que,m_socket);

    //std::thread work(work, m_socket);
    //std::thread que(que, m_socket);
    //
    thr.join();
    kek.join();
    /* bytesRecv = recv(m_socket, recvbuf, BUFFER_SIZE, 0);
     printf("Bytes Recv: %d\n", bytesRecv);
     //std::cout << recvbuf << "\n";

     bytesSent = sendall(m_socket, "KEK", BUFFER_SIZE, 0);
     printf("Bytes Sent: %d\n", bytesSent);*/
//close(m_socket);

    return 0;
}

