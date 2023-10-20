#include "cpp_client.h"

#include <string>

#include <socket_client.h>

RemoteCache::RemoteCache(const std::string& ip, int port) {
    this->ip = ip;
    this->port = port;
}

std::string RemoteCache::get(const std::string& key) {
    SocketClient client{this->ip, this->port};
    std::string response = client.sendMessage("get||" + key);
    return response;
}

bool RemoteCache::set(const std::string& key, const std::string& value) {
    SocketClient client{this->ip, this->port};
    std::string response = client.sendMessage("set||" + key + "||" + value);
    return true;
}
