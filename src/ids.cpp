#include "ids.hpp"
#include <cstdlib>
#include <random>
#include <sstream>
#include <iomanip>
#include <random>
#include <cstdint>

namespace ids {
    string getConfigurationVersionString() {
        static std::string charset = "abcdefghijklmnopqrstuvwxyz1234567890";
        std::string result;
        result.resize(40);
        for (int i = 0; i < 40; i++) {
            result[i] = charset[rand() % charset.length()];
        }
        return result;
    }

    string getUUID() {
        static UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
        UUIDv4::UUID uuid = uuidGenerator.getUUID();
        return uuid.str();
    }

    string getUUIDFor(string seed) {
        static hash<string> hash_func;
        static mt19937_64 gen;
        static std::uniform_int_distribution<uint64_t> dis(0, UINT64_MAX);

        gen.seed(hash_func(seed));

        uint64_t uuid_high = dis(gen);
        uint64_t uuid_low = dis(gen);

        std::stringstream ss;
        ss << std::hex << std::setw(8) << std::setfill('0') << (uuid_high >> 32)
           << "-" << std::setw(4) << (uuid_high >> 16 & 0xffff)
           << "-" << std::setw(4) << (uuid_high & 0xffff)
           << "-" << std::setw(4) << (uuid_low >> 48)
           << "-" << std::setw(12) << (uuid_low & 0xffffffffffff);

        return ss.str();
    }
}