#include <string.h>

#include "Serializable.h"
#include "Socket.h"
#include "Socket.cc"
#include "Chat.h"
#include "Chat.cc"
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    std::cout << "HOLA\n";
    ChatMessage mssg1 = ChatMessage("Usuario 1", "Hola!");
    int fd = open("./data_mensaje", O_CREAT | O_TRUNC | O_RDWR, 0666);

    // 1. Serializar el objeto one_w
    mssg1.to_bin();

    // 2. Escribir la serialización en un fichero
    write(fd, mssg1.data(), mssg1.size());

    fd = open("./data_jugador", O_RDONLY);

    // 3. Leer el fichero
    read(fd, mssg1.data(), mssg1.size());

    close(fd);

    // 4. "Deserializar" en one_r
    mssg1.from_bin(mssg1.data());

    close(fd);

    // 5. Mostrar el contenido de one_r
    //No funciona. Solo consigo mostrar la dirección en memoria.
    mssg1.get_info();
    return 0;
}
