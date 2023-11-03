#ifndef CPP_CLIENT_H
#define CPP_CLIENT_H

#include <string>
// #include <socket_client.h>
#include <parser.h>

class SocketClient;

class RemoteCache {
  public:
    RemoteCache(const std::string& ip, int port);
    ~RemoteCache();

    HandlerResponse get(const std::string& key);
    bool del(const std::string& key);
    bool set(const std::string& key, const std::string& value);
    void initiateAndCloseConnection();

  private:
    void establishConnection();
    void reestablishConnection();
    void closeConnection();

    SocketClient* client;
    std::string ip;
    int port;
    int maxRetries;
};

#endif