#include <iostream>
#include <cstring>

//НАСЛЕДОВАНИЕ И ПРАВИЛО ПЯТИ

#pragma warning (disable: 4996)  

class binary_priority_queue final {
    struct node {
        int priority;
        char* value;

        node() : priority(0), value(nullptr) {};
        ~node() {
            delete[] value;
        }; 
    };

    node* heap;
    int current_size;
    int max_size;

private:
    void heapify_up(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[parent].priority < heap[index].priority)
                break;
            swap(parent, index);
            index = parent;
        }
    }

    void heapify_down(int index) {
        while (true) {
            int left = 2 * index + 1;
            int right = 2 * index + 2;
            int largest = index;

            if (left < current_size && heap[left].priority > heap[largest].priority)
                largest = left;
            if (right < current_size && heap[right].priority > heap[largest].priority)
                largest = right;

            if (largest == index)
                break;

            swap(index, largest);
            index = largest;
        }
    }

    void swap(int first, int second) {
        node temp = heap[first];
        heap[first] = heap[second];
        heap[second] = temp;
    }

    void swap(binary_priority_queue& other) {
        node* temp_heap = heap;
        heap = other.heap;
        other.heap = temp_heap;

        int temp_size = current_size;
        current_size = other.current_size;
        other.current_size = temp_size;

        int temp_max = max_size;
        max_size = other.max_size;
        other.max_size = temp_max;
    }

    void copy_nodes(const node* source, int count) {
        for (int i = 0; i < count; ++i) {
            if (source[i].value) {
                heap[i].priority = source[i].priority;
                heap[i].value = new char[strlen(source[i].value) + 1];
                strcpy(heap[i].value, source[i].value);
            }
        }
    }

public:
    binary_priority_queue(int initial_size)
        : max_size(initial_size > 0 ? initial_size : throw "Invalid size"),
        current_size(0),
        heap(new node[max_size]()) {}

    binary_priority_queue(const binary_priority_queue& other)
        : max_size(other.max_size),
        current_size(other.current_size),
        heap(new node[max_size]) {
        copy_nodes(other.heap, current_size);
    }

    binary_priority_queue& operator=(const binary_priority_queue& other) {
        if (this != &other) {
            binary_priority_queue temp(other);
            swap(temp);
        }
        return *this;
    }

    binary_priority_queue(binary_priority_queue&& other) noexcept
        : heap(other.heap),
        max_size(other.max_size),
        current_size(other.current_size) {
        other.heap = nullptr;
        other.max_size = 0;
        other.current_size = 0;
    }

    binary_priority_queue& operator=(binary_priority_queue&& other) noexcept {
        if (this != &other) {
            delete[] heap;
            heap = other.heap;
            max_size = other.max_size;
            current_size = other.current_size;
            other.heap = nullptr;
            other.max_size = 0;
            other.current_size = 0;
        }
        return *this;
    }

    ~binary_priority_queue() {
        delete[] heap;
    }

    bool is_empty() const { return current_size == 0; }
    bool is_full() const { return current_size == max_size; }

public:
    void add_value(const char* str, int priority) {
        if (!str) throw "Null pointer";
        if (strlen(str) == 0) throw "Empty string";

        size_t len = strlen(str);
        heap[current_size].value = new char[len + 1];
        strncpy(heap[current_size].value, str, len);
        heap[current_size].value[len] = '\0';
    }

    const char* search_max() const {
        if (is_empty()) throw "Queue is empty";
        return heap[0].value;
    }

    void delete_max() {
        if (is_empty()) throw "Queue is empty";

        delete[] heap[0].value;

        if (current_size == 1) {
            current_size = 0;
        }
        else {
            heap[0] = heap[current_size - 1]; 
            current_size--;
            heapify_down(0);
        }
    }

    void merge(const binary_priority_queue& other) {
        for (int i = 0; i < other.current_size; ++i) {
            if (other.heap[i].value) {  // не nullptr
                add_value(other.heap[i].value, other.heap[i].priority);
            }
        }
    }

    binary_priority_queue meld(const binary_priority_queue& other) const {
        binary_priority_queue result(current_size + other.current_size);

        for (int i = 0; i < current_size; ++i) {
            result.add_value(heap[i].value, heap[i].priority);
        }

        for (int i = 0; i < other.current_size; ++i) {
            result.add_value(other.heap[i].value, other.heap[i].priority);
        }

        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const binary_priority_queue& queue) {
        os << "Queue (size: " << queue.current_size << "/" << queue.max_size << "):\n";
        for (int i = 0; i < queue.current_size; ++i) {
            os << "  [" << i << "] Priority: " << queue.heap[i].priority
                << ", Value: " << (queue.heap[i].value ? queue.heap[i].value : "null") << "\n";
        }
        return os;
    }
};

int main() {
    try {
        binary_priority_queue q1(5), q2(5);

        q1.add_value("a", 10);
        q1.add_value("b", 20);

        q2.add_value("c", 5);
        q2.add_value("d", 30);
        q2.add_value("e", 15);

        std::cout << "Queue 1:\n" << q1 << "\n";
        std::cout << "Queue 2:\n" << q2 << "\n";

        std::cout << "Max in q1: " << q1.search_max() << "\n";
        q1.delete_max();
        std::cout << "After delete max from q1:\n" << q1 << "\n";

        q1.merge(q2);
        std::cout << "After merge q2 to q1:\n" << q1 << "\n";

        auto q3 = q1.meld(q2);
        std::cout << "After meld q1 and q2 (q3):\n" << q3 << "\n";

    }
    catch (const char* msg) {
        std::cerr << "Error: " << msg << "\n";
    }

    return 0;
}
