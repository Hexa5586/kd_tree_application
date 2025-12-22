#include <iostream>
#include <string>
#include <array>
#include "kd_tree.hpp"

// 定义维度和数据类型
const int K_VAL = 2;
using DataType = int;
using Point = std::array<DataType, K_VAL>;

// 连续读取K个坐标元素
Point readPoint() {
    Point p;
    for (int i = 0; i < K_VAL; ++i) {
        if (!(std::cin >> p[i])) break;
    }
    return p;
}

int main() {
    KDTree<K_VAL, DataType> kt;
    std::string cmd;

    std::cout << "2D-Tree CLI\nCommands: i(nsert) <v1 v2>, s(earch) <v1 v2>, d(elete) <v1 v2>, p(rint), q(uit)" << std::endl;

    while (true) {
        std::cout << "> ";
        if (!(std::cin >> cmd)) break;

        if (cmd == "i") {
            Point p = readPoint();
            kt.insert(p);
            std::cout << "Inserted point." << std::endl;
        } 
        else if (cmd == "s") {
            Point p = readPoint();
            if (kt.search(p)) {
                std::cout << "Found point." << std::endl;
            } else {
                std::cout << "Point not found." << std::endl;
            }
        } 
        else if (cmd == "d") {
            Point p = readPoint();
            kt.remove(p);
            std::cout << "Delete operation performed." << std::endl;
        } 
        else if (cmd == "p") {
            kt.display();
        } 
        else if (cmd == "q") {
            break;
        } 
        else {
            std::cout << "Unknown command." << std::endl;
            std::cin.clear();
            std::cin.ignore(1000, '\n');
        }
    }

    return 0;
}