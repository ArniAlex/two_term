#include <iostream>
#include <cstring>
#include "C:\Users\arnau\source\repos\second_sem_labs\priority_queue.h"
#pragma warning (disable: 4996)

class binary_priority_queue final : public priority_queue {
    struct node {
        int priority;
        char* value;

        node() : priority(0), value(nullptr) {}

        node(int p, const char* str) : priority(p), value(nullptr) {
            if (str && std::strlen(str) > 0) {
                value = new char[std::strlen(str) + 1];
                std::strcpy(value, str);
            }
        }

        node(const node& other) : priority(other.priority), value(nullptr) {
            if (other.value) {
                value = new char[std::strlen(other.value) + 1];
                std::strcpy(value, other.value);
            }
        }

        node& operator=(const node& other) {
            if (this != &other) {
                delete[] value;
                priority = other.priority;
                value = nullptr;
                if (other.value) {
                    value = new char[std::strlen(other.value) + 1];
                    std::strcpy(value, other.value);
                }
            }
            return *this;
        }

        ~node() {
            delete[] value;
        }

        void clear() {
            delete[] value;
            value = nullptr;
            priority = 0;
        }
    };

    node* heap;
    int current_size;
    int max_size;

private:
    void heapify_up(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[parent].priority >= heap[index].priority)
                break;
            swap_nodes(parent, index);
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

            swap_nodes(index, largest);
            index = largest;
        }
    }

    void swap_nodes(int first, int second) {
        node temp = heap[first];
        heap[first] = heap[second];
        heap[second] = temp;
    }

    void swap_queues(binary_priority_queue& other) noexcept {
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

public:
    explicit binary_priority_queue(int initial_size)
            : heap(nullptr),
              current_size(0),
              max_size(initial_size) {

        if (max_size <= 0) {
            throw "Invalid size: must be positive";
        }

        heap = new node[max_size];
    }

    binary_priority_queue(const binary_priority_queue& other)
            : heap(nullptr),
              current_size(other.current_size),
              max_size(other.max_size) {

        heap = new node[max_size];

        for (int i = 0; i < current_size; ++i) {
            heap[i] = other.heap[i];
        }
    }

    binary_priority_queue& operator=(const binary_priority_queue& other) {
        if (this != &other) {
            binary_priority_queue temp(other);
            swap_queues(temp);
        }
        return *this;
    }

    binary_priority_queue(binary_priority_queue&& other) noexcept
            : heap(other.heap),
              current_size(other.current_size),
              max_size(other.max_size) {
        other.heap = nullptr;
        other.current_size = 0;
        other.max_size = 0;
    }

    binary_priority_queue& operator=(binary_priority_queue&& other) noexcept {
        if (this != &other) {
            delete[] heap;

            heap = other.heap;
            current_size = other.current_size;
            max_size = other.max_size;

            other.heap = nullptr;
            other.current_size = 0;
            other.max_size = 0;
        }
        return *this;
    }

    ~binary_priority_queue() {
        delete[] heap;
    }

    void add_value(const char* str, int priority) override {
        if (!str) throw "Null pointer";
        if (std::strlen(str) == 0) throw "Empty string";
        if (is_full()) throw "Queue is full";

        heap[current_size] = node(priority, str);

        if (current_size > 0) {
            heapify_up(current_size);
        }
        current_size++;
    }

    [[nodiscard]] const char* search_value() const override {
        if (is_empty()) throw "Queue is empty";
        return heap[0].value;
    }

    void delete_value() override {
        if (is_empty()) throw "Queue is empty";

        heap[0].clear();

        if (current_size == 1) {
            current_size = 0;
        }
        else {
            heap[0] = heap[current_size - 1];
            heap[current_size - 1].clear();
            current_size--;

            if (current_size > 0) {
                heapify_down(0);
            }
        }
    }

    priority_queue& merge(const priority_queue& second) override {
        auto other_queue = dynamic_cast<const binary_priority_queue*>(&second);

        if (!other_queue) {
            throw "Incompatible queue types for merge";
        }

        if (current_size + other_queue->current_size > max_size) {
            throw "Merge failed: Insufficient capacity in target queue.";
        }

        for (int i = 0; i < other_queue->current_size; ++i) {
            if (other_queue->heap[i].value) {
                add_value(other_queue->heap[i].value, other_queue->heap[i].priority);
            }
        }
        return *this;
    }

    [[nodiscard]] bool is_empty() const {
        return current_size == 0;
    }

    [[nodiscard]] bool is_full() const {
        return current_size >= max_size;
    }

    [[nodiscard]] int get_current_size() const {
        return current_size;
    }

    [[nodiscard]] int get_max_size() const {
        return max_size;
    }

    [[nodiscard]] binary_priority_queue meld(const binary_priority_queue& other) const {
        binary_priority_queue result(current_size + other.current_size);

        for (int i = 0; i < current_size; ++i) {
            if (heap[i].value) {
                result.add_value(heap[i].value, heap[i].priority);
            }
        }

        result.merge(other);
        return result;
    }

    friend std::ostream& operator<<(std::ostream& os, const binary_priority_queue& queue) {
        os << "Binary Priority Queue (size: " << queue.get_current_size()
           << "/" << queue.get_max_size() << "):\n";

        if (queue.heap == nullptr) {
            os << "  Heap is null\n";
            return os;
        }

        for (int i = 0; i < queue.get_current_size(); ++i) {
            const char* value_str = queue.heap[i].value ? queue.heap[i].value : "null";
            os << "  [" << i << "] Priority: " << queue.heap[i].priority
               << ", Value: " << value_str << "\n";
        }
        return os;
    }
};

int main() {
    try {
        std::cout << "Creating q1...\n";
        binary_priority_queue q1(5);

        std::cout << "Queue 1 capacity: " << q1.get_max_size() << "\n";
        std::cout << "Queue 1 is empty: " << (q1.is_empty() ? "true" : "false") << "\n";

        std::cout << "Adding values to q1...\n";
        q1.add_value("first", 10);
        q1.add_value("second", 20);
        q1.add_value("third", 15);

        std::cout << "Queue 1 current size: " << q1.get_current_size() << "\n";
        std::cout << "Queue 1 is full: " << (q1.is_full() ? "true" : "false") << "\n";
        std::cout << "Queue 1:\n" << q1 << "\n";

        const char* max_value = q1.search_value(); 
        std::cout << "Max value: " << max_value << "\n";

        std::cout << "Creating q2...\n";
        binary_priority_queue q2(3);
        std::cout << "Queue 2 capacity: " << q2.get_max_size() << "\n";

        q2.add_value("fourth", 25);
        q2.add_value("fifth", 5);

        std::cout << "Queue 2 current size: " << q2.get_current_size() << "\n";
        std::cout << "Queue 2:\n" << q2 << "\n";

        std::cout << "Testing copy constructor...\n";
        binary_priority_queue q3 = q1;
        std::cout << "Queue 3 (copy of q1) size: " << q3.get_current_size() << "\n";
        std::cout << "Queue 3:\n" << q3 << "\n";

        std::cout << "Testing assignment operator...\n";
        binary_priority_queue q4(2);
        q4 = q2;
        std::cout << "Queue 4 size: " << q4.get_current_size() << "\n";
        std::cout << "Queue 4:\n" << q4 << "\n";

        std::cout << "Testing meld ([[nodiscard]] метод)...\n";
        binary_priority_queue q5 = q1.meld(q2); 
        std::cout << "Queue 5 size after meld: " << q5.get_current_size() << "\n";
        std::cout << "Queue 5:\n" << q5 << "\n";

        std::cout << "Testing merge...\n";
        binary_priority_queue q6(10);
        q6.add_value("sixth", 30);
        q6.merge(q1);
        std::cout << "Queue 6 size after merge: " << q6.get_current_size() << "\n";
        std::cout << "Queue 6:\n" << q6 << "\n";

        std::cout << "Testing delete_value...\n";
        q1.delete_value();
        std::cout << "Queue 1 size after delete: " << q1.get_current_size() << "\n";
        std::cout << "Queue 1:\n" << q1 << "\n";

        std::cout << "Testing through interface...\n";
        priority_queue* interface_ptr = &q2;
        const char* interface_max = interface_ptr->search_value(); 
        std::cout << "Max via interface: " << interface_max << "\n";

    }
    catch (const char* msg) {
        std::cerr << "Error: " << msg << "\n";
        return 1;
    }

    return 0;
}

