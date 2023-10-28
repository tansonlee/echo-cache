#include "cpp_client.h"

#include <string>
#include <iostream>

#include <parser.h>
#include <socket_client.h>

RemoteCache::RemoteCache(const std::string& ip, int port) {
    this->ip = ip;
    this->port = port;
    this->establishConnection();
    this->maxRetries = 3;
}

RemoteCache::~RemoteCache() {
    if (this->client != nullptr) {
        this->closeConnection();
    }
}

// HACK: including <socket_client.h> in the header file does not work
// so set client to a void* then cast when using it.
SocketClient* giveType(void* client) {
    return (SocketClient*)client;
}

HandlerResponse RemoteCache::get(const std::string& key) {
    int retriesLeft = this->maxRetries;
    while (retriesLeft >= 1) {
        try {
            std::string response = giveType(this->client)->sendMessage("get||" + key);
            HandlerResponse parsed = parseResponseString(response);
            return parsed;
        } catch (...) {
            this->reestablishConnection();
        }
        --retriesLeft;
    }

    return {StatusCode::unexpectedError, ""};
}

bool RemoteCache::set(const std::string& key, const std::string& value) {
    int retriesLeft = this->maxRetries;
    while (retriesLeft >= 1) {
        try {
            std::string response = giveType(this->client)->sendMessage("set||" + key + "||" + value);
            return true;
        } catch (...) {
            this->reestablishConnection();
        }
        --retriesLeft;
    }
    return false;
}

void RemoteCache::reestablishConnection() {
    this->closeConnection();
    this->establishConnection();
}

void RemoteCache::establishConnection() {
    SocketClient* client = new SocketClient{this->ip, this->port};
    this->client = client;
}


void RemoteCache::closeConnection() {
    try {
        giveType(this->client)->sendMessage("end");
    } catch (...) {}

    delete giveType(this->client);
    this->client = nullptr;
}