#pragma once

#include "FaceDescriptor.hpp"
#include <string>
#include <map>
#include <optional>

namespace face_auth {

struct EnrolledUser {
    std::string username;
    std::vector<float> faceDescriptor;
    std::string enrollmentDate;
};

struct AuthResult {
    bool authenticated{false};
    std::string matchedUser;
    float matchDistance{0.0f};
    float confidence{0.0f};
    std::string diagnosticMessage;
};

class FaceAuthManager {
public:
    explicit FaceAuthManager(float threshold = 0.15f);

    void setDistanceThreshold(float t) noexcept { m_threshold = t; }
    [[nodiscard]] float getDistanceThreshold() const noexcept { return m_threshold; }

    bool enrollUser(const std::string& username, const FaceImage& faceImage);
    [[nodiscard]] AuthResult authenticate(const FaceImage& candidateFace) const;
    [[nodiscard]] size_t getEnrolledUserCount() const noexcept;

private:
    float m_threshold;
    std::map<std::string, EnrolledUser> m_users;
};

} // namespace face_auth
