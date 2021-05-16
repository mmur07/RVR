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
    char buff[80];

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

    int cnt = connect(sd, res->ai_addr, res->ai_addrlen);

    if (cnt == -1){
        std::cout << "Error at connecting\n";
        return -1;
    }

    bool finished = false;
    while (!finished){
        std::cin >> buff;
        if (buff[0] == 'Q') finished = true;
        else{
            int bytesSent = send(sd, buff, sizeof(buff), 0); //Bytes sent to server

            if (bytesSent <= 0){
                std::cout << "Error at sending message\n";
                return -1;
            }

            int bytesRec = recv(sd, buff, sizeof(buff), 0); //Bytes recieved from server
            buff[bytesRec] = '\0';
            std::cout << buff << std::endl;
        }
    }

    std::cout << "Ending connection\n";
    return 0;
}
