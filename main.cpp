#include <iostream>
#include <vector>
#include <algorithm>
#include <list>


template<typename T, class Comparator = std::less<T>>
class BTree {
public:
    using iterator = typename std::list<T>::iterator;

    explicit BTree() : root(nullptr), t(3), less([](const T& key) {
        static Comparator comp;
        return [&](const iterator& iter) {
            return comp(*iter, key);
        };
    }) {
    }

    ~BTree() {
        delete root;
    }

    void insert(const T& key) {
        if (!root) {
            root = new Node(true);
            list.push_back(key);
            root->iters.push_back(list.begin());
            return;
        }

        if (full(root)) {
            Node* tmp = new Node(false);
            tmp->children.push_back(root);
            root = tmp;
            splitChild(root, 0);
        }

        insertNonFull(root, key);
    }

    iterator begin() {
        return list.begin();
    }

    iterator end() {
        return list.end();
    }

private:
    struct Node {
        explicit Node(bool leaf) : leaf(leaf) {
        }

        ~Node() {
            for (Node* child : children) {
                delete child;
            }
        }

        bool leaf;
        std::vector<iterator> iters;
        std::vector<Node*> children;
    };

    Node* root;
    const size_t t;
    const std::function<std::function<bool(const iterator&)>(const T&)> less;
    std::list<T> list;

    bool full(Node* node) {
        return node->iters.size() == 2 * t - 1;
    }

    void insertNonFull(Node* node, const T& key) {
        while (!node->leaf) {
            auto insertTo = std::ranges::find_if_not(node->iters, less(key));
            size_t pos = insertTo - node->iters.begin();
            if (full(node->children[pos])) {
                splitChild(node, pos);
                // после разбиения потомка в текущий узел из него поднялся ключ
                // надо сравниться и с ним
                if (less(key)(node->iters[pos])) { // здесь замыкание. "если *node->iters[pos] < key"
                    ++pos;
                }
            }
            node = node->children[pos];
        }
        auto insertTo = std::ranges::find_if_not(node->iters, less(key));
        if (insertTo == node->iters.end()) {
            node->iters.insert(insertTo, list.insert(std::next(*std::prev(insertTo)), key));
        }
        else {
            node->iters.insert(insertTo, list.insert(*insertTo, key));
        }
    }

    void splitChild(Node* x, size_t i) {
        // x — parent, y — child, z — new child
        // описание алгоритма (псевдокод)         https://imgur.com/a/ULdgO3x
        Node* y = x->children[i];
        Node* z = new Node(y->leaf);
        std::copy_n(y->iters.begin() + t, t - 1, std::back_inserter(z->iters));

        if (!y->leaf) {
            std::copy_n(y->children.begin() + t, t, std::back_inserter(z->children));
            y->children.resize(t);
        }
        x->children.insert(x->children.begin() + i + 1, z);
        x->iters.insert(x->iters.begin() + i, y->iters[t - 1]);
        y->iters.resize(t - 1);
    }
};


void run(std::istream& in, std::ostream& out) {
    BTree<int> tree;
    tree.insert(1);
    tree.insert(324);
    tree.insert(-7);
    for (auto& i : tree) {
        out << i << ' ';
    }
    //    size_t t;
    //    in >> t;
    //    BTree<int> tree(t);
    //    for (int x; in >> x; tree.insert(x)) {
    //    }
    //
    //    std::vector<BTree<int>::Node*> level = {tree.root};
    //    std::vector<BTree<int>::Node*> nextLevel;
    //    while (!level.empty()) {
    //        for (auto& node : level) {
    //            for (auto& key : node->iters) {
    //                out << key << ' ';
    //            }
    //            for (auto& child : node->children) {
    //                nextLevel.push_back(child);
    //            }
    //        }
    //        out << '\n';
    //        level.swap(nextLevel);
    //        nextLevel = {};
    //    }
}


int main() {
    run(std::cin, std::cout);
    return 0;
}