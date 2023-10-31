#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

#include "hashtable.h"

// pre: start > 10
// post: returns smallest prime that is at least start
int find_first_bigger_prime(int start) {
    // no need to test even numbers
	if (start % 2 == 0) {
        start++;
    }

	for (int i = start; i <= (2 * start); i += 2) {
        // By Bertrand's postulate, a prime has to exist in (start, 2*start)
		bool isPrime = true;
		for (int j = 3; j * j <= i; j += 2) {
			if (i % j == 0) {
				isPrime = false;
				break;
			}
		}
		if (isPrime == true) {
			return i;
		}
	}

    // Won't happen.
    return -1;
}

int hash0(long key, int tableSize) {
	return (int) (key%tableSize);
}

int hash1(long key, int tableSize) {
	double phi = (sqrt(5)-1)/2;
	double val = key * phi;
	return (int) floor(tableSize*(val - floor(val)));
}

long extractNumbers(std::string key) {
    long result = 0;
    for (const char& c : key) {
        int asciiValue = c;

        result += asciiValue;
        result = result % INT_MAX;
    }

	return result;
}

void freeCuckooTable(Slot **table, int size) {
	for (int i = 0; i < size; ++i) {
		delete table[i];
	}

	delete[] table;
}

HashTableCuckoo::HashTableCuckoo() {
	capacity = 11;
	elements = 0;

	table1 = new Slot*[capacity];
	table2 = new Slot*[capacity];

	for (int i = 0; i < capacity; ++i) {
		table1[i] = new Slot{"", "", false};
		table2[i] = new Slot{"", "", false};
	}
}

HashTableCuckoo::~HashTableCuckoo() {
	freeCuckooTable(table1, capacity);
	freeCuckooTable(table2, capacity);
}

void HashTableCuckoo::set(std::string key, std::string value) {
	int numberOfRehashes = 0;
	bool currentIsTable1 = true;
	bool isDone = false;

	while (true) {
		int attempts = 2 * capacity;
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
				std::string tempValue = value;
				std::string tempKey = key;
				value = currentTable[hash]->value;
				key = currentTable[hash]->key;

				currentTable[hash]->value = tempValue;
				currentTable[hash]->key = key;
				currentIsTable1 = !currentIsTable1;
			}
		}

		if (isDone || numberOfRehashes > 2) {
			break;
		}

		// need to rehash
		rehash();
		++numberOfRehashes;
	}
	++elements;
}

std::string HashTableCuckoo::get(const std::string& key) {
	long convertedKey = extractNumbers(key);
	// check in table1
	int h1 = hash0(convertedKey, capacity);
	if (table1[h1]->key == key) {
		return table1[h1]->value;
	}

	// check in table2
	int h2 = hash1(convertedKey, capacity);
	if (table2[h2]->key == key) {
		std::cout << table2[h2]->value << std::endl;
		return table2[h2]->value;
	}

    return "";
}

void HashTableCuckoo::rehash() {
	int new_capacity = find_first_bigger_prime(2 * capacity + 1);

	std::vector<std::pair<std::string, std::string>> elements;

	// get from first table
	for (int i = 0; i < capacity; ++i) {
		if (table1[i]->occupied) {
			std::pair<std::string, std::string> element{table1[i]->key, table1[i]->value};
			elements.push_back(element);
		}
	}

	// get from second table
	for (int i = 0; i < capacity; ++i) {
		if (table2[i]->occupied) {
			std::pair<std::string, std::string> element{table2[i]->key, table2[i]->value};
			elements.push_back(element);
		}
	}

	// free the old tables and create new tables with larger size
	freeCuckooTable(table1, capacity);
	freeCuckooTable(table2, capacity);

	capacity = new_capacity;

	table1 = new Slot*[capacity];
	table2 = new Slot*[capacity];

	for (int i = 0; i < capacity; ++i) {
		table1[i] = new Slot{"", "", false};
		table2[i] = new Slot{"", "", false};
	}

	// insert back the items one by one
	for (auto &element : elements) {
		set(element.first, element.second);
	}
}

void HashTableCuckoo::print() {
	std::cout << capacity;

	for (int i = 0; i < capacity; ++i) {
		std::cout << " " << ((table1[i]->occupied) ? 1 : 0);
	}

	for (int i = 0; i < capacity; ++i) {
		std::cout << " " << ((table2[i]->occupied) ? 1 : 0);
	}

	std::cout << std::endl;
}