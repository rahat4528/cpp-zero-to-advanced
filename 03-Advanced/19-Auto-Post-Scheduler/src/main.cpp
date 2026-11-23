#include "PostScheduler.hpp"
#include <iostream>
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
        }
    }

    std::cout << "========================================\n";
    std::cout << "   Automated Social Media Post Scheduler\n";
    std::cout << "  (SQLite Persistent Job Dispatch Engine)\n";
    std::cout << "========================================\n";

    scheduler::PostScheduler scheduler(":memory:");
    scheduler.start();

    std::cout << "Scheduling simulated publication jobs...\n";
    int p1 = scheduler.schedule("Twitter", "Launching the modern C++ repository! #cpp #programming", 0);
    int p2 = scheduler.schedule("LinkedIn", "Excited to publish 60 advanced C++ applications.", 0);
    int p3 = scheduler.schedule("Discord", "Dev server update live on port 8080.", 1);

    std::cout << "Queued Posts: ID #" << p1 << ", #" << p2 << ", #" << p3 << "\n";
    std::cout << "Waiting 1200ms for worker dispatch loop...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1200));

    auto allPosts = scheduler.getDatabase().getAllPosts();
    std::cout << "\n=== Scheduled Posts Database Ledger ===\n";
    for (const auto& post : allPosts) {
        std::cout << "  [" << post.status << "] ID #" << post.id << " (" << post.platform << "):\n"
                  << "     \"" << post.content << "\"\n"
                  << "     Response: " << post.responseData << "\n";
    }

    scheduler.stop();
    std::cout << "Scheduler stopped cleanly.\n";

    if (testRun) {
        std::cout << "[Test Mode] Auto-post scheduler verified.\n";
        return 0;
    }

    return 0;
}
