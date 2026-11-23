#pragma once

#include <string>
#include <vector>
#include <optional>

namespace chatbot {

enum class MessageRole {
    System,
    User,
    Assistant
};

struct ChatMessage {
    MessageRole role;
    std::string content;
};

struct ChatbotResponse {
    std::string replyText;
    int promptTokens{0};
    int completionTokens{0};
    bool isMock{false};
    bool isSuccess{true};
    std::string errorMessage;
};

class ChatbotClient {
public:
    explicit ChatbotClient(std::string apiKey = "", std::string endpointUrl = "");

    void setSystemPrompt(const std::string& systemPrompt);
    void addUserMessage(const std::string& userText);
    void addAssistantMessage(const std::string& assistantText);
    void clearHistory();

    [[nodiscard]] const std::vector<ChatMessage>& getHistory() const;

    // Send conversation to LLM or fallback to local deterministic mock
    ChatbotResponse sendMessage(const std::string& userText);

    // JSON serialization / deserialization helpers
    static std::string serializeOpenAIRequest(const std::vector<ChatMessage>& history, const std::string& model);
    static ChatbotResponse parseOpenAIResponse(const std::string& rawJson);

    // Deterministic mock generator
    static ChatbotResponse generateMockResponse(const std::string& prompt);

private:
    std::string httpPost(const std::string& url, const std::string& payload);

    std::string m_apiKey;
    std::string m_endpointUrl;
    std::string m_modelName{"gpt-4o-mini"};
    std::vector<ChatMessage> m_history;
};

} // namespace chatbot
