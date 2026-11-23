#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ChatbotClient.hpp"

TEST_CASE("ChatbotClient - History and Request Serialization") {
    chatbot::ChatbotClient client;
    client.clearHistory();
    client.setSystemPrompt("System Instructions");
    client.addUserMessage("Explain templates");

    const auto& hist = client.getHistory();
    REQUIRE(hist.size() == 2);
    CHECK(hist[0].role == chatbot::MessageRole::System);
    CHECK(hist[1].role == chatbot::MessageRole::User);

    std::string jsonReq = chatbot::ChatbotClient::serializeOpenAIRequest(hist, "gpt-4o-mini");
    CHECK(jsonReq.find("gpt-4o-mini") != std::string::npos);
    CHECK(jsonReq.find("Explain templates") != std::string::npos);
    CHECK(jsonReq.find("System Instructions") != std::string::npos);
}

TEST_CASE("ChatbotClient - Response Deserialization") {
    std::string sampleResponse = R"({
        "id": "chatcmpl-123",
        "choices": [{
            "index": 0,
            "message": {
                "role": "assistant",
                "content": "C++ templates enable compile-time parametric polymorphism."
            },
            "finish_reason": "stop"
        }],
        "usage": {
            "prompt_tokens": 14,
            "completion_tokens": 9
        }
    })";

    auto resp = chatbot::ChatbotClient::parseOpenAIResponse(sampleResponse);
    CHECK(resp.isSuccess == true);
    CHECK(resp.replyText == "C++ templates enable compile-time parametric polymorphism.");
    CHECK(resp.promptTokens == 14);
    CHECK(resp.completionTokens == 9);
    CHECK(resp.isMock == false);
}

TEST_CASE("ChatbotClient - Mock Generation & Conversation Progression") {
    chatbot::ChatbotClient client;
    auto resp = client.sendMessage("What is RAII in C++?");
    CHECK(resp.isSuccess == true);
    CHECK(resp.isMock == true);
    CHECK(resp.replyText.find("RAII") != std::string::npos);

    // History now contains system, user, and assistant reply
    CHECK(client.getHistory().size() >= 3);
}
