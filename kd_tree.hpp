#pragma once

#include <iostream>
#include <array>
#include <vector>
#include <algorithm>

// KDNode类模板
template <typename T, size_t K>
class KDNode {
public:
    std::array<T, K> point;
    KDNode *left;
    KDNode *right;

    KDNode(const std::array<T, K>& p) 
        : point(p), left(nullptr), right(nullptr) {}
};

// KDTree类模板
template <typename T, size_t K>
class KDTree {
private:
    KDNode<T, K>* root;

    KDNode<T, K>* buildRecursive(std::vector<std::array<T, K>>& points, int left, int right, int depth);
    void clear(KDNode<T, K>* node);
    KDNode<T, K>* insertRecursive(KDNode<T, K>* node, const std::array<T, K>& p, int depth);
    bool searchRecursive(KDNode<T, K>* node, const std::array<T, K>& p, int depth) const;
    KDNode<T, K>* findMin(KDNode<T, K>* node, int dim, int depth);
    KDNode<T, K>* removeRecursive(KDNode<T, K>* node, const std::array<T, K>& p, int depth);
    void printSubtree(KDNode<T, K>* node, int depth) const;
    void rangeSearchRecursive(KDNode<T, K>* node, const std::array<T, K>& low, const std::array<T, K>& high, int depth, std::vector<std::array<T, K>>& results) const;

public:
    KDTree();
    ~KDTree();

    void build(const std::vector<std::array<T, K>>& pointList);
    void insert(const std::array<T, K>& p);
    bool search(const std::array<T, K>& p) const;
    void remove(const std::array<T, K>& p);
    void display() const;
    std::vector<std::array<T, K>> rangeSearch(const std::array<T, K>& low, const std::array<T, K>& high) const;
};


template <typename T, size_t K>
KDTree<T, K>::KDTree() : root(nullptr) {}

template <typename T, size_t K>
KDTree<T, K>::~KDTree() {
    clear(root);
}

// 根据现有的points数组递归建树
template <typename T, size_t K>
KDNode<T, K>* KDTree<T, K>::buildRecursive(std::vector<std::array<T, K>>& points, int left, int right, int depth) {
    if (left >= right) return nullptr;

    int axis = depth % K;
    int mid = left + (right - left) / 2;
    // 使用nth_element进行部分排序
    std::nth_element(points.begin() + left, 
                     points.begin() + mid, 
                     points.begin() + right,
                     [axis](const std::array<T, K>& a, const std::array<T, K>& b) {
                         return a[axis] < b[axis];
                     });

    // 创建节点
    KDNode<T, K>* node = new KDNode<T, K>(points[mid]);

    // 递归构建左右子树
    node->left = buildRecursive(points, left, mid, depth + 1);
    node->right = buildRecursive(points, mid + 1, right, depth + 1);

    return node;
}

// 从数组建树，外部接口
template <typename T, size_t K>
void KDTree<T, K>::build(const std::vector<std::array<T, K>>& pointList) {
    if (pointList.empty()) return;
    clear(root);
    // 原数据副本
    std::vector<std::array<T, K>> points = pointList;
    root = buildRecursive(points, 0, points.size(), 0);
}

template <typename T, size_t K>
void KDTree<T, K>::clear(KDNode<T, K>* node) {
    if (!node) return;
    clear(node->left);
    clear(node->right);
    delete node;
}

// 递归插入函数
template <typename T, size_t K>
KDNode<T, K>* KDTree<T, K>::insertRecursive(KDNode<T, K>* node, const std::array<T, K>& p, int depth) {
    if (node == nullptr) return new KDNode<T, K>(p);

    int cd = depth % K;
    if (p[cd] < node->point[cd])
        node->left = insertRecursive(node->left, p, depth + 1);
    else
        node->right = insertRecursive(node->right, p, depth + 1);

    return node;
}

// 从根部递归插入，外部接口
// 理想情况O(logn)，最坏情况O(n)
template <typename T, size_t K>
void KDTree<T, K>::insert(const std::array<T, K>& p) {
    root = insertRecursive(root, p, 0);
}

// 递归查找
template <typename T, size_t K>
bool KDTree<T, K>::searchRecursive(KDNode<T, K>* node, const std::array<T, K>& p, int depth) const {
    if (node == nullptr) return false;
    if (node->point == p) return true;

    int cd = depth % K;
    if (p[cd] < node->point[cd])
        return searchRecursive(node->left, p, depth + 1);
    else
        return searchRecursive(node->right, p, depth + 1);
}

// 从根部递归查找，外部接口
// 理想情况O(logn)，最坏情况O(n)
template <typename T, size_t K>
bool KDTree<T, K>::search(const std::array<T, K>& p) const {
    return searchRecursive(root, p, 0);
}

// 查找指定维度最小值
template <typename T, size_t K>
KDNode<T, K>* KDTree<T, K>::findMin(KDNode<T, K>* node, int dim, int depth) {
    if (node == nullptr) return nullptr;

    int cd = depth % K;
    if (cd == dim) {
        if (node->left == nullptr) return node;
        return findMin(node->left, dim, depth + 1);
    }

    KDNode<T, K>* leftMin = findMin(node->left, dim, depth + 1);
    KDNode<T, K>* rightMin = findMin(node->right, dim, depth + 1);
    
    KDNode<T, K>* res = node;
    if (leftMin && leftMin->point[dim] < res->point[dim]) res = leftMin;
    if (rightMin && rightMin->point[dim] < res->point[dim]) res = rightMin;
    return res;
}

// 递归删除节点
// 删除部分的时间复杂度：O(n^(1-1/k))，在维度很大时可退化为近似O(n)
template <typename T, size_t K>
KDNode<T, K>* KDTree<T, K>::removeRecursive(KDNode<T, K>* node, const std::array<T, K>& p, int depth) {
    if (node == nullptr) return nullptr;

    int cd = depth % K;
    if (node->point == p) {
        if (node->right != nullptr) {
            KDNode<T, K>* minNode = findMin(node->right, cd, depth + 1);
            node->point = minNode->point;
            node->right = removeRecursive(node->right, minNode->point, depth + 1);
        } else if (node->left != nullptr) {
            KDNode<T, K>* minNode = findMin(node->left, cd, depth + 1);
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

// 删除某一节点，外部接口
template <typename T, size_t K>
void KDTree<T, K>::remove(const std::array<T, K>& p) {
    root = removeRecursive(root, p, 0);
}

// 递归进行范围查找
// O(n^(1-1/k)+m)，在维度很大时可退化为近似O(n+m)
template <typename T, size_t K>
void KDTree<T, K>::rangeSearchRecursive(KDNode<T, K>* node, 
                            const std::array<T, K>& low, 
                            const std::array<T, K>& high, 
                            int depth, 
                            std::vector<std::array<T, K>>& results) const {
    if (node == nullptr) return;

    // 检查当前点是否在[low, high]指定的超矩形范围内
    bool inRange = true;
    for (size_t i = 0; i < K; ++i) {
        if (node->point[i] < low[i] || node->point[i] > high[i]) {
            inRange = false;
            break;
        }
    }
    if (inRange) results.push_back(node->point);

    int cd = depth % K;
    // 剪枝：如果当前节点的分割维度值>=范围最小值，则去左子树找
    if (node->point[cd] >= low[cd]) {
        rangeSearchRecursive(node->left, low, high, depth + 1, results);
    }
    // 剪枝：如果当前节点的分割维度值<=范围最大值，则去右子树找
    if (node->point[cd] <= high[cd]) {
        rangeSearchRecursive(node->right, low, high, depth + 1, results);
    }
}

// 范围查找，外部接口
template <typename T, size_t K>
std::vector<std::array<T, K>> KDTree<T, K>::rangeSearch(const std::array<T, K>& low, 
                                                        const std::array<T, K>& high) const {
    std::vector<std::array<T, K>> results;
    rangeSearchRecursive(root, low, high, 0, results);
    return results;
}

// 递归地打印KD树的树形结构
template <typename T, size_t K>
void KDTree<T, K>::printSubtree(KDNode<T, K>* node, int depth) const {
    if (node == nullptr) return;

    std::cout << "[";
    for (size_t i = 0; i < K; ++i) {
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

// 打印整棵树的树形结构，外部接口
template <typename T, size_t K>
void KDTree<T, K>::display() const {
    if (!root) {
        std::cout << "Heap is empty." << std::endl;
        return;
    }
    printSubtree(root, 0);
    std::cout << "\n";
}