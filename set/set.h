#pragma once


#include <algorithm>
#include <vector>
#include <list>


template<class T, class Comparator = std::less<T>>
class Set {
public:
    using iterator = typename std::list<T>::const_iterator;

    Set();

    template<class I>
    requires std::forward_iterator<I> Set(I begin, I end);

    Set(std::initializer_list<T> initList);

    Set(const Set& other);

    Set& operator=(const Set& other);

    ~Set();

    iterator lower_bound(const T& key) const;

    iterator find(const T& key) const;

    void insert(const T& key);

    iterator begin() const;

    iterator end() const;

    [[nodiscard]] size_t size() const;

    [[nodiscard]] bool empty() const;

private:
    struct Node {
        explicit Node(bool leaf);

        ~Node();

        bool leaf;
        std::vector<iterator> iters;
        std::vector<Node*> children;
    };

    static constexpr Comparator comp = Comparator();
    static constexpr size_t t = 25; // значение t должно быть больше 1
    Node* root;
    std::list<T> list;

    std::function<bool(const iterator&)> less(const T& key) const;

    bool full(Node* node) const;

    void insertNonFull(Node* node, const T& key);

    void splitChild(Node* x, size_t i);
};


#include "set.cpp"