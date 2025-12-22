#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <vector>

std::string intToIp(uint32_t ip) {
    return std::to_string((ip >> 24) & 0xFF) + "." +
           std::to_string((ip >> 16) & 0xFF) + "." +
           std::to_string((ip >> 8) & 0xFF) + "." +
           std::to_string(ip & 0xFF);
}

void generate_random_ipv4(long long count) {
    const std::string output_file = "./ips.txt";
    std::cout << "Generating " << count << " IPv4 addresses to " << output_file << "..." << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);

    std::ofstream outfile(output_file);
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open " << output_file << " for writing." << std::endl;
        return;
    }

    // 设置64KB缓冲区
    std::vector<char> buffer(65536);
    outfile.rdbuf()->pubsetbuf(buffer.data(), buffer.size());

    for (long long i = 0; i < count; ++i) {
        uint32_t ip_int = dis(gen);
        outfile << intToIp(ip_int) << "\n";

        if ((i + 1) % 100000 == 0) {
            std::cout << "\rGenerating... " << (i + 1) << "/" << count << std::flush;
        }
    }

    outfile.close();
    std::cout << "\nFinished." << std::endl;
}

int main(int argc, char* argv[]) {
    long long count = 100000;

    if (argc >= 2) {
        try {
            count = std::stoll(argv[1]);
        } catch (...) {
            std::cerr << "Invalid argument. Using default: 100,000" << std::endl;
        }
    }

    generate_random_ipv4(count);

    return 0;
}