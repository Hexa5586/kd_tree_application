#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <algorithm>

/**
 * KDNode类模板
 */
template <int K, typename T>
class KDNode {
public:
    std::array<T, K> point;
    KDNode *left;
    KDNode *right;

    KDNode(const std::array<T, K>& p) 
        : point(p), left(nullptr), right(nullptr) {}
};

/**
 * KDTree类模板
 */
template <int K, typename T>
class KDTree {

private:
    KDNode<K, T>* root;

    // 内部辅助递归函数
    void clear(KDNode<K, T>* node);
    KDNode<K, T>* insertRecursive(KDNode<K, T>* node, const std::array<T, K>& p, int depth);
    bool searchRecursive(KDNode<K, T>* node, const std::array<T, K>& p, int depth) const;
    KDNode<K, T>* findMin(KDNode<K, T>* node, int dim, int depth);
    KDNode<K, T>* removeRecursive(KDNode<K, T>* node, const std::array<T, K>& p, int depth);
    void printSubtree(KDNode<K, T>* node, int depth) const; // 对应原私有 display
    void rangeSearchRecursive(KDNode<K, T>* node, const std::array<T, K>& low, const std::array<T, K>& high, int depth, std::vector<std::array<T, K>>& results) const;

public:
    KDTree();
    ~KDTree();

    void insert(const std::array<T, K>& p);
    bool search(const std::array<T, K>& p) const;
    void remove(const std::array<T, K>& p);
    void display() const;
    std::vector<std::array<T, K>> rangeSearch(const std::array<T, K>& low, const std::array<T, K>& high) const;
};


template <int K, typename T>
KDTree<K, T>::KDTree() : root(nullptr) {}

template <int K, typename T>
KDTree<K, T>::~KDTree() {
    clear(root);
}

template <int K, typename T>
void KDTree<K, T>::clear(KDNode<K, T>* node) {
    if (!node) return;
    clear(node->left);
    clear(node->right);
    delete node;
}

// 插入逻辑
template <int K, typename T>
KDNode<K, T>* KDTree<K, T>::insertRecursive(KDNode<K, T>* node, const std::array<T, K>& p, int depth) {
    if (node == nullptr) return new KDNode<K, T>(p);

    int cd = depth % K;
    if (p[cd] < node->point[cd])
        node->left = insertRecursive(node->left, p, depth + 1);
    else
        node->right = insertRecursive(node->right, p, depth + 1);

    return node;
}

template <int K, typename T>
void KDTree<K, T>::insert(const std::array<T, K>& p) {
    root = insertRecursive(root, p, 0);
}

// 查找逻辑
template <int K, typename T>
bool KDTree<K, T>::searchRecursive(KDNode<K, T>* node, const std::array<T, K>& p, int depth) const {
    if (node == nullptr) return false;
    if (node->point == p) return true;

    int cd = depth % K;
    if (p[cd] < node->point[cd])
        return searchRecursive(node->left, p, depth + 1);
    else
        return searchRecursive(node->right, p, depth + 1);
}

template <int K, typename T>
bool KDTree<K, T>::search(const std::array<T, K>& p) const {
    return searchRecursive(root, p, 0);
}

// 查找指定维度最小值
template <int K, typename T>
KDNode<K, T>* KDTree<K, T>::findMin(KDNode<K, T>* node, int dim, int depth) {
    if (node == nullptr) return nullptr;

    int cd = depth % K;
    if (cd == dim) {
        if (node->left == nullptr) return node;
        return findMin(node->left, dim, depth + 1);
    }

    KDNode<K, T>* leftMin = findMin(node->left, dim, depth + 1);
    KDNode<K, T>* rightMin = findMin(node->right, dim, depth + 1);
    
    KDNode<K, T>* res = node;
    if (leftMin && leftMin->point[dim] < res->point[dim]) res = leftMin;
    if (rightMin && rightMin->point[dim] < res->point[dim]) res = rightMin;
    return res;
}

// 删除逻辑
template <int K, typename T>
KDNode<K, T>* KDTree<K, T>::removeRecursive(KDNode<K, T>* node, const std::array<T, K>& p, int depth) {
    if (node == nullptr) return nullptr;

    int cd = depth % K;
    if (node->point == p) {
        if (node->right != nullptr) {
            KDNode<K, T>* minNode = findMin(node->right, cd, depth + 1);
            node->point = minNode->point;
            node->right = removeRecursive(node->right, minNode->point, depth + 1);
        } else if (node->left != nullptr) {
            KDNode<K, T>* minNode = findMin(node->left, cd, depth + 1);
            node->point = minNode->point;
            node->right = removeRecursive(node->left, minNode->point, depth + 1);
            node->left = nullptr;
        } else {
            delete node;
            return nullptr;
        }
        return node;
    }

    if (p[cd] < node->point[cd])
        node->left = removeRecursive(node->left, p, depth + 1);
    else
        node->right = removeRecursive(node->right, p, depth + 1);
    return node;
}

template <int K, typename T>
void KDTree<K, T>::remove(const std::array<T, K>& p) {
    root = removeRecursive(root, p, 0);
}

// 打印KD-TREE的树形结构
template <int K, typename T>
void KDTree<K, T>::printSubtree(KDNode<K, T>* node, int depth) const {
    if (node == nullptr) return;

    std::cout << "[";
    for (int i = 0; i < K; ++i) {
        std::cout << node->point[i] << (i == K - 1 ? "" : " | ");
    }
    std::cout << "]";

    if (node->left != nullptr || node->right != nullptr) {
        std::cout << " -> ( ";

        if (node->left == nullptr && node->right != nullptr) {
            std::cout << ", ";
            printSubtree(node->right, depth + 1);
        }
        else if (node->left != nullptr && node->right == nullptr) {
            printSubtree(node->left, depth + 1);
        }
        else {
            printSubtree(node->left, depth + 1);
            std::cout << ", ";
            printSubtree(node->right, depth + 1);
        }

        std::cout << " )";
    }
}

template <int K, typename T>
void KDTree<K, T>::rangeSearchRecursive(KDNode<K, T>* node, 
                            const std::array<T, K>& low, 
                            const std::array<T, K>& high, 
                            int depth, 
                            std::vector<std::array<T, K>>& results) const {
    if (node == nullptr) return;

    // 检查当前点是否在 [low, high] 指定的超矩形范围内
    bool inRange = true;
    for (int i = 0; i < K; ++i) {
        if (node->point[i] < low[i] || node->point[i] > high[i]) {
            inRange = false;
            break;
        }
    }
    if (inRange) results.push_back(node->point);

    int cd = depth % K;
    // 剪枝逻辑：如果当前节点的分割维度值 >= 范围最小值，则左子树可能存在符合条件的点
    if (node->point[cd] >= low[cd]) {
        rangeSearchRecursive(node->left, low, high, depth + 1, results);
    }
    // 剪枝逻辑：如果当前节点的分割维度值 <= 范围最大值，则右子树可能存在符合条件的点
    if (node->point[cd] <= high[cd]) {
        rangeSearchRecursive(node->right, low, high, depth + 1, results);
    }
}

template <int K, typename T>
std::vector<std::array<T, K>> KDTree<K, T>::rangeSearch(const std::array<T, K>& low, 
                                                        const std::array<T, K>& high) const {
    std::vector<std::array<T, K>> results;
    rangeSearchRecursive(root, low, high, 0, results);
    return results;
}

template <int K, typename T>
void KDTree<K, T>::display() const {
    if (!root) {
        std::cout << "Heap is empty." << std::endl;
        return;
    }
    printSubtree(root, 0);
    std::cout << "\n";
}