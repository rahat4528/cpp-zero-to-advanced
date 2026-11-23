#include "FaceAuthManager.hpp"
#include <limits>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace face_auth {

FaceAuthManager::FaceAuthManager(float threshold)
    : m_threshold(threshold) {}

bool FaceAuthManager::enrollUser(const std::string& username, const FaceImage& faceImage) {
    if (username.empty() || faceImage.grayscale.empty()) return false;

    auto desc = FaceDescriptor::extractLBPFeatures(faceImage);
    if (desc.empty()) return false;

    EnrolledUser u;
    u.username = username;
    u.faceDescriptor = std::move(desc);
    u.enrollmentDate = "2026-11-23";
    m_users[username] = std::move(u);
    return true;
}

AuthResult FaceAuthManager::authenticate(const FaceImage& candidateFace) const {
    AuthResult res;
    if (m_users.empty()) {
        res.diagnosticMessage = "No users enrolled in database";
        return res;
    }

    auto candidateDesc = FaceDescriptor::extractLBPFeatures(candidateFace);
    if (candidateDesc.empty()) {
        res.diagnosticMessage = "Failed to extract candidate face descriptor";
        return res;
    }

    float bestDist = std::numeric_limits<float>::max();
    std::string bestUser;

    for (const auto& [name, user] : m_users) {
        float d = FaceDescriptor::computeChiSquareDistance(candidateDesc, user.faceDescriptor);
        if (d < bestDist) {
            bestDist = d;
            bestUser = name;
        }
    }

    res.matchDistance = bestDist;
    res.confidence = std::clamp(1.0f - (bestDist / m_threshold), 0.0f, 1.0f);

    if (bestDist <= m_threshold) {
        res.authenticated = true;
        res.matchedUser = bestUser;
        std::ostringstream oss;
        oss << "Access Granted to '" << bestUser << "' (Chi2 distance: " << std::fixed << std::setprecision(4)
            << bestDist << " <= threshold " << m_threshold << ")";
        res.diagnosticMessage = oss.str();
    } else {
        res.authenticated = false;
        std::ostringstream oss;
        oss << "Access Denied: Imposter or unrecognized face (Closest match: '" << bestUser
            << "', Chi2 distance: " << std::fixed << std::setprecision(4) << bestDist << " > threshold " << m_threshold << ")";
        res.diagnosticMessage = oss.str();
    }

    return res;
}

size_t FaceAuthManager::getEnrolledUserCount() const noexcept {
    return m_users.size();
}

} // namespace face_auth
