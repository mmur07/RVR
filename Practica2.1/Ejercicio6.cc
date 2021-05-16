#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <thread>

#define N_THREADS 5
#define SLEEP_TIME 3

/*
argv[0] = addr
argv[1] = puerto
*/

class MessageThread{
    public:
    MessageThread(int sd, int id){socketDestination = sd; threadId = id;}

    void process_mssg(){
        while(true){
            char buff[80];

            char host[NI_MAXHOST];
            char serv[NI_MAXSERV];

            struct sockaddr client;
            socklen_t clientLen = sizeof(sockaddr);

            bool finished = false;
            while(!finished){
                int bytes = recvfrom(socketDestination, (void *)buff, sizeof(buff), 0, &client, &clientLen);

                buff[bytes] = '\0';

                int nInfo = getnameinfo(&client, clientLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST);
                std::cout << "Recibidos " << bytes << " bytes desde " << host << ":" << serv << "en el thread" << threadId << std::endl;

                sleep(SLEEP_TIME);

                char command = buff[0];
                time_t t;
                switch(command){
                    case('t'):
                        time(&t);
                        bytes = strftime(buff, sizeof(buff), "%r", localtime(&t)); //Mensaje del servidor al cliente en buffer
                        sendto(socketDestination, buff, bytes, 0, &client, clientLen);
                    break;

                    case('d'):
                        time(&t);
                        bytes = strftime(buff, sizeof(buff), "%F", localtime(&t));
                        sendto(socketDestination, buff, bytes, 0, &client, clientLen);
                    break;

                    default:
                        std::cout << "Comando no detectado.\n";
                    break;
                }
            }
            close(socketDestination);
        }
    }
    private:
    int socketDestination;
    int threadId;
};

int main(int argc, char** argv){
    struct addrinfo hints;
    struct addrinfo * res;

    memset((void *) &hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (rc != 0){
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    if (sd == -1){
        std::cerr << "[socket]: " << gai_strerror(sd) << std::endl;
    }

    bind(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);

    for (int k = 0; k < N_THREADS; k++)
    {
        MessageThread *mt = new MessageThread(sd, k);

        std::thread([&mt](){
            mt->process_mssg();

            delete mt;
        }).detach();
    }
    char input;
    bool finished = false;

    while (!finished){
        std::cin >> input;
        finished = (input == 'q');
    }
    
    close(sd);
    std::cout << "Ending connection\n";

    return 0;
}
