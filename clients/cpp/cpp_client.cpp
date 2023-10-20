#include "cpp_client.h"

#include <string>

#include <socket_client.h>
#include <parser.h>

RemoteCache::RemoteCache(const std::string& ip, int port) {
    this->ip = ip;
    this->port = port;
}

HandlerResponse RemoteCache::get(const std::string& key) {
    SocketClient client{this->ip, this->port};
    std::string response = client.sendMessage("get||" + key);
    HandlerResponse parsed = parseResponseString(response);
    return parsed;
}

bool RemoteCache::set(const std::string& key, const std::string& value) {
    SocketClient client{this->ip, this->port};
    std::string response = client.sendMessage("set||" + key + "||" + value);
    return true;
}
