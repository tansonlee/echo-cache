#ifndef HASHTABLE_H
#define HASHTABLE_H

struct Slot {
	std::string key;
	std::string value;
	bool occupied;
};

class HashTableCuckoo {
public:
	HashTableCuckoo();
	~HashTableCuckoo();

	void set(std::string key, std::string value);
	std::string get(const std::string& key);
	void print();

private:
	void rehash();

	int capacity;
	int elements;

	Slot **table1;
	Slot **table2;
};

#endif