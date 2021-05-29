#include <string.h>

#include "Serializable.h"
#include "Socket.h"
#include "Socket.cc"
#include <iostream>

void compare(Socket s1, Socket s2){
    if (s1 == s2) std::cout << s1 << " " << s2 << ": IGUALES\n";
    else std::cout << s1 << " " << s2 << ": NO IGUALES\n";
}

int main(int argc, char **argv)
{
    Socket sock_1("localhost", "1212");
    sock_1.bind();
    Socket sock_2("127.0.0.3", "1212");
    sock_2.bind();
    Socket sock_3("localhost", "1212");
    sock_3.bind();
    Socket sock_4("localhost", "3245");
    sock_3.bind();

    compare(sock_1, sock_2);
    compare(sock_1, sock_3);
    compare(sock_1, sock_4);

    return 0;
}
