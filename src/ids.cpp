#include "ids.hpp"
#include <cstdlib>
#include <random>

std::string ids::getConfigurationVersionString() {
    static std::string charset = "abcdefghijklmnopqrstuvwxyz1234567890";
    std::string result;
    result.resize(40);
    for (int i = 0; i < 40; i++) {
        result[i] = charset[rand() % charset.length()];
    }
    return result;
}

std::string ids::getUUID() {
    static UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
    UUIDv4::UUID uuid = uuidGenerator.getUUID();
    return uuid.str();
}