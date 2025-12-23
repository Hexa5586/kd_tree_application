#include <iostream>
#include <string>
#include <array>
#include "kd_tree.hpp"

std::array<int, 2> read2DPoint() {
    std::array<int, 2> p;
    for (int i = 0; i < 2; ++i) {
        if (!(std::cin >> p[i])) break;
    }
    return p;
}

int main() {
    KDTree<int, 2> kt;
    std::string cmd;

    std::cout << "2D-Tree CLI\nCommands: i(nsert) <v1 v2>, s(earch) <v1 v2>, d(elete) <v1 v2>, p(rint), q(uit)" << std::endl;

    while (true) {
        std::cout << "> ";
        if (!(std::cin >> cmd)) break;

        if (cmd == "i") {
            std::array<int, 2> p = read2DPoint();
            kt.insert(p);
            std::cout << "Inserted point." << std::endl;
        } 
        else if (cmd == "s") {
            std::array<int, 2> p = read2DPoint();
            if (kt.search(p)) {
                std::cout << "Found point." << std::endl;
            } else {
                std::cout << "Point not found." << std::endl;
            }
        } 
        else if (cmd == "d") {
            std::array<int, 2> p = read2DPoint();
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