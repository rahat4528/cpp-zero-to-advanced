#pragma once

#include <string>
#include <vector>
#include <functional>
#include <map>

namespace voice {

enum class VoiceAction {
    SystemStatus,
    VolumeUp,
    VolumeDown,
    MuteAudio,
    PlayMusic,
    StopMusic,
    OpenApp,
    Help,
    Unknown
};

struct CommandMatch {
    VoiceAction action{VoiceAction::Unknown};
    std::string intent;
    double confidence{0.0};
    std::string parameter;
    bool isRecognized{false};
};

class VoiceCommandEngine {
public:
    using ActionHandler = std::function<void(const CommandMatch&)>;

    VoiceCommandEngine();

    void registerCommand(const std::string& trigger, VoiceAction action, const std::string& intent);
    void setActionHandler(VoiceAction action, ActionHandler handler);

    // Parses offline transcript (or audio recognizer output) with fuzzy acoustic tolerance
    [[nodiscard]] CommandMatch processUtterance(const std::string& spokenText) const;

    // Dispatches recognized command to registered action handler
    bool dispatch(const CommandMatch& match) const;

    [[nodiscard]] size_t getRegisteredCommandCount() const;

private:
    static int levenshteinDistance(const std::string& s1, const std::string& s2);
    static std::string toLower(const std::string& s);

    struct RegisteredCommand {
        std::string trigger;
        VoiceAction action;
        std::string intent;
    };

    std::vector<RegisteredCommand> m_grammar;
    std::map<VoiceAction, ActionHandler> m_handlers;
};

} // namespace voice
