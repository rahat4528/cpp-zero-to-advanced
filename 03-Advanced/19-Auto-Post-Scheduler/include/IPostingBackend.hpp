#pragma once

#include <string>
#include <cstdint>

namespace scheduler {

struct ScheduledPost {
    int id{0};
    std::string platform;
    std::string content;
    int64_t scheduledEpochSec{0};
    std::string status{"PENDING"};
    int retryCount{0};
    int maxRetries{3};
    std::string responseData;
};

class IPostingBackend {
public:
    virtual ~IPostingBackend() = default;
    [[nodiscard]] virtual std::string getPlatformName() const = 0;
    virtual bool dispatchPost(const ScheduledPost& post, std::string& outResponse) = 0;
};

class MockPostingBackend : public IPostingBackend {
public:
    explicit MockPostingBackend(const std::string& platform = "MockSocial")
        : m_platform(platform) {}

    [[nodiscard]] std::string getPlatformName() const override { return m_platform; }

    bool dispatchPost(const ScheduledPost& post, std::string& outResponse) override {
        outResponse = "DISPATCH_ACK: Published to " + m_platform + " [ID: " + std::to_string(post.id) + "]";
        return true;
    }

private:
    std::string m_platform;
};

} // namespace scheduler
