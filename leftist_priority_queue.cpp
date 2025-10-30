#include "C:\Users\arnau\source\repos\second_sem_labs\priority_queue.h"
#include <iostream>
#include <cstring>

class leftist_priority_queue final : public priority_queue {
private:
    struct Node {
        char* value;
        int priority;
        int rank;
        Node* left;
        Node* right;

        Node(const char* str, int p) : priority(p), rank(1), left(nullptr), right(nullptr) {
            if (str && std::strlen(str) > 0) {
                value = new char[std::strlen(str) + 1];
                strcpy(value, str);
            }
            else {
                value = nullptr;
            }
        }

        Node(const Node& other) : priority(other.priority), rank(other.rank),
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
                rank = other.rank;
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

private:
    [[nodiscard]] int get_rank(Node* node) const {
        return node ? node->rank : 0;
    }

    Node* merge_nodes(Node* node1, Node* node2) {
        if (!node1) return node2;
        if (!node2) return node1;

        if (node1->priority < node2->priority) {
            Node* temp = node1;
            node1 = node2;
            node2 = temp;
        }

        node1->right = merge_nodes(node1->right, node2);
        
        int left_rank = node1->left ? node1->left->rank : 0;
        int right_rank = node1->right ? node1->right->rank : 0;

        if (left_rank < right_rank) {
            Node* temp = node1->left;
            node1->left = node1->right;
            node1->right = temp;
        }

        node1->rank = (node1->right ? node1->right->rank : 0) + 1;
        return node1;
    }

    [[nodiscard]] Node* copy_tree(const Node* node) const {
        if (!node) return nullptr;
        Node* new_node = new Node(node->value, node->priority);
        new_node->rank = node->rank;
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

    void print_tree(std::ostream& os, Node* node, int depth = 0) const {
        if (!node) return;
        print_tree(os, node->right, depth + 1);

        for (int i = 0; i < depth; ++i) {
            os << "    ";
        }

        os << "[" << node->priority << ": " << (node->value ? node->value : "null")
            << ", rank=" << node->rank << "]\n";

        print_tree(os, node->left, depth + 1);
    }

public:
    leftist_priority_queue() : root(nullptr) {}

    leftist_priority_queue(const leftist_priority_queue& other) : root(nullptr) {
        if (other.root) {
            root = copy_tree(other.root);
        }
    }

    leftist_priority_queue& operator=(const leftist_priority_queue& other) {
        if (this != &other) {
            delete_tree(root);
            root = other.root ? copy_tree(other.root) : nullptr;
        }
        return *this;
    }

    leftist_priority_queue(leftist_priority_queue&& other) noexcept : root(other.root) {
        other.root = nullptr;
    }

    leftist_priority_queue& operator=(leftist_priority_queue&& other) noexcept {
        if (this != &other) {
            delete_tree(root);
            root = other.root;
            other.root = nullptr;
        }
        return *this;
    }

    ~leftist_priority_queue() {
        delete_tree(root);
    }

    void add_value(const char* str, int priority) override {
        if (!str) throw "Null pointer";
        if (std::strlen(str) == 0) throw "Empty string";

        Node* new_node = new Node(str, priority);
        root = merge_nodes(root, new_node);
    }

    [[nodiscard]] const char* search_value() const override {
        if (!root) throw "Queue is empty";
        return root->value;
    }

    void delete_value() override {
        if (!root) throw "Queue is empty";

        Node* old_root = root;
        root = merge_nodes(root->left, root->right);
        old_root->left = nullptr;
        old_root->right = nullptr;
        delete old_root;
    }

    priority_queue& merge(const priority_queue& second) override {
        const leftist_priority_queue* other_queue =
            dynamic_cast<const leftist_priority_queue*>(&second);

        if (!other_queue) {
            throw "Incompatible queue types for merge";
        }

        Node* other_copy = copy_tree(other_queue->root);
        root = merge_nodes(root, other_copy);

        return *this;
    }
    
    [[nodiscard]] bool is_empty() const {
        return root == nullptr;
    }

    [[nodiscard]] int get_size() const {
        return count_nodes(root);
    }

    [[nodiscard]] leftist_priority_queue meld(const leftist_priority_queue& other) {
        leftist_priority_queue result;

        if (root && other.root) {
            Node* copy1 = copy_tree(root);
            Node* copy2 = copy_tree(other.root);
            result.root = merge_nodes(copy1, copy2);
        } else if (root) {
            result.root = copy_tree(root);
        } else if (other.root) {
            result.root = copy_tree(other.root);
        }

        return result;
    }


    friend void print_queue(const leftist_priority_queue& queue, const char* name) {
        std::cout << name << " (size: " << queue.get_size() << "):\n";

        if (queue.root) {
            queue.print_tree(std::cout, queue.root, 0);
        }
        else {
            std::cout << "  [EMPTY]\n";
        }
        std::cout << "\n";
    }
};

int main() {
    try {
        std::cout << "Basic operations\n";
        std::cout << "------------------------\n";

        leftist_priority_queue queue1;
        queue1.add_value("Task A", 10);
        queue1.add_value("Task B", 30);
        queue1.add_value("Task C", 20);
        queue1.add_value("Task D", 40);

        print_queue(queue1, "Queue 1 after adding 4 tasks");
        
        const char* max_value = queue1.search_value();
        std::cout << "Max priority task: " << max_value << "\n\n";

        queue1.delete_value();
        print_queue(queue1, "Queue 1 after deleting max");

        std::cout << "New max priority task: " << queue1.search_value() << "\n\n";

        std::cout << "Queue is empty: " << (queue1.is_empty() ? "true" : "false") << "\n";
        std::cout << "Queue size: " << queue1.get_size() << "\n\n";

        std::cout << "Copy operations\n";
        std::cout << "-----------------------\n";

        leftist_priority_queue queue2 = queue1;
        print_queue(queue2, "Queue 2 (copy of queue1)");

        leftist_priority_queue queue3;
        queue3 = queue1;
        print_queue(queue3, "Queue 3 (assigned from queue1)");

        std::cout << "Merge operation\n";
        std::cout << "-----------------------\n";

        leftist_priority_queue queue4;
        queue4.add_value("Task X", 15);
        queue4.add_value("Task Y", 35);

        print_queue(queue4, "Queue 4 before merge");
        queue1.merge(queue4);
        print_queue(queue1, "Queue 1 after merge with queue4");

        std::cout << "Meld operation ([[nodiscard]])\n";
        std::cout << "--------------------------------------\n";

        leftist_priority_queue queue5;
        queue5.add_value("Task P", 25);

        leftist_priority_queue queue6;
        queue6.add_value("Task Q", 45);
        
        leftist_priority_queue queue7 = queue5.meld(queue6);
        print_queue(queue7, "Queue 7 (meld of queue5 and queue6)");

        std::cout << "Error handling\n";
        std::cout << "----------------------\n";

        leftist_priority_queue empty_queue;
        std::cout << "Empty queue is_empty: " << (empty_queue.is_empty() ? "true" : "false") << "\n";

        try {
            const char* unused = empty_queue.search_value();
            (void)unused; // подавляем предупреждение о неиспользовании
        }
        catch (const char* msg) {
            std::cout << "Expected error: " << msg << "\n";
        }

        try {
            empty_queue.delete_value();
        }
        catch (const char* msg) {
            std::cout << "Expected error: " << msg << "\n";
        }

        std::cout << "\nTesting through interface\n";
        std::cout << "----------------------------------\n";

        priority_queue* interface_ptr = &queue1;
        const char* interface_max = interface_ptr->search_value(); // [[nodiscard]] из интерфейса
        std::cout << "Max via interface: " << interface_max << "\n";

    }
    catch (const char* msg) {
        std::cerr << "Error: " << msg << "\n";
        return 1;
    }

    return 0;
}
