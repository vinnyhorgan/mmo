#include "network.h"

#include <enet/enet.h>

#include <iostream>

ENetHost* client;
ENetAddress address;
ENetPeer* server;
ENetEvent event;

void connect() {
    if (enet_initialize() != 0) {
        std::cout << "Error initializing ENet" << std::endl;
        return;
    }

    client = enet_host_create(NULL, 1, 1, 0, 0);
    if (client == NULL) {
        std::cout << "Error creating client" << std::endl;
        return;
    }

    enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;

    server = enet_host_connect(client, &address, 1, 0);
    if (server == NULL) {
        std::cout << "Error connecting to server" << std::endl;
        return;
    }
}

void disconnect() {
    if (server != NULL) {
        enet_peer_disconnect(server, 0);

        while (enet_host_service(client, &event, 3000) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE:
                    enet_packet_destroy(event.packet);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout << "Disconnected from server" << std::endl;
                    break;
            }
        }
    }

    if (client != NULL) {
        enet_host_destroy(client);
    }

    client = NULL;
    server = NULL;

    enet_deinitialize();
}

void update() {
    if (enet_host_service(client, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "Connected to server" << std::endl;
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << "Disconnected from server" << std::endl;
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                std::cout << "Received packet from server" << std::endl;
                break;
        }
    }
}
