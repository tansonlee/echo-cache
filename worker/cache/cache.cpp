#include "cache.h"
#include <iostream>

Cache::Cache(int maxSize): table(new LRUCache(maxSize)) {}

Cache::~Cache() {
    delete table;
}

HandlerResponse Cache::get(std::string key) {
    try {
        std::string value = this->table->get(key);
        return {StatusCode::success, value};

    } catch (...) {
        return {StatusCode::keyNotFound, "Value for key not found: '" + key + "'"};
    }
}

HandlerResponse Cache::set(std::string key, std::string value) {
    this->table->set(key, value);
    return {StatusCode::success, "Got it!"};
}

HandlerResponse Cache::del(std::string key) {
    this->table->del(key);
    return {StatusCode::success, "Deleted!"};
}

std::string compress(const std::string& value) {
    return "";
}

std::string decompress(const std::string& compressedValue) {
    return "";
}


std::string encrypt(const std::string& value) {
    return "";
}

std::string decrypt(const std::string& encryptedValue) {
    return "";
}
