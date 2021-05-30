#include "Chat.h"
#include "Socket.h"
#include <bits/stdint-uintn.h>
#include <cstring>
#include <malloc.h>
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    char* tmp = _data;

    memcpy(tmp, &type, sizeof(uint8_t));
    tmp += sizeof(uint8_t);

    memcpy(tmp, nick.c_str(), sizeof(char) * 8);
    tmp += sizeof(char) * 8;

    memcpy(tmp, message.c_str(), sizeof(char) * 80);
    tmp += sizeof(char) * 80;
}

int ChatMessage::from_bin(char * bobj)
{
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);

    //Reconstruir la clase usando el buffer _data
    char* tmp = _data;

    memcpy(&type, tmp, sizeof(uint8_t));
    tmp += sizeof(uint8_t);

    nick = tmp;
    tmp += sizeof(char) * 8;

    message = tmp;
    tmp += sizeof(char) * 80;

    return 0;
}

void ChatMessage::get_info()
{
    //Si les quito el '&' lanza un error de segmentation fault. He intentado arreglarlo pero no lo consigo.
    //std::cout << "Nick :" << nick.c_str() << ", Type: " << &type << ", Mssg: " << message.c_str() << "\n";
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {

        /*
         * NOTA: los clientes están definidos con "smart pointers", es necesario
         * crear un unique_ptr con el objeto socket recibido y usar std::move
         * para añadirlo al vector
         */
        
        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)

        Socket *client;
        ChatMessage msg;
        socket.recv(msg, client);
        switch(msg.type){
            case ChatMessage::LOGIN:
                {
                    std::unique_ptr<Socket> ptr = std::make_unique<Socket>(*client);
                    clients.push_back(std::move(ptr));
                    std::cout << "LOGIN from: " << msg.nick << "@" << *client << "\n";
                }
            break;

            case ChatMessage::MESSAGE:
                std::cout << "MESSAGE from: " << msg.nick << "@" << *client << "\n";
                for (auto it = clients.begin(); it != clients.end(); ++it){
                    if (!(**it == *client)) socket.send(msg, **it);
                }
            break;

            case ChatMessage::LOGOUT:
                {
                    std::cout << "LOGOUT from: " << msg.nick << "@" << *client << "\n";
                    bool found = false;
                    for (auto it = clients.begin(); found == false && it != clients.end(); ++it){
                        if (**it == *client){
                            it = clients.erase(it);
                            found = true;
                        }
                        else ++it;
                    }

                    for (auto it = clients.begin(); it != clients.end(); ++it){
                        if (!(**it == *client)){
                            msg.type = ChatMessage::MESSAGE;
                            msg.message = " I just left the chat room. Bye!\n";
                            socket.send(msg, **it);
                        }
                    }
                }
            break;
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatClient::login()
{
    std::string msg;
    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
}

void ChatClient::logout()
{
    std::string msg;
    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;
    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    while (true)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
        std::string msg;
        std::getline(std::cin, msg);

        if (msg == "q"){
            logout();
            break;
        }
        else{
            ChatMessage em(nick, msg);
            em.type = ChatMessage::MESSAGE;
            socket.send(em, socket);
        }
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        // Recibir Mensajes de red
        // Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        ChatMessage em;

        socket.recv(em);
        std::cout << em.nick << ": " << em.message << "\n";
    }
}

