#pragma once

#include <iostream>
#include "set.h"


template<class T, class Comparator>
template<class I>
requires std::forward_iterator<I> Set<T, Comparator>::Set(I begin, I end) : Set() {
    for (auto iter = begin; iter != end; ++iter) {
        insert(*iter);
    }
}

template<class T, class Comparator>
Set<T, Comparator>::Set(std::initializer_list<T> initList) : Set() {
    for (auto elem : initList) {
        insert(elem);
    }
}

template<class T, class Comparator>
typename Set<T, Comparator>::iterator Set<T, Comparator>::lower_bound(const T& key) const {
    if (empty()) {
        return end();
    }
    auto result = end();
    Node* node = root;
    while (true) {
        auto lb = std::ranges::find_if_not(node->iters, less(key)); // lower bound
        if (lb != node->iters.end()) {
            result = *lb;
        }
        if (node->leaf) {
            break;
        }
        node = node->children[lb - node->iters.begin()];
    }
    return result;
}

template<class T, class Comparator>
typename Set<T, Comparator>::iterator Set<T, Comparator>::find(const T& key) const {
    auto result = lower_bound(key);
    if (result == end()) {
        return end();
    }
    if (comp(*result, key) || comp(key, *result)) {
        return end();
    }
    return result;
}

template<class T, class Comparator>
Set<T, Comparator>::Set(const Set& other) : Set(other.begin(), other.end()) {
}

template<class T, class Comparator>
Set<T, Comparator>::Set() : root(nullptr) {
}

template<class T, class Comparator>
void Set<T, Comparator>::insert(const T& key) {
    if (find(key) != end()) {
        return;
    }

    if (empty()) {
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

template<class T, class Comparator>
typename Set<T, Comparator>::iterator Set<T, Comparator>::begin() const {
    return list.begin();
}

template<class T, class Comparator>
typename Set<T, Comparator>::iterator Set<T, Comparator>::end() const {
    return list.end();
}

template<class T, class Comparator>
size_t Set<T, Comparator>::size() const {
    return list.size();
}

template<class T, class Comparator>
bool Set<T, Comparator>::empty() const {
    return list.empty();
}

template<class T, class Comparator>
std::function<bool(const typename Set<T, Comparator>::iterator&)> Set<T, Comparator>::less(const T& key) const {
    return [&](const iterator& iter) {
        return comp(*iter, key);
    };
}

template<class T, class Comparator>
bool Set<T, Comparator>::full(Set::Node* node) const {
    return node->iters.size() == 2 * t - 1;
}

template<class T, class Comparator>
void Set<T, Comparator>::insertNonFull(Set::Node* node, const T& key) {
    while (!node->leaf) {
        auto before = std::ranges::find_if_not(node->iters, less(key));
        size_t pos = before - node->iters.begin();
        if (full(node->children[pos])) {
            splitChild(node, pos);
            // после разбиения потомка в текущий узел из него поднялся ключ, надо сравниться и с ним
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

template<class T, class Comparator>
void Set<T, Comparator>::splitChild(Set::Node* x, size_t i) {
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

template<class T, class Comparator>
Set<T, Comparator>::~Set() {
    delete root;
}

template<class T, class Comparator>
Set<T, Comparator>& Set<T, Comparator>::operator=(const Set& other) {
    if (this == &other) {
        return *this;
    }
    delete root;
    root = nullptr;
    list = {};
    for (auto elem : other) {
        insert(elem);
    }
    return *this;
}


template<class T, class Comparator>
Set<T, Comparator>::Node::~Node() {
    for (auto child : children) {
        delete child;
    }
}

template<class T, class Comparator>
Set<T, Comparator>::Node::Node(bool leaf) : leaf(leaf) {
}
