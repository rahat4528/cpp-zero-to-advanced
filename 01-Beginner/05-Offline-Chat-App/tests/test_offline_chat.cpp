#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "OfflineChat.h"

#include <cstdio>
#include <fstream>

using namespace offline_chat;

TEST_CASE("Bot response heuristics") {
    ChatEngine engine(ChatMode::Bot);

    std::string rHello = engine.generateBotResponse("Hello bot!");
    CHECK(rHello.find("Hello") != std::string::npos);

    std::string rCpp = engine.generateBotResponse("Tell me about C++");
    CHECK(rCpp.find("Modern C++") != std::string::npos);

    std::string rTime = engine.generateBotResponse("What is the time?");
    CHECK(rTime.find("simulation time") != std::string::npos);

    std::string rUnknown = engine.generateBotResponse("xyzabc123");
    CHECK(rUnknown.find("Fascinating query") != std::string::npos);
}

TEST_CASE("Message sending and history recording") {
    ChatEngine engine(ChatMode::Bot);

    engine.sendMessage("Alice", "Hello there");
    const auto& hist = engine.getHistory();
    // User message + Bot response = 2 messages
    REQUIRE(hist.size() == 2);
    CHECK(hist[0].sender == "Alice");
    CHECK(hist[0].content == "Hello there");
    CHECK(hist[1].sender == "CyberBot");
}

TEST_CASE("Two-user mode without auto-bot responses") {
    ChatEngine engine(ChatMode::TwoUser);

    engine.sendMessage("UserA", "Hey UserB!");
    engine.sendMessage("UserB", "Hello UserA, how are you?");

    const auto& hist = engine.getHistory();
    REQUIRE(hist.size() == 2);
    CHECK(hist[0].sender == "UserA");
    CHECK(hist[1].sender == "UserB");
}

TEST_CASE("Slash command dispatch and clear") {
    ChatEngine engine(ChatMode::Bot);

    CHECK(engine.isCommand("/help") == true);
    CHECK(engine.isCommand("Regular message") == false);

    std::string helpOutput = engine.executeCommand("/help");
    CHECK(helpOutput.find("Available commands") != std::string::npos);

    engine.sendMessage("Bob", "Message 1");
    CHECK(engine.getHistory().size() == 2);

    engine.executeCommand("/clear");
    CHECK(engine.getHistory().empty());
}

TEST_CASE("Transcript file export") {
    const std::string logPath = "temp_chat_test.log";

    ChatEngine engine(ChatMode::TwoUser);
    engine.sendMessage("Dev1", "Commit 1 pushed");
    engine.sendMessage("Dev2", "Reviewing now");

    CHECK(engine.exportLog(logPath) == true);

    std::ifstream in(logPath);
    REQUIRE(in.is_open());
    std::string content((std::istreambuf_iterator<char>(in)),
                         std::istreambuf_iterator<char>());
    CHECK(content.find("Dev1: Commit 1 pushed") != std::string::npos);
    CHECK(content.find("Dev2: Reviewing now") != std::string::npos);

    in.close();
    std::remove(logPath.c_str());
}
