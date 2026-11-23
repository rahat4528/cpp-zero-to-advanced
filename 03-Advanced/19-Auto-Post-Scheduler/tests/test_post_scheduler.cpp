#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "PostScheduler.hpp"
#include <thread>
#include <chrono>

using namespace scheduler;

class FailingBackend : public IPostingBackend {
public:
    std::string getPlatformName() const override { return "FailingChannel"; }
    bool dispatchPost(const ScheduledPost&, std::string& outResponse) override {
        outResponse = "HTTP 500 Internal Server Error";
        return false;
    }
};

TEST_SUITE("Auto-Post Scheduler and Dispatch Pipeline") {
    TEST_CASE("Database Persistence and Due Job Retrieval") {
        PostDatabase db(":memory:");
        REQUIRE(db.open());

        int id1 = db.schedulePost("Twitter", "Hello World", 1000);
        int id2 = db.schedulePost("LinkedIn", "Career Update", 2000);
        CHECK(id1 > 0);
        CHECK(id2 > 0);

        // At epoch 1500, only id1 is due
        auto due1500 = db.getPendingDuePosts(1500);
        REQUIRE(due1500.size() == 1);
        CHECK(due1500[0].id == id1);

        // At epoch 2500, both are due
        auto due2500 = db.getPendingDuePosts(2500);
        REQUIRE(due2500.size() == 2);
    }

    TEST_CASE("Scheduler Worker Execution and Pluggable Mock Backend") {
        PostScheduler s(":memory:");
        int id = s.schedule("Twitter", "Automated Tweet", 0);

        // Run immediate dispatch
        int dispatched = s.dispatchDueNow(std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count() + 10);

        CHECK(dispatched == 1);

        auto post = s.getDatabase().getPostById(id);
        REQUIRE(post.has_value());
        CHECK(post->status == "PUBLISHED");
        CHECK(post->responseData.find("Published to Twitter") != std::string::npos);
    }

    TEST_CASE("Retry Logic on Dispatch Failure") {
        PostScheduler s(":memory:");
        s.registerBackend(std::make_shared<FailingBackend>());

        int id = s.getDatabase().schedulePost("FailingChannel", "This will fail", 1000);
        int64_t now = 2000;

        int dispatched = s.dispatchDueNow(now);
        CHECK(dispatched == 0);

        auto post = s.getDatabase().getPostById(id);
        REQUIRE(post.has_value());
        CHECK(post->retryCount == 1);
        CHECK(post->status == "PENDING"); // queued for retry
    }
}
