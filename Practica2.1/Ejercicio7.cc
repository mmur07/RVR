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
    MessageThread(int sd){socketDestination = sd;}

    void process_mssg(){
        bool finished = false;
        while(!finished){
            char buff[80];
            int bytesRec = recv(socketDestination, buff, sizeof(buff), 0); //Recieving mssg from client
            
            if (bytesRec <= 0){
                std::cout << "CONEXIÓN TERMINADA CON CLIENTE\n";
                finished = true;
            }
            
            else{
                buff[bytesRec] = '\0';
                int bytesSent = send(socketDestination, buff, bytesRec, 0); //Sending mssg to client        
            }
        }
        close(socketDestination);
    }
    private:
    int socketDestination;
};

int main(int argc, char** argv){
    struct addrinfo hints;
    struct addrinfo * res;

    memset((void *) &hints, 0, sizeof(addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (rc != 0){
        std::cerr << "[getaddrinfo]: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket(res->ai_family, res->ai_socktype, 0);

    if (sd == -1){
        std::cerr << "[socket]: " << gai_strerror(sd) << std::endl;
    }

    int bnd = bind(sd, res->ai_addr, res->ai_addrlen);

    if (bnd == -1){
        std::cout << "Error at binding\n";
    }

    freeaddrinfo(res);

    int lis = listen(sd, 16); //No puede ser menor que 16

    if (lis == -1){
        std::cout << "Error at listening\n";
        return -1;
    }

    struct sockaddr client;
    socklen_t clientLen = sizeof(struct sockaddr);

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];    
    
    while (true){
        int clientSocket = accept(sd, &client, &clientLen);

        if (clientSocket == -1){
            std::cout << "Error at accepting\n";
        }

        getnameinfo(&client, clientLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST| NI_NUMERICSERV);
        std::cout << "CONEXION DESDE HOST: " << host << " Port: " << serv << std::endl; 

        MessageThread* mt = new MessageThread(clientSocket);
		std::thread([&mt](){
			mt->process_mssg();

			delete mt;
		}).detach();
    }

    close(sd);
    std::cout << "Ending connection\n";

    return 0;
}
