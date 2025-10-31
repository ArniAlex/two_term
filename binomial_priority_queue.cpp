#include <iostream>
#include <cstring>
#include "../../source/repos/second_sem_labs/priority_queue.h"

class binomial_priority_queue final : public priority_queue {
private:
    struct BinomialNode {
        char* value;
        int priority;
        int degree;
        BinomialNode* child;
        BinomialNode* sibling;
        BinomialNode* parent;

        BinomialNode(const char* str, int p) : priority(p), degree(0),
                                              child(nullptr), sibling(nullptr), parent(nullptr) {
            if (str && std::strlen(str) > 0) {
                value = new char[std::strlen(str) + 1];
                std::strcpy(value, str);
            } else {
                value = nullptr;
            }
        }

        BinomialNode(const BinomialNode& other) : priority(other.priority), degree(other.degree),
                                                 child(nullptr), sibling(nullptr), parent(nullptr) {
            if (other.value) {
                value = new char[std::strlen(other.value) + 1];
                std::strcpy(value, other.value);
            } else {
                value = nullptr;
            }
        }

        ~BinomialNode() {
            delete[] value;
        }
    };

    BinomialNode* head;

private:
    BinomialNode* link_trees(BinomialNode* tree1, BinomialNode* tree2) {
        if (tree1->priority < tree2->priority) {
            BinomialNode* temp = tree1;
            tree1 = tree2;
            tree2 = temp;
        }

        tree2->parent = tree1;
        tree2->sibling = tree1->child;
        tree1->child = tree2;
        tree1->degree++;
        return tree1;
    }

    BinomialNode* merge_lists(BinomialNode* list1, BinomialNode* list2) {
        if (!list1) return list2;
        if (!list2) return list1;

        BinomialNode* new_head = nullptr;
        BinomialNode** current = &new_head;

        while (list1 && list2) {
            if (list1->degree <= list2->degree) {
                *current = list1;
                list1 = list1->sibling;
            } else {
                *current = list2;
                list2 = list2->sibling;
            }
            current = &((*current)->sibling);
        }

        if (list1) *current = list1;
        if (list2) *current = list2;

        return new_head;
    }

    BinomialNode* consolidate(BinomialNode* list) {
        if (!list) return nullptr;

        const int MAX_DEGREE = 32;
        BinomialNode* degree_table[MAX_DEGREE] = {nullptr};

        BinomialNode* current = list;
        BinomialNode* prev = nullptr;

        while (current) {
            BinomialNode* node = current;
            current = current->sibling;

            while (degree_table[node->degree]) {
                BinomialNode* other = degree_table[node->degree];
                degree_table[node->degree] = nullptr;
                node = link_trees(node, other);
            }

            degree_table[node->degree] = node;
        }

        BinomialNode* new_head = nullptr;
        BinomialNode** tail = &new_head;

        for (int i = 0; i < MAX_DEGREE; ++i) {
            if (degree_table[i]) {
                *tail = degree_table[i];
                (*tail)->sibling = nullptr;
                tail = &((*tail)->sibling);
            }
        }

        return new_head;
    }

    BinomialNode* reverse_list(BinomialNode* list) {
        BinomialNode* prev = nullptr;
        while (list) {
            BinomialNode* next = list->sibling;
            list->sibling = prev;
            list->parent = nullptr;
            prev = list;
            list = next;
        }
        return prev;
    }

    BinomialNode* copy_tree(BinomialNode* node) const {
        if (!node) return nullptr;
        BinomialNode* new_node = new BinomialNode(node->value, node->priority);
        new_node->degree = node->degree;
        new_node->child = copy_tree(node->child);
        if (new_node->child) new_node->child->parent = new_node;
        return new_node;
    }

    BinomialNode* copy_list(BinomialNode* list) const {
        if (!list) return nullptr;
        BinomialNode* new_list = copy_tree(list);
        new_list->sibling = copy_list(list->sibling);
        return new_list;
    }

    void delete_tree(BinomialNode* node) {
        if (!node) return;
        delete_tree(node->child);
        delete_tree(node->sibling);
        delete node;
    }

    [[nodiscard]] int count_nodes(BinomialNode* node) const {
        if (!node) return 0;
        return 1 + count_nodes(node->child) + count_nodes(node->sibling);
    }

    [[nodiscard]] BinomialNode* find_max_node() const {
        if (!head) return nullptr;

        BinomialNode* max_node = head;
        BinomialNode* current = head->sibling;

        while (current) {
            if (current->priority > max_node->priority) {
                max_node = current;
            }
            current = current->sibling;
        }

        return max_node;
    }

    void print_tree(std::ostream& os, BinomialNode* node, int depth = 0) const {
        if (!node) return;

        for (int i = 0; i < depth; ++i) os << "  ";
        os << "B" << node->degree << " [Prio: " << node->priority
           << ", Val: " << (node->value ? node->value : "null") << "]\n";

        print_tree(os, node->child, depth + 1);
        print_tree(os, node->sibling, depth);
    }

public:
    binomial_priority_queue() : head(nullptr) {}

    binomial_priority_queue(const binomial_priority_queue& other) : head(nullptr) {
        if (other.head) {
            head = copy_list(other.head);
        }
    }

    binomial_priority_queue& operator=(const binomial_priority_queue& other) {
        if (this != &other) {
            delete_tree(head);
            head = other.head ? copy_list(other.head) : nullptr;
        }
        return *this;
    }

    binomial_priority_queue(binomial_priority_queue&& other) noexcept : head(other.head) {
        other.head = nullptr;
    }

    binomial_priority_queue& operator=(binomial_priority_queue&& other) noexcept {
        if (this != &other) {
            delete_tree(head);
            head = other.head;
            other.head = nullptr;
        }
        return *this;
    }

    ~binomial_priority_queue() {
        delete_tree(head);
    }

    void add_value(const char* str, int priority) override {
        if (!str) throw "Null pointer";
        if (std::strlen(str) == 0) throw "Empty string";

        binomial_priority_queue temp;
        temp.head = new BinomialNode(str, priority);
        merge(temp);
    }

    [[nodiscard]] const char* search_value() const override {
        BinomialNode* max_node = find_max_node();
        if (!max_node) throw "Queue is empty";
        return max_node->value;
    }

    void delete_value() override {
        BinomialNode* max_node = find_max_node();
        if (!max_node) throw "Queue is empty";

        if (max_node == head) {
            head = head->sibling;
        } else {
            BinomialNode* prev = head;
            while (prev->sibling != max_node) {
                prev = prev->sibling;
            }
            prev->sibling = max_node->sibling;
        }

        BinomialNode* child_list = reverse_list(max_node->child);
        binomial_priority_queue temp;
        temp.head = child_list;
        merge(temp);

        max_node->child = nullptr;
        delete max_node;
    }

    priority_queue& merge(const priority_queue& second) override {
        const binomial_priority_queue* other_queue =
            dynamic_cast<const binomial_priority_queue*>(&second);

        if (!other_queue) {
            throw "Incompatible queue types for merge";
        }

        BinomialNode* other_head = copy_list(other_queue->head);
        head = merge_lists(head, other_head);
        head = consolidate(head);

        return *this;
    }

    [[nodiscard]] bool is_empty() const {
        return head == nullptr;
    }

    [[nodiscard]] int get_size() const {
        return count_nodes(head);
    }

    [[nodiscard]] binomial_priority_queue meld(const binomial_priority_queue& other) const {
        binomial_priority_queue result;
        result.head = copy_list(head);
        binomial_priority_queue temp;
        temp.head = copy_list(other.head);
        result.merge(temp);
        return result;
    }

    friend void print_binomial_queue(const binomial_priority_queue& queue, const char* name) {
        std::cout << name << " (size: " << queue.get_size() << "):\n";

        if (queue.head) {
            queue.print_tree(std::cout, queue.head);
        } else {
            std::cout << "  [EMPTY]\n";
        }
        std::cout << "\n";
    }
};

int main() {
    try {
        binomial_priority_queue queue1, queue2;

        queue1.add_value("first", 10);
        queue1.add_value("second", 20);
        queue2.add_value("third", 15);

        print_binomial_queue(queue1, "Queue 1");
        print_binomial_queue(queue2, "Queue 2");

        queue1.merge(queue2);
        print_binomial_queue(queue1, "After merge");

        std::cout << "Max value: " << queue1.search_value() << std::endl;

        queue1.delete_value();
        print_binomial_queue(queue1, "After delete");

    } catch (const char* error) {
        std::cerr << "Error: " << error << std::endl;
    }

    return 0;
}
