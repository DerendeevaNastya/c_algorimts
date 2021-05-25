#include <iostream>

class Entry;
using EntryPtr = Entry*;

class Entry
{
public:
	EntryPtr next;
	int key;
	int value;

	Entry() = default;

	Entry(int key, int value)
	{
		this->key = key;
		this->value = value;
		next = nullptr;
	}

	~Entry() = default;
};

class HashMap
{
private:
	int count;
	long max_size;
	int current_size;
	EntryPtr* elements;
public:
	HashMap(int ms) {
		this->count = 0;
		max_size = ms;
		current_size = 16;
		elements = new EntryPtr[current_size];
		for (int i = 0; i < current_size; i++) {
			elements[i] = nullptr;
		}
	}

	HashMap() : HashMap((long)INT32_MAX) { }

	void Add(int key, int value) {
		EntryPtr ptr = getEntryBy(key);
		if (ptr != nullptr) {
			ptr->value = value;
			return;
		}

		long hash = getHash(key);
		int index = getIndex(hash);
		auto newEntry = new Entry(key, value);

		innerAdd(elements, index, newEntry);
		count++;

		resize(true, current_size * 2);
	}

	bool Contains(int key) {
		Entry* ptr = getEntryBy(key);
		return ptr == nullptr;
	}

	int Get(int key) {
		EntryPtr ptr = getEntryBy(key);
		if (ptr == nullptr) {
			throw std::invalid_argument("hashmap doesn't contains value by key");
		}
		return (*ptr).value;
	}

	void Delete(int key) {
		long hash = getHash(key);
		int index = getIndex(hash);

		if (elements[index] == nullptr) {
			return;
		}

		EntryPtr currentElement = elements[index];
		if (currentElement->key == key) {
			elements[index] = currentElement->next;
			delete currentElement;
			count--;

			resize(false, current_size / 2);
			return;
		}

		while (currentElement->next != nullptr) {
			if (currentElement->next->key == key) {
				currentElement->next = currentElement->next->next;
				delete currentElement->next;
				count--;

				resize(false, current_size / 2);
				return;
			}
			currentElement = currentElement->next;
		}
	}

	int Count() {
		return count;
	}

private:

	long getHash(int element)
	{
		long hash = 0;
		int q = (1<<30) - 1;
		int p = 31;
		long currentP = 1;
		int currentElement = element;
		while (true) {
			int remainder = currentElement % 10;
			hash = (hash % q + (currentP * (remainder + 9)) % q) % q;

			currentP = (currentP * p) % q;
			currentElement = currentElement / 10;

			if (currentElement == 0) {
				break;
			}
		}
		return hash;
	}

	int getIndex(long hash)
	{
		return hash % current_size;
	}

	EntryPtr getEntryBy(int key) {
		long hash = getHash(key);
		int index = getIndex(hash);
		if (elements[index] == nullptr) {
			return nullptr;
		}
		EntryPtr currentElement = elements[index];

		while (currentElement != nullptr) {
			if (currentElement->key == key) {
				return currentElement;
			}
			currentElement = currentElement->next;
		}
		return nullptr;
	}

	void innerAdd(EntryPtr* arr, int index, EntryPtr newEntry) {
		if (arr[index] == nullptr)
		{
			arr[index] = newEntry;
		}
		else
		{
			EntryPtr currentElement = arr[index];
			while (currentElement->next != nullptr) {
				currentElement = currentElement->next;
			}
			currentElement->next = newEntry;
		}
	}

	void resize(bool isIncreace, int newSize) {
		double load_factor = (double)count / (double)current_size;
		if (isIncreace && (load_factor < 0.75 || current_size >= max_size) ||
			!isIncreace && (load_factor > 0.15 || current_size <= 16))
		{
			return;
		}

		int oldSize = current_size;
		current_size = newSize;
		EntryPtr* newArray = new EntryPtr[current_size];
		for (int i = 0; i < current_size; i++) {
			newArray[i] = nullptr;
		}

		for (int i = 0; i < oldSize; i++) {
			EntryPtr currentElement = elements[i];

			while (currentElement != nullptr) {
				long hash = getHash(currentElement->key);
				int newIndex = getIndex(hash);
				innerAdd(newArray, newIndex, currentElement);

				currentElement = currentElement->next;
			}
			currentElement = nullptr;
		}

		EntryPtr* oldElements = elements;
		elements = newArray;

		delete[] oldElements;
	}
};
