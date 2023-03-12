#include <enet/enet.h>

#include <iostream>

int main() {
    if (enet_initialize() != 0) {
        std::cout << "Error initializing ENet" << std::endl;
        return 1;
    }

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    ENetHost* server = enet_host_create(&address, 32, 1, 0, 0);
    if (server == NULL) {
        std::cout << "Error creating server" << std::endl;
        return 1;
    }

    ENetEvent event;

    while (true) {
        while (enet_host_service(server, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                    std::cout << "Client connected" << std::endl;
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    std::cout << "Packet received" << std::endl;
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "Client disconnected" << std::endl;
                    break;
            }
        }
    }

    enet_host_destroy(server);

    enet_deinitialize();
}
