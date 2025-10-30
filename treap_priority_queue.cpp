#include <iostream>
#include <cstring>
#include "C:\Users\arnau\source\repos\second_sem_labs\priority_queue.h"

class treap_priority_queue final : public priority_queue {
private:
    struct Node {
        char* value;
        int priority;
        int key;
        Node* left;
        Node* right;

        Node(const char* str, int p, int k) : priority(p), key(k), left(nullptr), right(nullptr) {
            if (str && std::strlen(str) > 0) {
                value = new char[std::strlen(str) + 1];
                std::strcpy(value, str);
            }
            else {
                value = nullptr;
            }
        }

        Node(const Node& other) : priority(other.priority), key(other.key),
            left(nullptr), right(nullptr) {
            if (other.value) {
                value = new char[std::strlen(other.value) + 1];
                std::strcpy(value, other.value);
            }
            else {
                value = nullptr;
            }
        }

        Node& operator=(const Node& other) {
            if (this != &other) {
                delete[] value;
                priority = other.priority;
                key = other.key;
                if (other.value) {
                    value = new char[std::strlen(other.value) + 1];
                    std::strcpy(value, other.value);
                }
                else {
                    value = nullptr;
                }
            }
            return *this;
        }

        ~Node() {
            delete[] value;
        }
    };

    Node* root;
    int key_counter;

private:
    void split(Node* current, int key, Node*& left, Node*& right) {
        if (!current) {
            left = right = nullptr;
            return;
        }

        if (current->key <= key) {
            split(current->right, key, current->right, right);
            left = current;
        }
        else {
            split(current->left, key, left, current->left);
            right = current;
        }
    }

    Node* merge_nodes(Node* left, Node* right) {
        if (!left) return right;
        if (!right) return left;

        if (left->priority > right->priority) {
            left->right = merge_nodes(left->right, right);
            return left;
        }
        else {
            right->left = merge_nodes(left, right->left);
            return right;
        }
    }

    Node* insert_node(Node* current, Node* new_node) {
        if (!current) return new_node;

        if (new_node->priority > current->priority) {
            split(current, new_node->key, new_node->left, new_node->right);
            return new_node;
        }

        if (new_node->key < current->key) {
            current->left = insert_node(current->left, new_node);
        }
        else {
            current->right = insert_node(current->right, new_node);
        }

        return current;
    }

    Node* delete_root(Node* current) {
        if (!current) return nullptr;
        Node* result = merge_nodes(current->left, current->right);
        current->left = current->right = nullptr;
        delete current;
        return result;
    }

    [[nodiscard]] Node* find_max_priority(Node* current) const {
        if (!current) return nullptr;

        Node* max_node = current;
        Node* left_max = find_max_priority(current->left);
        Node* right_max = find_max_priority(current->right);

        if (left_max && left_max->priority > max_node->priority) {
            max_node = left_max;
        }
        if (right_max && right_max->priority > max_node->priority) {
            max_node = right_max;
        }

        return max_node;
    }

    Node* copy_tree(Node* node) {
        if (!node) return nullptr;
        Node* new_node = new Node(node->value, node->priority, node->key);
        new_node->left = copy_tree(node->left);
        new_node->right = copy_tree(node->right);
        return new_node;
    }

    void delete_tree(Node* node) {
        if (node) {
            delete_tree(node->left);
            delete_tree(node->right);
            delete node;
        }
    }

    [[nodiscard]] int count_nodes(Node* node) const {
        if (!node) return 0;
        return 1 + count_nodes(node->left) + count_nodes(node->right);
    }

    [[nodiscard]] int calculate_height(Node* node) const {
        if (!node) return 0;
        int left_height = calculate_height(node->left);
        int right_height = calculate_height(node->right);
        return 1 + (left_height > right_height ? left_height : right_height);
    }

    void print_tree(std::ostream& os, Node* node, int depth = 0) const {
        if (!node) return;
        print_tree(os, node->right, depth + 1);

        for (int i = 0; i < depth; ++i) {
            os << "    ";
        }

        os << "[Key: " << node->key << ", Prio: " << node->priority
            << ", Val: " << (node->value ? node->value : "null") << "]\n";

        print_tree(os, node->left, depth + 1);
    }

public:
    treap_priority_queue() : root(nullptr), key_counter(0) {}

    treap_priority_queue(const treap_priority_queue& other) : root(nullptr), key_counter(other.key_counter) {
        if (other.root) {
            root = copy_tree(other.root);
        }
    }

    treap_priority_queue& operator=(const treap_priority_queue& other) {
        if (this != &other) {
            delete_tree(root);
            root = other.root ? copy_tree(other.root) : nullptr;
            key_counter = other.key_counter;
        }
        return *this;
    }

    treap_priority_queue(treap_priority_queue&& other) noexcept
        : root(other.root), key_counter(other.key_counter) {
        other.root = nullptr;
        other.key_counter = 0;
    }

    treap_priority_queue& operator=(treap_priority_queue&& other) noexcept {
        if (this != &other) {
            delete_tree(root);
            root = other.root;
            key_counter = other.key_counter;
            other.root = nullptr;
            other.key_counter = 0;
        }
        return *this;
    }

    ~treap_priority_queue() {
        delete_tree(root);
    }

    void add_value(const char* str, int priority) override {
        if (!str) throw "Null pointer";
        if (std::strlen(str) == 0) throw "Empty string";

        int new_key = key_counter++;
        Node* new_node = new Node(str, priority, new_key);
        root = insert_node(root, new_node);
    }

    [[nodiscard]] const char* search_value() const override {
        Node* max_node = find_max_priority(root);
        if (!max_node) throw "Queue is empty";
        return max_node->value;
    }

    void delete_value() override {
        Node* max_node = find_max_priority(root);
        if (!max_node) throw "Queue is empty";

        if (root == max_node) {
            root = delete_root(root);
        }
        else {
            root = delete_max_priority_node(root);
        }
    }

private:
    Node* delete_max_priority_node(Node* current) {
        if (!current) return nullptr;

        Node* max_node = find_max_priority(current);
        if (!max_node) return current;

        if (current == max_node) {
            return delete_root(current);
        }

        if (max_node->key < current->key) {
            current->left = delete_max_priority_node(current->left);
        }
        else {
            current->right = delete_max_priority_node(current->right);
        }

        return current;
    }

public:
    priority_queue& merge(const priority_queue& second) override {
        const treap_priority_queue* other_queue =
            dynamic_cast<const treap_priority_queue*>(&second);

        if (!other_queue) {
            throw "Incompatible queue types for merge";
        }

        treap_priority_queue temp;
        copy_and_merge(*other_queue, temp);
        root = merge_nodes(root, temp.root);
        temp.root = nullptr;

        return *this;
    }

private:
    void copy_and_merge(const treap_priority_queue& other, treap_priority_queue& result) {
        copy_with_new_keys(other.root, result);
    }

    void copy_with_new_keys(Node* node, treap_priority_queue& result) const{
        if (!node) return;
        result.add_value(node->value, node->priority);
        copy_with_new_keys(node->left, result);
        copy_with_new_keys(node->right, result);
    }

public:
    [[nodiscard]] bool is_empty() const {
        return root == nullptr;
    }

    [[nodiscard]] int get_size() const {
        return count_nodes(root);
    }

    [[nodiscard]] int get_height() const {
        return calculate_height(root);
    }

    [[nodiscard]] treap_priority_queue meld(const treap_priority_queue& other) const {
        treap_priority_queue result;

        if (root) {
            copy_with_new_keys(root, result);
        }

        if (other.root) {
            copy_with_new_keys(other.root, result);
        }

        return result;
    }

    friend void print_treap_queue(const treap_priority_queue& queue, const char* name) {
        std::cout << name << " (size: " << queue.get_size()
            << ", height: " << queue.get_height() << "):\n";

        if (queue.root) {
            queue.print_tree(std::cout, queue.root, 0);
        }
        else {
            std::cout << "  [EMPTY]\n";
        }
        std::cout << "\n";
    }
};

// Демонстрация работы декартова дерева
int main() {
    try {
        std::cout << "Basic operations\n";
        std::cout << "------------------------\n";

        treap_priority_queue queue1;
        queue1.add_value("Task A", 10);
        queue1.add_value("Task B", 30);
        queue1.add_value("Task C", 20);
        queue1.add_value("Task D", 40);

        print_treap_queue(queue1, "Queue 1 after adding 4 tasks");

        const char* max_value = queue1.search_value();
        std::cout << "Max priority task: " << max_value << "\n\n";

        std::cout << "Queue is empty: " << (queue1.is_empty() ? "true" : "false") << "\n";
        std::cout << "Queue size: " << queue1.get_size() << "\n";
        std::cout << "Queue height: " << queue1.get_height() << "\n\n";

        queue1.delete_value();
        print_treap_queue(queue1, "Queue 1 after deleting max");

        std::cout << "New max priority task: " << queue1.search_value() << "\n\n";

        std::cout << "Copy operations\n";
        std::cout << "-----------------------\n";

        treap_priority_queue queue2 = queue1;
        print_treap_queue(queue2, "Queue 2 (copy of queue1)");

        treap_priority_queue queue3;
        queue3 = queue1;
        print_treap_queue(queue3, "Queue 3 (assigned from queue1)");

        std::cout << "Complex operations\n";
        std::cout << "--------------------------\n";

        treap_priority_queue queue4;

        queue4.add_value("Urgent Task", 100);
        queue4.add_value("Medium Task", 50);
        queue4.add_value("Low Task", 10);
        queue4.add_value("High Task", 80);
        queue4.add_value("Critical Task", 120);

        print_treap_queue(queue4, "Queue 4 with mixed priorities");

        std::cout << "Max priority in queue4: " << queue4.search_value() << "\n";
        std::cout << "\nExtracting tasks in priority order:\n";
        int extraction_count = 0;
        while (!queue4.is_empty() && extraction_count < 3) {
            const char* task = queue4.search_value();
            std::cout << "  - " << task << "\n";
            queue4.delete_value();
            extraction_count++;
        }

        print_treap_queue(queue4, "Queue 4 after partial extraction");

        std::cout << "Meld operation ([[nodiscard]])\n";
        std::cout << "--------------------------------------\n";

        treap_priority_queue queue5;
        queue5.add_value("Project A", 60);
        queue5.add_value("Project B", 25);

        treap_priority_queue queue6;
        queue6.add_value("Project C", 90);
        queue6.add_value("Project D", 15);

        treap_priority_queue queue7 = queue5.meld(queue6);
        print_treap_queue(queue7, "Queue 7 (meld of queue5 and queue6)");

        std::cout << "Error handling\n";
        std::cout << "----------------------\n";

        treap_priority_queue empty_queue;
        std::cout << "Empty queue is_empty: " << (empty_queue.is_empty() ? "true" : "false") << "\n";

        try {
            const char* unused = empty_queue.search_value();
            (void)unused; // подавляем предупреждение
        }
        catch (const char* msg) {
            std::cout << "Expected error when searching empty queue: " << msg << "\n";
        }

        try {
            empty_queue.delete_value();
        }
        catch (const char* msg) {
            std::cout << "Expected error when deleting from empty queue: " << msg << "\n";
        }

        std::cout << "\nTesting through interface\n";
        std::cout << "----------------------------------\n";

        priority_queue* interface_ptr = &queue1;
        const char* interface_max = interface_ptr->search_value();
        std::cout << "Max via interface: " << interface_max << "\n";

        std::cout << "\nLarge operations test\n";
        std::cout << "-----------------------------\n";

        treap_priority_queue large_queue;
        for (int i = 0; i < 5; ++i) {
            char task_name[20];
            std::sprintf(task_name, "Task%d", i);
            large_queue.add_value(task_name, i * 10 + 5);
        }

        print_treap_queue(large_queue, "Large queue with 5 tasks");
        std::cout << "Large queue height: " << large_queue.get_height() << "\n";

    }
    catch (const char* msg) {
        std::cerr << "Error: " << msg << "\n";
        return 1;
    }

    return 0;
}
