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
argv[2] = comando
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

    int bytes = sendto(sd, argv[3], 2, 0, res->ai_addr, res->ai_addrlen);

    if (bytes != 2){ //Message should be X\n"
        std::cout << "Error at sending message.\n";
    }

    freeaddrinfo(res);

    char buffer[80];
    memset(buffer, 0, sizeof(buffer));
    bytes = recvfrom(sd, buffer, sizeof(buffer), 0, res->ai_addr, &res->ai_addrlen);

    if (bytes == -1){
        std::cout << "Error at recieving message.\n";
    }
    buffer[bytes] = '\0';
    std::cout << buffer << std::endl;

    return 0;
}
