#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include "hashtable.h"
#include <string>

class LRUCache {
  public:
    LRUCache(int capacity);
    ~LRUCache();

	void set(std::string key, std::string value);
	std::string get(const std::string& key);
	void del(const std::string& key);
  private:
    struct Node {
        std::string key;
        std::string value;
        Node* next;
        Node* prev;
    };

    HashTableCuckoo<Node*>* hashtable;
    int capacity;
    int size;

    Node* head;
    Node* tail;

    void llRemove(Node* node);
    void llAdd(Node* node);
    void llFree();
};

#endif