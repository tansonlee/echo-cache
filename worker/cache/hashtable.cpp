#include "hashtable.h"
#include<cmath>

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

// Use Polynomial Rolling Hash
int extractNumbers(std::string key) {
	int base = 13;

    int result = 0;
	int keyLength = key.length();
	for (int i = 0; i < keyLength; ++i) {
        int asciiValue = key.at(i);

		result = (result * base + asciiValue) % INT_MAX;

	}

	return result;
}