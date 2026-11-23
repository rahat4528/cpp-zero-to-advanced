#pragma once

#include <string>

namespace crime_manager {

enum class UserRole {
    Officer,
    Detective,
    Admin
};

struct UserSession {
    int userId{0};
    std::string username;
    UserRole role{UserRole::Officer};
    std::string sessionToken;
};

class AuthManager {
public:
    [[nodiscard]] static std::string sha256Hex(const std::string& input);
    [[nodiscard]] static std::string hashPassword(const std::string& password, const std::string& salt);
    [[nodiscard]] static std::string generateSalt();
    [[nodiscard]] static std::string roleToString(UserRole role);
    [[nodiscard]] static UserRole stringToRole(const std::string& str);
    [[nodiscard]] static bool hasPermission(UserRole role, const std::string& action);
};

} // namespace crime_manager
