#include "AuthManager.hpp"
#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
#include <random>

namespace crime_manager {

std::string AuthManager::sha256Hex(const std::string& input) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int length = 0;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
    EVP_DigestUpdate(ctx, input.data(), input.size());
    EVP_DigestFinal_ex(ctx, hash, &length);
    EVP_MD_CTX_free(ctx);

    std::ostringstream oss;
    for (unsigned int i = 0; i < length; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return oss.str();
}

std::string AuthManager::hashPassword(const std::string& password, const std::string& salt) {
    return sha256Hex(salt + "$" + password + "$pepper987");
}

std::string AuthManager::generateSalt() {
    static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dist(0, sizeof(charset) - 2);

    std::string salt;
    salt.reserve(16);
    for (size_t i = 0; i < 16; ++i) {
        salt += charset[dist(gen)];
    }
    return salt;
}

std::string AuthManager::roleToString(UserRole role) {
    switch (role) {
        case UserRole::Admin:     return "Admin";
        case UserRole::Detective: return "Detective";
        case UserRole::Officer:
        default:                  return "Officer";
    }
}

UserRole AuthManager::stringToRole(const std::string& str) {
    if (str == "Admin") return UserRole::Admin;
    if (str == "Detective") return UserRole::Detective;
    return UserRole::Officer;
}

bool AuthManager::hasPermission(UserRole role, const std::string& action) {
    if (role == UserRole::Admin) {
        return true; // Admin has all permissions
    }
    if (role == UserRole::Detective) {
        if (action == "CREATE_CASE" || action == "UPDATE_STATUS" ||
            action == "VIEW_CASES" || action == "VIEW_AUDIT") {
            return true;
        }
        return false;
    }
    if (role == UserRole::Officer) {
        if (action == "CREATE_CASE" || action == "VIEW_PUBLIC_CASES") {
            return true;
        }
        return false;
    }
    return false;
}

} // namespace crime_manager
