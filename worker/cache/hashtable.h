#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <exception>

// NOTE: The implementation needs to be in the header file
// due to the use of templates.

int find_first_bigger_prime(int start);
int hash0(long key, int tableSize);
int hash1(long key, int tableSize);
int extractNumbers(std::string key);

template<typename ValueType = std::string>
class HashTableCuckoo {
public:
	HashTableCuckoo() {
		capacity = 11;
		elements = 0;

		table1 = new Slot*[capacity];
		table2 = new Slot*[capacity];

		for (int i = 0; i < capacity; ++i) {
			table1[i] = new Slot();
			table2[i] = new Slot();
		}
	}

	~HashTableCuckoo() {
		freeTable(table1);
		freeTable(table2);
	}

	int currLevel = 0;

	void set(std::string key, ValueType value) {
		int numberOfRehashes = 0;
		bool currentIsTable1 = true;
		bool isDone = false;

		try {
			this->get(key);
			// If get did not throw then the key exists.
			this->del(key);
		} catch (...) {}

		while (true) {
			int attempts = capacity * 2;
			for (int i = 0; i < attempts; ++i) {
				if (isDone) {
					break;
				}
				int hash;
				if (currentIsTable1) {
					hash = hash0(extractNumbers(key), capacity);
				} else {
					hash = hash1(extractNumbers(key), capacity);
				}

				Slot **currentTable = (currentIsTable1 ? table1 : table2);
				if (!currentTable[hash]->occupied) {
					currentTable[hash]->value = value;
					currentTable[hash]->key = key;
					currentTable[hash]->occupied = true;
					isDone = true;
				} else {
					// swap
					ValueType tempValue = value;
					std::string tempKey = key;
					value = currentTable[hash]->value;
					key = currentTable[hash]->key;

					currentTable[hash]->value = tempValue;
					currentTable[hash]->key = tempKey;
					currentIsTable1 = !currentIsTable1;
				}
			}

			if (isDone || numberOfRehashes > 2) {
				break;
			}

			// need to rehash
			++numberOfRehashes;
			rehash();
		}
		++elements;
	}

	void del(const std::string& key) {
		long convertedKey = extractNumbers(key);
		// check in table1
		int h1 = hash0(convertedKey, capacity);
		if (table1[h1]->key == key) {
			table1[h1]->occupied = false;
			table1[h1]->key = "";
		}

		// check in table2
		int h2 = hash1(convertedKey, capacity);
		if (table2[h2]->key == key) {
			table2[h2]->occupied = false;
			table2[h2]->key = "";
		}
	}

	ValueType get(const std::string& key) {
		long convertedKey = extractNumbers(key);
		// check in table1
		int h1 = hash0(convertedKey, capacity);
		if (table1[h1]->key == key) {
			return table1[h1]->value;
		}

		// check in table2
		int h2 = hash1(convertedKey, capacity);
		if (table2[h2]->key == key) {
			return table2[h2]->value;
		}

		throw std::runtime_error("Key not found");
	}

	void print() {
		std::cout << capacity << std::endl;;

		for (int i = 0; i < capacity; ++i) {
			std::cout << " " << ((table1[i]->occupied) ? 1 : 0);
		}
		std::cout << std::endl;

		for (int i = 0; i < capacity; ++i) {
			std::cout << " " << ((table2[i]->occupied) ? 1 : 0);
		}

		std::cout << std::endl;
	}

private:
	struct Slot {
		std::string key;
		ValueType value;
		bool occupied;
	};

	void rehash() {
		int new_capacity = find_first_bigger_prime(2 * capacity + 1);

		std::vector<std::pair<std::string, ValueType>> elements;

		// get from first table
		for (int i = 0; i < capacity; ++i) {
			if (table1[i]->occupied) {
				std::pair<std::string, ValueType> element{table1[i]->key, table1[i]->value};
				elements.push_back(element);
			}
		}

		// get from second table
		for (int i = 0; i < capacity; ++i) {
			if (table2[i]->occupied) {
				std::pair<std::string, ValueType> element{table2[i]->key, table2[i]->value};
				elements.push_back(element);
			}
		}

		// free the old tables and create new tables with larger size
		this->freeTable(table1);
		this->freeTable(table2);

		capacity = new_capacity;

		table1 = new Slot*[capacity];
		table2 = new Slot*[capacity];

		for (int i = 0; i < capacity; ++i) {
			table1[i] = new Slot();
			table2[i] = new Slot();
		}

		// insert back the items one by one
		currLevel += 1;

		for (auto &element : elements) {
			this->set(element.first, element.second);
		}
		currLevel -= 1;
	}

	void freeTable(Slot **table) {
		for (int i = 0; i < this->capacity; ++i) {
			delete table[i];
		}

		delete[] table;
	}

	int capacity;
	int elements;

	Slot **table1;
	Slot **table2;
};

#endif