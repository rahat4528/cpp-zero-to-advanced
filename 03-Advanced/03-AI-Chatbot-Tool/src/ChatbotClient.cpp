#include "ChatbotClient.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstdlib>

#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <wininet.h>
    #if defined(_MSC_VER)
        #pragma comment(lib, "wininet.lib")
    #endif
#endif

namespace chatbot {

static std::string roleToString(MessageRole role) {
    switch (role) {
        case MessageRole::System: return "system";
        case MessageRole::User: return "user";
        case MessageRole::Assistant: return "assistant";
    }
    return "user";
}

ChatbotClient::ChatbotClient(std::string apiKey, std::string endpointUrl)
    : m_apiKey(std::move(apiKey)), m_endpointUrl(std::move(endpointUrl)) {
    if (m_apiKey.empty()) {
        const char* k = std::getenv("LLM_API_KEY");
        if (k) m_apiKey = k;
    }
    if (m_endpointUrl.empty()) {
        const char* url = std::getenv("LLM_ENDPOINT_URL");
        m_endpointUrl = url ? url : "https://api.openai.com/v1/chat/completions";
    }
    setSystemPrompt("You are an expert C++ programming assistant. Respond with concise, accurate code insights.");
}

void ChatbotClient::setSystemPrompt(const std::string& systemPrompt) {
    if (!m_history.empty() && m_history[0].role == MessageRole::System) {
        m_history[0].content = systemPrompt;
    } else {
        m_history.insert(m_history.begin(), {MessageRole::System, systemPrompt});
    }
}

void ChatbotClient::addUserMessage(const std::string& userText) {
    m_history.push_back({MessageRole::User, userText});
}

void ChatbotClient::addAssistantMessage(const std::string& assistantText) {
    m_history.push_back({MessageRole::Assistant, assistantText});
}

void ChatbotClient::clearHistory() {
    std::string sys;
    if (!m_history.empty() && m_history[0].role == MessageRole::System) {
        sys = m_history[0].content;
    }
    m_history.clear();
    if (!sys.empty()) setSystemPrompt(sys);
}

const std::vector<ChatMessage>& ChatbotClient::getHistory() const {
    return m_history;
}

std::string ChatbotClient::httpPost(const std::string& url, const std::string& payload) {
#if defined(_WIN32) || defined(_WIN64)
    HINTERNET hInternet = InternetOpenA("AIChatbotTool/1.0", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hInternet) return "";

    std::string headers = "Content-Type: application/json\r\nAuthorization: Bearer " + m_apiKey + "\r\n";

    URL_COMPONENTSA urlComp{};
    urlComp.dwStructSize = sizeof(urlComp);
    char host[256] = {0};
    char path[1024] = {0};
    urlComp.lpszHostName = host;
    urlComp.dwHostNameLength = sizeof(host);
    urlComp.lpszUrlPath = path;
    urlComp.dwUrlPathLength = sizeof(path);

    if (!InternetCrackUrlA(url.c_str(), 0, 0, &urlComp)) {
        InternetCloseHandle(hInternet);
        return "";
    }

    INTERNET_PORT port = urlComp.nPort ? urlComp.nPort : INTERNET_DEFAULT_HTTPS_PORT;
    HINTERNET hConnect = InternetConnectA(hInternet, host, port, nullptr, nullptr, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return "";
    }

    DWORD flags = INTERNET_FLAG_SECURE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE;
    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", path, nullptr, nullptr, nullptr, flags, 0);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return "";
    }

    BOOL sent = HttpSendRequestA(hRequest, headers.c_str(), static_cast<DWORD>(headers.length()),
                                 const_cast<char*>(payload.c_str()), static_cast<DWORD>(payload.length()));
    if (!sent) {
        InternetCloseHandle(hRequest);
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return "";
    }

    std::string response;
    char buffer[4096];
    DWORD bytesRead = 0;
    while (InternetReadFile(hRequest, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        response.append(buffer, bytesRead);
    }

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return response;
#else
    (void)url;
    (void)payload;
    return "";
#endif
}

std::string ChatbotClient::serializeOpenAIRequest(const std::vector<ChatMessage>& history, const std::string& model) {
    nlohmann::json j;
    j["model"] = model;
    j["messages"] = nlohmann::json::array();

    for (const auto& msg : history) {
        j["messages"].push_back({
            {"role", roleToString(msg.role)},
            {"content", msg.content}
        });
    }
    j["temperature"] = 0.7;
    return j.dump();
}

ChatbotResponse ChatbotClient::parseOpenAIResponse(const std::string& rawJson) {
    ChatbotResponse res;
    try {
        auto j = nlohmann::json::parse(rawJson);
        if (j.contains("error")) {
            res.isSuccess = false;
            res.errorMessage = j["error"].value("message", "Unknown API error");
            return res;
        }

        if (j.contains("choices") && j["choices"].is_array() && !j["choices"].empty()) {
            res.replyText = j["choices"][0]["message"].value("content", "");
        }

        if (j.contains("usage")) {
            res.promptTokens = j["usage"].value("prompt_tokens", 0);
            res.completionTokens = j["usage"].value("completion_tokens", 0);
        }

        res.isMock = false;
        res.isSuccess = true;
        return res;
    } catch (const std::exception& e) {
        res.isSuccess = false;
        res.errorMessage = e.what();
        return res;
    }
}

ChatbotResponse ChatbotClient::generateMockResponse(const std::string& prompt) {
    ChatbotResponse res;
    res.isMock = true;
    res.isSuccess = true;
    res.promptTokens = static_cast<int>(prompt.length() / 4);
    res.completionTokens = 42;

    if (prompt.find("RAII") != std::string::npos) {
        res.replyText = "RAII (Resource Acquisition Is Initialization) ties resource lifespan to object lifetime. Destructors guarantee cleanup.";
    } else if (prompt.find("hello") != std::string::npos || prompt.find("hi") != std::string::npos) {
        res.replyText = "Hello! I am your C++ LLM assistant. How can I help you design your systems today?";
    } else {
        res.replyText = "In C++, prefer smart pointers, strong typing, const correctness, and RAII over raw pointer management. (Mock LLM Output)";
    }
    return res;
}

ChatbotResponse ChatbotClient::sendMessage(const std::string& userText) {
    addUserMessage(userText);

    if (m_apiKey.empty() || m_apiKey == "your_api_key_here") {
        auto mock = generateMockResponse(userText);
        addAssistantMessage(mock.replyText);
        return mock;
    }

    std::string payload = serializeOpenAIRequest(m_history, m_modelName);
    std::string responseBody = httpPost(m_endpointUrl, payload);

    if (responseBody.empty()) {
        auto mock = generateMockResponse(userText);
        mock.replyText += " [Network fallback to Mock]";
        addAssistantMessage(mock.replyText);
        return mock;
    }

    auto parsed = parseOpenAIResponse(responseBody);
    if (parsed.isSuccess) {
        addAssistantMessage(parsed.replyText);
    }
    return parsed;
}

} // namespace chatbot
