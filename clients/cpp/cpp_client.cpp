#include "cpp_client.h"

#include <string>
#include <iostream>

#include <parser.h>
#include <socket_client.h>
#include <exception>

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

HandlerResponse RemoteCache::get(const std::string& key) {
    int retriesLeft = this->maxRetries;
    while (retriesLeft >= 1) {
        try {
            this->client->sendMessage("get||" + key);
            std::string response = this->client->receiveResponse();
            if (response.empty()) {
                throw std::runtime_error("bad connection");
            }
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
            this->client->sendMessage("set||" + key + "||" + value);
            std::string response = this->client->receiveResponse();
            if (response.empty()) {
                throw std::runtime_error("bad connection");
            }
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
    delete this->client;
    this->client = nullptr;
}

void RemoteCache::initiateAndCloseConnection() {
    try {
        this->client->sendMessage("end");
    } catch (...) {}

    this->closeConnection();
}