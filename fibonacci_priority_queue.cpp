#include <iostream>
#include <cstring>
#include "../../source/repos/second_sem_labs/priority_queue.h"

class fibonacci_priority_queue final : public priority_queue {
private:
    struct FibonacciNode {
        char* value;
        int priority;
        int degree;
        bool marked;
        FibonacciNode* parent;
        FibonacciNode* child;
        FibonacciNode* left;
        FibonacciNode* right;

        FibonacciNode(const char* str, int p) : priority(p), degree(0), marked(false),
                                               parent(nullptr), child(nullptr) {
            if (str && std::strlen(str) > 0) {
                value = new char[std::strlen(str) + 1];
                std::strcpy(value, str);
            } else {
                value = nullptr;
            }
            left = this;
            right = this;
        }

        FibonacciNode(const FibonacciNode& other) : priority(other.priority), degree(other.degree),
                                                   marked(other.marked), parent(nullptr), child(nullptr) {
            if (other.value) {
                value = new char[std::strlen(other.value) + 1];
                std::strcpy(value, other.value);
            } else {
                value = nullptr;
            }
            left = this;
            right = this;
        }

        ~FibonacciNode() {
            delete[] value;
        }
    };

    FibonacciNode* min_node;
    int node_count;

private:
    void insert_into_list(FibonacciNode*& list, FibonacciNode* node) {
        if (!list) {
            list = node;
            node->left = node;
            node->right = node;
        } else {
            node->right = list;
            node->left = list->left;
            list->left->right = node;
            list->left = node;
        }
    }

    void remove_from_list(FibonacciNode*& list, FibonacciNode* node) {
        if (node->right == node) {
            list = nullptr;
        } else {
            node->left->right = node->right;
            node->right->left = node->left;
            if (list == node) {
                list = node->right;
            }
        }
        node->left = node;
        node->right = node;
    }

    void link_trees(FibonacciNode* child, FibonacciNode* parent) {
        remove_from_list(min_node, child);
        child->parent = parent;
        child->marked = false;

        if (!parent->child) {
            parent->child = child;
        } else {
            insert_into_list(parent->child, child);
        }
        parent->degree++;
    }


    void consolidate() {
        const int MAX_DEGREE = 45; // log_phi(2^32) ~ 45
        FibonacciNode* degree_table[MAX_DEGREE] = {nullptr};

        FibonacciNode* nodes_to_process[min_node ? node_count : 1];
        int count = 0;

        if (min_node) {
            FibonacciNode* current = min_node;
            do {
                nodes_to_process[count++] = current;
                current = current->right;
            } while (current != min_node);
        }

        for (int i = 0; i < count; ++i) {
            FibonacciNode* current = nodes_to_process[i];
            int degree = current->degree;

            while (degree_table[degree]) {
                FibonacciNode* other = degree_table[degree];
                if (current->priority < other->priority) {
                    FibonacciNode* temp = current;
                    current = other;
                    other = temp;
                }
                link_trees(other, current);
                degree_table[degree] = nullptr;
                degree++;
            }
            degree_table[degree] = current;
        }

        min_node = nullptr;
        for (int i = 0; i < MAX_DEGREE; ++i) {
            if (degree_table[i]) {
                if (!min_node || degree_table[i]->priority > min_node->priority) {
                    min_node = degree_table[i];
                }
            }
        }
    }

    void cut(FibonacciNode* node, FibonacciNode* parent) {
        remove_from_list(parent->child, node);
        parent->degree--;
        insert_into_list(min_node, node);
        node->parent = nullptr;
        node->marked = false;
    }

    void cascading_cut(FibonacciNode* node) {
        FibonacciNode* parent = node->parent;
        if (parent) {
            if (!node->marked) {
                node->marked = true;
            } else {
                cut(node, parent);
                cascading_cut(parent);
            }
        }
    }

    FibonacciNode* copy_node(FibonacciNode* node, FibonacciNode* parent) {
        if (!node) return nullptr;

        FibonacciNode* new_node = new FibonacciNode(node->value, node->priority);
        new_node->degree = node->degree;
        new_node->marked = node->marked;
        new_node->parent = parent;

        if (node->child) {
            FibonacciNode* child_current = node->child;
            FibonacciNode* first_child = nullptr;
            FibonacciNode* last_child = nullptr;

            do {
                FibonacciNode* new_child = copy_node(child_current, new_node);
                if (!first_child) {
                    first_child = new_child;
                } else {
                    last_child->right = new_child;
                    new_child->left = last_child;
                }
                last_child = new_child;
                child_current = child_current->right;
            } while (child_current != node->child);

            first_child->left = last_child;
            last_child->right = first_child;
            new_node->child = first_child;
        }

        return new_node;
    }

    void delete_node(FibonacciNode* node) {
        if (!node) return;

        FibonacciNode* child = node->child;
        if (child) {
            FibonacciNode* current = child;
            do {
                FibonacciNode* next = current->right;
                delete_node(current);
                current = next;
            } while (current != child);
        }
        delete node;
    }

    [[nodiscard]] int count_nodes(FibonacciNode* list) const {
        if (!list) return 0;
        int count = 0;
        FibonacciNode* current = list;
        do {
            count += 1 + count_nodes(current->child);
            current = current->right;
        } while (current != list);
        return count;
    }

public:
    fibonacci_priority_queue() : min_node(nullptr), node_count(0) {}

    fibonacci_priority_queue(const fibonacci_priority_queue& other) : min_node(nullptr), node_count(other.node_count) {
        if (other.min_node) {
            FibonacciNode* current = other.min_node;
            do {
                FibonacciNode* new_node = copy_node(current, nullptr);
                insert_into_list(min_node, new_node);
                if (!min_node || new_node->priority > min_node->priority) {
                    min_node = new_node;
                }
                current = current->right;
            } while (current != other.min_node);
        }
    }

    fibonacci_priority_queue& operator=(const fibonacci_priority_queue& other) {
        if (this != &other) {
            delete_node(min_node);
            min_node = nullptr;
            node_count = other.node_count;

            if (other.min_node) {
                FibonacciNode* current = other.min_node;
                do {
                    FibonacciNode* new_node = copy_node(current, nullptr);
                    insert_into_list(min_node, new_node);
                    if (!min_node || new_node->priority > min_node->priority) {
                        min_node = new_node;
                    }
                    current = current->right;
                } while (current != other.min_node);
            }
        }
        return *this;
    }

    fibonacci_priority_queue(fibonacci_priority_queue&& other) noexcept
        : min_node(other.min_node), node_count(other.node_count) {
        other.min_node = nullptr;
        other.node_count = 0;
    }

    fibonacci_priority_queue& operator=(fibonacci_priority_queue&& other) noexcept {
        if (this != &other) {
            delete_node(min_node);
            min_node = other.min_node;
            node_count = other.node_count;
            other.min_node = nullptr;
            other.node_count = 0;
        }
        return *this;
    }

    ~fibonacci_priority_queue() {
        delete_node(min_node);
    }

    void add_value(const char* str, int priority) override {
        if (!str) throw "Null pointer";
        if (std::strlen(str) == 0) throw "Empty string";

        FibonacciNode* new_node = new FibonacciNode(str, priority);
        insert_into_list(min_node, new_node);

        if (!min_node || new_node->priority > min_node->priority) {
            min_node = new_node;
        }
        node_count++;
    }

    [[nodiscard]] const char* search_value() const override {
        if (!min_node) throw "Queue is empty";
        return min_node->value;
    }

    void delete_value() override {
        if (!min_node) throw "Queue is empty";

        FibonacciNode* old_min = min_node;

        if (old_min->child) {
            FibonacciNode* child = old_min->child;
            do {
                FibonacciNode* next_child = child->right;
                insert_into_list(min_node, child);
                child->parent = nullptr;
                child = next_child;
            } while (child != old_min->child);
        }

        remove_from_list(min_node, old_min);
        node_count--;

        if (old_min == old_min->right) {
            min_node = nullptr;
        } else {
            min_node = old_min->right;
            consolidate();
        }

        delete old_min;
    }

    priority_queue& merge(const priority_queue& second) override {
        const fibonacci_priority_queue* other_queue =
            dynamic_cast<const fibonacci_priority_queue*>(&second);

        if (!other_queue) {
            throw "Incompatible queue types for merge";
        }

        if (!other_queue->min_node) return *this;

        if (min_node) {
            FibonacciNode* this_left = min_node->left;
            FibonacciNode* other_left = other_queue->min_node->left;

            this_left->right = other_queue->min_node;
            other_queue->min_node->left = this_left;
            other_left->right = min_node;
            min_node->left = other_left;

            if (other_queue->min_node->priority > min_node->priority) {
                min_node = other_queue->min_node;
            }
        } else {
            min_node = other_queue->min_node;
        }

        node_count += other_queue->node_count;
        return *this;
    }

    [[nodiscard]] bool is_empty() const {
        return min_node == nullptr;
    }

    [[nodiscard]] int get_size() const {
        return node_count;
    }

    [[nodiscard]] fibonacci_priority_queue meld(const fibonacci_priority_queue& other) const {
        fibonacci_priority_queue result = *this;
        fibonacci_priority_queue temp = other;
        result.merge(temp);
        return result;
    }

    friend void print_fibonacci_queue(const fibonacci_priority_queue& queue, const char* name) {
        std::cout << name << " (size: " << queue.get_size() << "):\n";

        if (queue.min_node) {
            FibonacciNode* current = queue.min_node;
            int tree_count = 0;
            do {
                std::cout << "Tree " << ++tree_count << ": ";
                fibonacci_priority_queue::print_tree(std::cout, current, 1);
                current = current->right;
            } while (current != queue.min_node);
        } else {
            std::cout << "  [EMPTY]\n";
        }
        std::cout << "\n";
    }

private:
    static void print_tree(std::ostream& os, FibonacciNode* node, int depth) {
        if (!node) return;

        FibonacciNode* current = node;
        do {
            for (int i = 0; i < depth; ++i) os << "  ";
            os << "[Prio: " << current->priority
               << ", Val: " << (current->value ? current->value : "null")
               << ", Deg: " << current->degree
               << (current->marked ? ", Marked" : "") << "]\n";

            if (current->child) {
                print_tree(os, current->child, depth + 1);
            }

            current = current->right;
        } while (current != node);
    }
};

int main() {
    try {
        std::cout << "Fibonacci Queue Basic Operations\n";
        std::cout << "-----------------------------------------\n";

        fibonacci_priority_queue fib_queue;
        fib_queue.add_value("Task X", 15);
        fib_queue.add_value("Task Y", 35);
        fib_queue.add_value("Task Z", 25);
        fib_queue.add_value("Task W", 45);

        print_fibonacci_queue(fib_queue, "Fibonacci Queue after adding 4 tasks");
        std::cout << "Max priority: " << fib_queue.search_value() << "\n\n";

        fib_queue.delete_value();
        print_fibonacci_queue(fib_queue, "Fibonacci Queue after delete");

        std::cout << "Merge Operations\n";
        std::cout << "------------------------\n";

        fibonacci_priority_queue fib_queue2;
        fib_queue2.add_value("Critical", 200);
        fib_queue2.add_value("Minor", 1);

        fib_queue.merge(fib_queue2);
        print_fibonacci_queue(fib_queue, "Fibonacci Queue after merge");

        std::cout << "Meld Operations\n";
        std::cout << "-----------------------\n";

        auto fib_result = fib_queue.meld(fib_queue2);
        print_fibonacci_queue(fib_result, "Fibonacci Meld Result");

    }
    catch (const char* msg) {
        std::cerr << "Error: " << msg << "\n";
        return 1;
    }

    return 0;
}
