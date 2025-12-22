#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdint>
#include "kd_tree.hpp"

// 解析 IP 字符串为 std::array<int, 4>
std::array<int, 4> parseIP(const std::string& ipStr) {
    std::array<int, 4> addr;
    std::stringstream ss(ipStr);
    std::string item;
    int i = 0;
    while (std::getline(ss, item, '.') && i < 4) {
        addr[i++] = std::stoi(item);
    }
    return addr;
}

int main() {
    KDTree<4, int> ipTree;
    std::ifstream inFile("ips.txt");

    if (!inFile) {
        std::cerr << "Cannot find ./ips.txt." << std::endl;
        return 1;
    }

    // 读取并插入数据
    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty()) {
            ipTree.insert(parseIP(line));
        }
    }
    inFile.close();

    // 处理用户输入的子网
    std::string subnetInput;
    std::cout << "Subnet (XX.XX.XX.XX/XX) or q to quit: ";
    std::cin >> subnetInput;

    while (subnetInput != "q") {
        size_t slash = subnetInput.find('/');
        if (slash == std::string::npos) {
            std::cerr << "Unknown subnet syntax." << std::endl;
            return 1;
        }

        std::string baseIp = subnetInput.substr(0, slash);
        int prefix = std::stoi(subnetInput.substr(slash + 1));

        // 将基础 IP 转为 32 位整数处理边界
        std::array<int, 4> baseAddr = parseIP(baseIp);
        uint32_t ipUint = 0;
        for (int i = 0; i < 4; ++i) {
            ipUint = (ipUint << 8) | (static_cast<uint8_t>(baseAddr[i]));
        }

        // 计算子网范围
        uint32_t mask = (prefix == 0) ? 0 : (0xFFFFFFFF << (32 - prefix));
        uint32_t startIp = ipUint & mask;
        uint32_t endIp = startIp | (~mask);

        // 将边界转回 4-D 坐标点
        std::array<int, 4> lowPoint, highPoint;
        for (int i = 0; i < 4; ++i) {
            lowPoint[3 - i] = (startIp >> (i * 8)) & 0xFF;
            highPoint[3 - i] = (endIp >> (i * 8)) & 0xFF;
        }

        // 执行 4-D Tree 范围查找
        std::vector<std::array<int, 4>> results = ipTree.rangeSearch(lowPoint, highPoint);

        // 打印输出
        for (const auto& ip : results) {
            printf("%d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
        }
        std::cout << results.size() << " IP(s) found." << std::endl;
        
        std::cout << "Subnet (XX.XX.XX.XX/XX) or q to quit: ";
        std::cin >> subnetInput;
    }
    
    return 0;
}