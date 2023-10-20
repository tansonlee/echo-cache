#ifndef CACHE_H
#define CACHE_H

#include "../shared/parser/parser.h"
#include <string>
#include <map>

class Cache {
public:
    Cache();
    ~Cache();

    HandlerResponse get(std::string key);
    HandlerResponse set(std::string key, std::string val);
    void dump();

private:
    std::map<std::string, std::string> map;
};


#endif