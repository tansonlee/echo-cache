#ifndef CPP_CLIENT_H
#define CPP_CLIENT_H

#include <string>
#include <parser.h>

class RemoteCache {
  public:
    RemoteCache(const std::string& ip, int port);
    ~RemoteCache() = default;

    HandlerResponse get(const std::string& key);
    bool set(const std::string& key, const std::string& value);
  private:
    std::string ip;
    int port;
};

#endif