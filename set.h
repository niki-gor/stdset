#include <algorithm>
#include <vector>
#include <list>

// некоторые библиотеки структур данных поставляются как single-header файлы — я решил не отступать от традиции
template<typename T, class Comparator = std::less<T>>
class Set {
public:
    using iterator = typename std::list<T>::iterator;

    explicit Set() : root(nullptr) {
    }

    ~Set() {
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

    [[nodiscard]] size_t size() const {
        return list.size();
    }

    [[nodiscard]] bool empty() const {
        return list.empty();
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

    static const size_t t;
    Node* root;
    std::list<T> list;

    std::function<bool(const iterator&)> less(const T& key) {
        static Comparator comp;
        return [&](const iterator& iter) {
            return comp(*iter, key);
        };
    }

    bool full(Node* node) {
        return node->iters.size() == 2 * t - 1;
    }

    void insertNonFull(Node* node, const T& key) {
        while (!node->leaf) {
            auto before = std::ranges::find_if_not(node->iters, less(key));
            size_t pos = before - node->iters.begin();
            if (full(node->children[pos])) {
                splitChild(node, pos);
                // после разбиения потомка в текущий узел из него поднялся ключ
                // надо сравниться и с ним
                if (less(key)(node->iters[pos])) { // здесь замыкание. "если  *node->iters[pos]  <  key"
                    ++pos;
                }
            }
            node = node->children[pos];
        }
        auto before = std::ranges::find_if_not(node->iters, less(key));
        if (before == node->iters.end()) {
            node->iters.insert(before, list.insert(std::next(*std::prev(before)), key));
        }
        else {
            node->iters.insert(before, list.insert(*before, key));
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

template<class T, class Comparator>
const size_t Set<T, Comparator>::t = 25; // значение t должно быть больше 1