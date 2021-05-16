#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

/*
argv[0] = addr
argv[1] = puerto
*/

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
        return -1;
    }

    int bnd = bind(sd, res->ai_addr, res->ai_addrlen);

    if(bnd == -1) {
        std::cout << "Error at binding\n";
        return -1;
    }

    int lis = listen(sd, 16); //No puede ser menor que 16

    if (lis == -1){
        std::cout << "Error at listening\n";
        return -1;
    }

    struct sockaddr cliente;
    socklen_t clienteLen = sizeof(sockaddr);

    int cliente_sd = accept(sd, &cliente, &clienteLen);

    if (cliente_sd == -1){
        std::cout << "Error at accept\n";
    }

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo(&cliente, clienteLen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST| NI_NUMERICSERV);

    std::cout << "CONEXION DESDE HOST: " << host << " Port: " << serv << std::endl; 

    bool finished = false;
    while (!finished){
        char buffer[80];

        int bytesRec = recv(cliente_sd, (void*)buffer, sizeof(buffer), 0); //Bytes recieved from client
        if (bytesRec <= 0){
            std::cout << "CONEXIÓN TERMINADA\n";
            finished = true;
        }
        else{
            buffer[bytesRec] = '\0';

            int bytesSent = send(cliente_sd, buffer, bytesRec, 0); //Bytes sent from server
        }
    }
    close(sd);
    return 0;
}
