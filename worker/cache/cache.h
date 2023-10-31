#ifndef CACHE_H
#define CACHE_H

#include <parser.h>
#include <string>
#include <map>
#include "hashtable.h"

class Cache {
public:
    Cache();
    ~Cache() = default;

    HandlerResponse get(std::string key);
    HandlerResponse set(std::string key, std::string val);

private:
    HashTableCuckoo table;

    std::string compress(const std::string& value);
    std::string decompress(const std::string& compressedValue);

    std::string encrypt(const std::string& value);
    std::string decrypt(const std::string& encryptedValue);


};


#endif