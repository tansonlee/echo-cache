#include "cache.h"
#include <iostream>

Cache::Cache() {}

HandlerResponse Cache::get(std::string key) {
    std::string value = this->table.get(key);
    if (value.empty()) {
        return {StatusCode::keyNotFound, "Value for key not found: '" + key + "'"};
    }

    return {StatusCode::success, value};
}

HandlerResponse Cache::set(std::string key, std::string value) {
    this->table.set(key, value);
    return {StatusCode::success, "Got it!"};
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
