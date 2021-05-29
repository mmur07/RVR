#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, 80);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t data_size = 80 * sizeof(char) + 2 * sizeof(int16_t);
        alloc_data(data_size);

        char* tmp = _data;

        memcpy(tmp, name, 80 * sizeof(char));
        
        tmp += 80 * sizeof(char);

        memcpy(tmp, &x, sizeof(int16_t));

        tmp += sizeof(int16_t);

        memcpy(tmp, &y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        char* tmp = data;

        memcpy(name, tmp, sizeof(char));

        tmp += 80 * sizeof(char);

        memcpy(&x, tmp, sizeof(int16_t));

        tmp += sizeof(int16_t);

        memcpy(&y, tmp, sizeof(int16_t));

        return 0;
    }

    int from_file(){
        char* buff = (char*)malloc(sizeof(char)*80+sizeof(int16_t)*2);

        int fd = open("./data_jugador", O_RDONLY);
        read(fd,buff,sizeof(char)*80+sizeof(int16_t)*2);
    	close(fd);
        from_bin(buff);

        return 0;
    }


public:
    char name[80];
    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("Player_ZERO", 12, 45);
    Jugador one_w("Player_ONE", 4545, -4545);

    // 1. Serializar el objeto one_w
    one_w.to_bin();

    int fd = open("./data_jugador", O_CREAT | O_TRUNC | O_RDWR, 0666);

    // 2. Escribir la serialización en un fichero
    write(fd, one_w.data(), one_w.size());

    close(fd);

    // 5. Leer desde archivo.
    one_r.from_file();

    std::cout << "Player name: " << one_r.name << " Pos_x: " << one_r.x << " Pos_y: " << one_r.y << std::endl; 


    return 0;
}

