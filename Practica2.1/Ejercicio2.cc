#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>
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

    bool finished = false;

    while(!finished){
        char buffer[80]; //Cadena para enviar-recibir mensajes

        struct sockaddr cliente;
        socklen_t clienteLenght = sizeof(struct sockaddr);

        int bytes = recvfrom(sd, buffer, 80, 0, &cliente, &clienteLenght); //Mensaje del cliente en buffer.
        
        if (bytes == -1){
            std::cout << "Error al recibir información del cliente.\n";
            return -1;
        }
        
        buffer[bytes] = '\0';

        char command = buffer[0];
        time_t t;

        switch(command){

            case('t'):
            time(&t);
            bytes = strftime(buffer, sizeof(buffer), "%r", localtime(&t)); //Mensaje del servidor al cliente en buffer
            sendto(sd, buffer, bytes, 0, &cliente, clienteLenght);
            break;

            case('d'):
            time(&t);
            bytes = strftime(buffer, sizeof(buffer), "%F", localtime(&t));
            sendto(sd, buffer, bytes, 0, &cliente, clienteLenght);
            break;

            case ('q'):
            finished = true;
            std::cout << "Fin del programa.\n";
            break;

            default:
            std::cout << "Comando no detectado.\n";
            break;
        }
    }
    
    close(sd);

    return 0;
}
