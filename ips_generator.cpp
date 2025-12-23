#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <iomanip>
#include <sstream>

// 将32位无符号整数转为IPv4字符串
std::string intToIp(uint32_t ip) {
    return std::to_string((ip >> 24) & 0xFF) + "." +
           std::to_string((ip >> 16) & 0xFF) + "." +
           std::to_string((ip >> 8) & 0xFF) + "." +
           std::to_string(ip & 0xFF);
}

// 生成随机IPv6字符串
std::string generateIPv6String(std::mt19937& gen) {
    std::uniform_int_distribution<uint32_t> dis(0, 0xFFFF);
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < 8; ++i) {
        ss << std::setw(4) << dis(gen);
        if (i < 7) ss << ":";
    }
    return ss.str();
}

void generate_ips(int version, long long count) {
    const std::string output_file = "./ips.txt";
    std::cout << "Generating " << count << " IPv" << version << " addresses to " << output_file << "..." << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> disV4(0, 0xFFFFFFFF);

    std::ofstream outfile(output_file);
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open " << output_file << " for writing." << std::endl;
        return;
    }

    // 设置缓冲区
    std::vector<char> write_buffer(65536);
    outfile.rdbuf()->pubsetbuf(write_buffer.data(), write_buffer.size());

    // 首行写入版本号
    outfile << version << "\n";
    for (long long i = 0; i < count; ++i) {
        if (version == 4) {
            outfile << intToIp(disV4(gen)) << "\n";
        } else {
            outfile << generateIPv6String(gen) << "\n";
        }

        if ((i + 1) % 100000 == 0 || i == count - 1) {
            std::cout << "\rProgress: " << (i + 1) << "/" << count << std::flush;
        }
    }

    outfile.close();
    std::cout << "\nFinished." << std::endl;
}

int main(int argc, char* argv[]) {
    int version = 4;
    long long count = 100000;

    // 命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-v" && i + 1 < argc) {
            version = std::stoi(argv[++i]);
        } else if (arg == "-n" && i + 1 < argc) {
            count = std::stoll(argv[++i]);
        }
    }

    if (version != 4 && version != 6) {
        std::cerr << "Unsupported version: " << version << ". Defaulting to 4." << std::endl;
        version = 4;
    }

    generate_ips(version, count);

    return 0;
}