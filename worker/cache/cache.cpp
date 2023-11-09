#include "cache.h"
#include <iostream>
#include <string>

// Cache::Cache(int maxSize): table(new LRUCache(maxSize)) {}
Cache::Cache(int maxSize): table(new std::map<std::string, std::string>()) {}

Cache::~Cache() {
    // delete table;
}

HandlerResponse Cache::get(std::string key) {
    try {
        // std::string value = this->table->get(key);
        std::string value = this->table->at(key);
        return {StatusCode::success, value};

    } catch (...) {
        return {StatusCode::keyNotFound, "Value for key not found: '" + key + "'"};
    }
}

HandlerResponse Cache::set(std::string key, std::string value) {
    // this->table->set(key, value);
    (*this->table)[key] = value;
    return {StatusCode::success, "Got it!"};
}

HandlerResponse Cache::del(std::string key) {
    // this->table->del(key);
    this->table->erase(key);
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
