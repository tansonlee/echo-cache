#include "lru_cache.h"
#include "hashtable.h"

LRUCache::LRUCache(int capacity) {
    this->hashtable = new HashTableCuckoo<Node*>();
    this->capacity = capacity;
    this->size = 0;

    this->head = new Node();
    this->head->key = "";
    this->head->prev = nullptr;
    this->head->next = this->tail;

    this->tail = new Node();
    this->tail->key = "";
    this->tail->prev = this->head;
    this->tail->next = nullptr;
}

LRUCache::~LRUCache() {
    delete this->hashtable;
    this->llFree();
}

void LRUCache::llFree() {

}

void LRUCache::set(std::string key, std::string value) {
    try {
        Node* found = this->hashtable->get(key);
        this->llRemove(found);
        this->hashtable->del(key);
    } catch (...) {}

    Node* newNode = new Node();
    newNode->key = key;
    newNode->value = value;
    this->llAdd(newNode);
    this->hashtable->set(key, newNode);

    if (this->size > this->capacity) {
        std::cerr << "I am at capacity, deleting now" << std::endl;
        Node* removeNode = this->head->next;
        this->llRemove(removeNode);
        this->hashtable->del(removeNode->key);
        delete removeNode;
    }
}

std::string LRUCache::get(const std::string& key) {
    try {
        Node* result = this->hashtable->get(key);
        this->llRemove(result);
        this->llAdd(result);
        return (result)->value;
    } catch (...) {
        return "";
    }
}

void LRUCache::del(const std::string& key) {
    try {
        Node* found = this->hashtable->get(key);
        this->hashtable->del(key);
        this->llRemove(found);
    } catch (...) {
        // Nothing to remove.
    }
}

void LRUCache::llRemove(Node* node) {
    Node* prev = node->prev;
    Node* next = node->next;

    prev->next = next;
    next->prev = prev;

    --this->size;
}

void LRUCache::llAdd(Node* node) {
    Node* prev = this->tail->prev;
    prev->next = node;
    this->tail->prev = node;
    node->prev = prev;
    node->next = this->tail;
    ++this->size;
}