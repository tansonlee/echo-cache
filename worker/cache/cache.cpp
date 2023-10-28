#include "cache.h"
#include <iostream>

Cache::Cache() {}

void Cache::dump() {
    for(auto it = this->map.begin(); it != this->map.end(); ++it) {
        std::cout << it->first << " " << it->second << " " << "\n";
    }
}

HandlerResponse Cache::get(std::string key) {
    auto found = this->map.find(key);
    if (found == this->map.end()) {
        return {StatusCode::keyNotFound, "Value for key not found: '" + key + "'"};
    }

    return {StatusCode::success, found->second};
}

HandlerResponse Cache::set(std::string key, std::string val) {
    this->map[key] = val;
    return {StatusCode::success, "Got it!"};
}
