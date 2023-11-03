#ifndef CACHE_H
#define CACHE_H

#include <parser.h>
#include <string>
#include <map>
#include "lru_cache.h"

class Cache {
public:
    Cache(int maxSize);
    ~Cache();

    HandlerResponse get(std::string key);
    HandlerResponse set(std::string key, std::string val);
    HandlerResponse del(std::string key);


private:
    LRUCache* table;

    std::string compress(const std::string& value);
    std::string decompress(const std::string& compressedValue);

    std::string encrypt(const std::string& value);
    std::string decrypt(const std::string& encryptedValue);


};


#endif