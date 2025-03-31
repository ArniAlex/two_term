/*
ЕЩЁ В ПРОЦЕССЕ
*/

#include <iostream>
#include "5z.h"  
#include <stdexcept> 
#include <cstring> 

class binary_priority_queue : public priority_queue {
	/*
	 * структура
	 * массив
	 * методы
	 */
	struct node {
		int key;
		char* value;
	};
	node* heap;
	int current_size;
	int max_size;

	//вспомогательные методы
	void heapify_up() noexept {
		for (int i = current_size - 1; i > 0; --i) {
			int par_i = (i - 1) / 2;
			if (heap[i].key > heap[par_i].key) {
				swap(i, par_i);
			}
			else {
				break; 
			}
		}
	};

	void heapify_down(int ind) noexept {
		int left = 2 * ind + 1, right = 2 * ind + 2;  
		int max = ind;

		if (left < size && heap[left].key > heap[max].key) {
			max = left; 
		}

		if (right < size && heap[right].key > heap[max].key) {
			max = right;
		}

		if (largest != ind) {
			swap(ind, max);  
			heapifyDown(max);  
		}
	};

	void swap(int i1, int i2) {
		node temp;
		temp = heap[i1];
		heap[i1] = heap[i2];
		heap[i2] = temp;
	};
	
public:
	binary_priority_queue(int initial_size) : max_size(initial_size), current_size(0) {
		heap = new node[max_size];

		if (heap == nullptr) { throw std::bad_alloc(); } //обязательно ли вообще

		for (int i = 0; i < max_size; ++i) {
			heap[i].key = 0;
			heap[i].value = nullptr;
		}
	}; 

	binary_priority_queue(const binary_priority_queue& other): heap(nullptr), max_size(other.max_size), current_size(other.current_size) {
		heap = new node[other.max_size];
		if (heap == nullptr) { throw std::bad_alloc(); }

		for (int i = 0; i < current_size; ++i) {
			heap[i].key = other.heap[i].key;

			if (other.heap[i].value != nullptr) {
				int len = strlen(other.heap[i].value);
				heap[i].value = new char[len + 1];
				if (heap[i].value == nullptr) {
					for (int j = 0; j < i; ++j) {
						delete[] heap[j].value;
					}
					delete[] heap;
					heap = nullptr;
					throw std::bad_alloc();
				}
				memcpy(heap[i].value, other.heap[i].value, len + 1);
			}
			else {
				heap[i].value = nullptr;
			}
		}
	};

	binary_priority_queue& operator=(const binary_priority_queue& other) {
		if (this == &other) {
			return *this;
		}

		for (int i = 0; i < current_size; ++i) {
			delete[] heap[i].value;
		}
		delete[] heap;

		max_size = other.max_size;
		current_size = other.current_size;

		heap = new node[max_size]; 
		if (heap == nullptr) {
			heap = nullptr;
			max_size = 0;
			current_size = 0;
			throw std::bad_alloc();
		}

		for (int i = 0; i < current_size; ++i) {
			heap[i].key = other.heap[i].key;
			if (other.heap[i].value != nullptr) {
				int len = strlen(other.heap[i].value); 
				heap[i].value = new char[len + 1];
				if (heap[i].value == nullptr) {
					for (int j = 0; j < i; ++j) {
						delete[] heap[j].value;
					}
					delete[] heap;
					heap = nullptr;
					max_size = 0;
					current_size = 0;
					throw std::bad_alloc();
				}
				memcpy(heap[i].value, other.heap[i].value, len + 1);

			}
			else {
				heap[i].value = nullptr;
			}
		}

		return *this;
	};

	~binary_priority_queue() noexept override {
		for (int i = 0; i < current_size; ++i) { 
			delete[] heap[i].str;
			heap[i].str = nullptr; 
		}
		delete[] heap;
		heap = nullptr;
	}; 


	//основные методы

	void add_line(const char* str, int key) override {
		if (current_size == max_size) {
			throw std::runtime_error("The queue is full.");
		}
		int ind = current_size;

		heap[ind].key = key;
		heap[ind].value = new char[strlen(str) + 1];

		if (heap[ind].value == nullptr) { throw std::bad_alloc(); }

		current_size += 1;

		memcpy(heap[ind].value, str, strlen(str));

		heapify_up();
	};

	char* search_value() override {
		if (current_size == 0) {
			throw std::runtime_error("The queue is empty.");
		}
		return heap[0].value; //возврат указателя?? чекнуть
	};

	void delete_value() override {
		if (current_size == 0) {
			throw std::runtime_error("The queue is empty.");
		}

		delete[] heap[0].value;

		heap[0] = heap[current_size - 1]; 
		current_size--;

		heapify_down(0);
	};

	binary_priority_queue& merge(const binary_priority_queue& second) override {
		if (this == &second || second.current_size == 0) {
			return *this; 
		}
		
		if (current_size == 0) {
			*this = second;
		} 
		
		if (max_size < current_size + second.current_size) {
			node* newHeap = new node[current_size + second.current_size];
			if (newHeap == nullptr) {
				throw std::bad_alloc();
			}

			for (size_t i = 0; i < current_size; ++i) {
				newHeap[i].key = heap[i].key;
				if (heap[i].value != nullptr) {
					int len = strlen(heap[i].value);
					newHeap[i].value = new char[len + 1];
					if (newHeap[i].value == nullptr) {
						throw std::bad_alloc();
					}
					memcpy(newHeap[i].value, heap[i].value, len + 1);
				}
				else {
					newHeap[i].value = nullptr;
				}
			}
		}
		

		//TODO глубокое копирование надо..... 
		memcpy(heap + current_size, second.heap, second.current_size);
		heapify_down(current_size - 1);
		current_size += second.current_size;
	};
	
};

