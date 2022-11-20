#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>


template<typename T, class Comparator = std::less<T>>
class BTree {
public:
    struct Node {
        explicit Node(bool leaf) : leaf(leaf) {
        }

        ~Node() {
            for (Node* child : children) {
                delete child;
            }
        }

        bool leaf;
        std::vector<T> keys;
        std::vector<Node*> children;
    };

    explicit BTree(size_t min_degree) : root(nullptr), t(min_degree), comparator() {
        assert(min_degree >= 2);
    }

    ~BTree() {
        delete root;
    }

    void insert(const T& key) {
        if (!root) {
            root = new Node(true);
        }

        if (full(root)) {
            Node* newRoot = new Node(false);
            newRoot->children.push_back(root);
            root = newRoot;
            splitChild(root, 0);
        }

        insertNonFull(root, key);
    }

    Node* root;
    size_t t;
    Comparator comparator;

    bool full(Node* node) {
        return node->keys.size() == 2 * t - 1;
    }

    void insertNonFull(Node* node, const T& key) {
        while (!node->leaf) {
            size_t pos = std::ranges::lower_bound(node->keys, key, comparator) - node->keys.begin();
            if (full(node->children[pos])) {
                splitChild(node, pos);
                // после разбиения потомка в текущий узел из него поднялся ключ
                // надо сравниться и с ним
                if (key > node->keys[pos]) {
                    ++pos;
                }
            }
            node = node->children[pos];
        }
        node->keys.insert(std::ranges::lower_bound(node->keys, key, comparator), key);
    }

    void splitChild(Node* x, size_t i) {
        Node* y = x->children[i];
        Node* z = new Node(y->leaf);
        std::copy_n(y->keys.begin() + t, t - 1, std::back_inserter(z->keys));
        if (!y->leaf) {
            std::copy_n(y->children.begin() + t, t, std::back_inserter(z->children));
            y->children.resize(t);
        }
        x->children.insert(x->children.begin() + i + 1, z);
        x->keys.insert(x->keys.begin() + i, y->keys[t - 1]);
        y->keys.resize(t - 1);
    }
};


void run(std::istream& in, std::ostream& out) {
    size_t t;
    in >> t;
    BTree<int> tree(t);
    for (int x; in >> x; tree.insert(x)) {
    }

    std::vector<BTree<int>::Node*> level = {tree.root};
    std::vector<BTree<int>::Node*> nextLevel;
    while (!level.empty()) {
        for (auto& node : level) {
            for (auto& key : node->keys) {
                out << key << ' ';
            }
            for (auto& child : node->children) {
                nextLevel.push_back(child);
            }
        }
        out << '\n';
        level.swap(nextLevel);
        nextLevel = {};
    }
}


int main() {
    run(std::cin, std::cout);
    return 0;
}