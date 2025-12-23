#include <iostream>
#include <cstdio>
#include <string>
#include <random>
#include <vector>

const char HEX_CHARS[] = "0123456789abcdef";

void writeIPv4(FILE* fp, uint32_t ip) {
    char buf[16];
    int len = sprintf(buf, "%u.%u.%u.%u\n",
                      (ip >> 24) & 0xFF,
                      (ip >> 16) & 0xFF,
                      (ip >> 8) & 0xFF,
                      ip & 0xFF);
    fwrite(buf, 1, len, fp);
}

void writeIPv6(FILE* fp, std::mt19937& gen) {
    char buf[40];
    // 每次生成32位随机数，提供给2个IPv6段
    std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);
    
    for (int i = 0; i < 8; ++i) {
        static uint32_t randVal;
        if (i % 2 == 0) randVal = dis(gen); // 只在偶数次调用
        else randVal >>= 16;

        uint16_t section = static_cast<uint16_t>(randVal & 0xFFFF);
        
        // 字符映射
        int base = i * 5;
        buf[base]     = HEX_CHARS[(section >> 12) & 0xF];
        buf[base + 1] = HEX_CHARS[(section >> 8) & 0xF];
        buf[base + 2] = HEX_CHARS[(section >> 4) & 0xF];
        buf[base + 3] = HEX_CHARS[section & 0xF];
        
        if (i < 7) buf[base + 4] = ':';
    }

    buf[39] = '\n';
    fwrite(buf, 1, 40, fp);
}

void generate_ips(int version, long long count) {
    const char* output_file = "./ips.txt";
    std::cout << "Generating " << count << " IPv" << version << " addresses to " << output_file << "..." << std::endl;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> disV4(0, 0xFFFFFFFF);

    FILE* fp = fopen(output_file, "w");
    if (!fp) {
        std::cerr << "Error: Could not open " << output_file << " for writing." << std::endl;
        return;
    }

    // 设置一个较大的系统级缓冲区
    char setbuf_buffer[65536];
    setvbuf(fp, setbuf_buffer, _IOFBF, sizeof(setbuf_buffer));

    // 首行写入版本号
    fprintf(fp, "%d\n", version);

    for (long long i = 0; i < count; ++i) {
        if (version == 4) {
            writeIPv4(fp, disV4(gen));
        } else {
            writeIPv6(fp, gen);
        }

        if ((i + 1) % 100000 == 0 || i == count - 1) {
            std::cout << "\rProgress: " << (i + 1) << "/" << count << std::flush;
        }
    }

    fclose(fp);
    std::cout << "\nFinished." << std::endl;
}

int main(int argc, char* argv[]) {
    int version = 4;
    long long count = 100000;

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