#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <sstream>
#include <cstdint>
#include <iomanip>
#include <algorithm>
#include "kd_tree.hpp"

// IPv4解析
std::array<int, 4> parseIPv4(const std::string& ipStr) {
    std::array<int, 4> addr = {0};
    std::stringstream ss(ipStr);
    std::string item;
    int i = 0;
    while (std::getline(ss, item, '.') && i < 4) {
        addr[i++] = std::stoi(item);
    }
    return addr;
}

// IPv6解析，支持::连写的形式
std::array<int, 16> parseIPv6(const std::string& ipStr) {
    std::array<int, 16> addr = {0};
    std::vector<std::string> parts;
    std::string s = ipStr;
    size_t doubleColonPos = s.find("::");

    auto parseHexPart = [](const std::string& section, std::vector<uint8_t>& out) {
        std::stringstream ss(section);
        std::string item;
        while (std::getline(ss, item, ':')) {
            if (item.empty()) continue;
            uint32_t val = std::stoul(item, nullptr, 16);
            out.push_back((val >> 8) & 0xFF);
            out.push_back(val & 0xFF);
        }
    };

    if (doubleColonPos != std::string::npos) {
        std::vector<uint8_t> left, right;
        parseHexPart(s.substr(0, doubleColonPos), left);
        parseHexPart(s.substr(doubleColonPos + 2), right);
        
        int middleZeros = 16 - left.size() - right.size();
        int idx = 0;
        for (auto b : left) addr[idx++] = b;
        for (int i = 0; i < middleZeros; ++i) addr[idx++] = 0;
        for (auto b : right) addr[idx++] = b;
    } else {
        std::vector<uint8_t> all;
        parseHexPart(s, all);
        for (int i = 0; i < 16 && i < (int)all.size(); ++i) addr[i] = all[i];
    }
    return addr;
}

// IPv4模式
void runIPv4(std::ifstream& inFile) {
    KDTree<int, 4> ipTree;
    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty()) ipTree.insert(parseIPv4(line));
    }
    inFile.close();

    std::string input;
    while (true) {
        std::cout << "IPv4 Subnet (CIDR) or q: ";
        std::cin >> input;
        if (input == "q") break;

        size_t slash = input.find('/');
        if (slash == std::string::npos) continue;

        std::array<int, 4> base = parseIPv4(input.substr(0, slash));
        int prefix = std::stoi(input.substr(slash + 1));

        uint32_t ipUint = 0;
        for (int i = 0; i < 4; ++i) ipUint = (ipUint << 8) | (uint8_t)base[i];

        uint32_t mask = (prefix == 0) ? 0 : (0xFFFFFFFF << (32 - prefix));
        uint32_t start = ipUint & mask;
        uint32_t end = start | (~mask);

        std::array<int, 4> low, high;
        for (int i = 0; i < 4; ++i) {
            low[i] = (start >> (24 - i * 8)) & 0xFF;
            high[i] = (end >> (24 - i * 8)) & 0xFF;
        }

        auto results = ipTree.rangeSearch(low, high);
        
        // 结果写入到文件
        std::ofstream outFile("ips_query_result.txt", std::ios::trunc);
        if (outFile.is_open()) {
            for (const auto& ip : results) {
                char buf[64];
                sprintf(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
                outFile << buf << "\n";
            }
            outFile.close();
        }

        std::cout << results.size() << " IP(s) found and saved to ips_query_result.txt\n" << std::endl;
    }
}

// IPv6模式
void runIPv6(std::ifstream& inFile) {
    KDTree<int, 16> ipTree;
    std::string line;
    while (std::getline(inFile, line)) {
        if (!line.empty()) ipTree.insert(parseIPv6(line));
    }
    inFile.close();

    std::string input;
    while (true) {
        std::cout << "IPv6 Subnet (CIDR) or q: ";
        std::cin >> input;
        if (input == "q") break;

        size_t slash = input.find('/');
        if (slash == std::string::npos) continue;

        std::array<int, 16> base = parseIPv6(input.substr(0, slash));
        int prefix = std::stoi(input.substr(slash + 1));

        std::array<int, 16> low, high;
        for (int i = 0; i < 16; ++i) {
            int bitOffset = i * 8;
            if (prefix >= bitOffset + 8) {
                low[i] = high[i] = base[i];
            } else if (prefix <= bitOffset) {
                low[i] = 0; high[i] = 255;
            } else {
                int bits = prefix - bitOffset;
                uint8_t mask = 0xFF << (8 - bits);
                low[i] = base[i] & mask;
                high[i] = base[i] | (~mask & 0xFF);
            }
        }

        auto results = ipTree.rangeSearch(low, high);

        // 结果写入到文件
        std::ofstream outFile("ips_query_result.txt", std::ios::trunc);
        if (outFile.is_open()) {
            for (const auto& ip : results) {
                char buf[128];
                int pos = 0;
                for (int i = 0; i < 16; i += 2) {
                    pos += sprintf(buf + pos, "%02x%02x%c", ip[i], ip[i+1], (i == 14 ? '\0' : ':'));
                }
                outFile << buf << "\n";
            }
            outFile.close();
        }

        std::cout << results.size() << " IP(s) found and saved to ips_query_result.txt\n" << std::endl;
    }
}


int main() {
    std::ifstream inFile("ips.txt");
    if (!inFile) {
        std::cerr << "Error: Cannot open ips.txt" << std::endl;
        return 1;
    }

    // 读取首行，代表的是ips.txt中的IP版本
    std::string firstLine;
    if (!std::getline(inFile, firstLine)) return 1;

    int version = std::stoi(firstLine);
    if (version == 4) {
        std::cout << "Detected IPv4 mode." << std::endl;
        runIPv4(inFile);
    } else if (version == 6) {
        std::cout << "Detected IPv6 mode." << std::endl;
        runIPv6(inFile);
    } else {
        std::cerr << "Unsupported version in first line: " << version << std::endl;
        return 1;
    }

    return 0;
}