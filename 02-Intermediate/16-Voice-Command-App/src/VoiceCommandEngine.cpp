#include "VoiceCommandEngine.hpp"
#include <algorithm>
#include <sstream>
#include <cmath>

namespace voice {

VoiceCommandEngine::VoiceCommandEngine() {
    // Populate default grammar rules
    registerCommand("system status", VoiceAction::SystemStatus, "Display OS telemetry and resource metrics");
    registerCommand("volume up", VoiceAction::VolumeUp, "Increase master audio gain by 10%");
    registerCommand("volume down", VoiceAction::VolumeDown, "Decrease master audio gain by 10%");
    registerCommand("mute", VoiceAction::MuteAudio, "Toggle master mute state");
    registerCommand("play music", VoiceAction::PlayMusic, "Resume background playback");
    registerCommand("stop", VoiceAction::StopMusic, "Halt background playback");
    registerCommand("help", VoiceAction::Help, "List available voice commands");
    registerCommand("open application", VoiceAction::OpenApp, "Launch requested application");
}

void VoiceCommandEngine::registerCommand(const std::string& trigger, VoiceAction action, const std::string& intent) {
    m_grammar.push_back({toLower(trigger), action, intent});
}

void VoiceCommandEngine::setActionHandler(VoiceAction action, ActionHandler handler) {
    m_handlers[action] = std::move(handler);
}

size_t VoiceCommandEngine::getRegisteredCommandCount() const {
    return m_grammar.size();
}

std::string VoiceCommandEngine::toLower(const std::string& s) {
    std::string out = s;
    std::transform(out.begin(), out.end(), out.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return out;
}

int VoiceCommandEngine::levenshteinDistance(const std::string& s1, const std::string& s2) {
    const size_t len1 = s1.size();
    const size_t len2 = s2.size();
    std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));

    for (size_t i = 0; i <= len1; ++i) d[i][0] = static_cast<int>(i);
    for (size_t j = 0; j <= len2; ++j) d[0][j] = static_cast<int>(j);

    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            d[i][j] = std::min({
                d[i - 1][j] + 1,     // deletion
                d[i][j - 1] + 1,     // insertion
                d[i - 1][j - 1] + cost // substitution
            });
        }
    }
    return d[len1][len2];
}

CommandMatch VoiceCommandEngine::processUtterance(const std::string& spokenText) const {
    std::string clean = toLower(spokenText);
    // Trim leading/trailing whitespace
    while (!clean.empty() && (clean.front() == ' ' || clean.front() == '\t')) clean.erase(clean.begin());
    while (!clean.empty() && (clean.back() == ' ' || clean.back() == '\t')) clean.pop_back();

    if (clean.empty()) {
        return {VoiceAction::Unknown, "Empty utterance", 0.0, "", false};
    }

    const RegisteredCommand* bestCmd = nullptr;
    double bestConfidence = 0.0;
    std::string bestParam;

    for (const auto& cmd : m_grammar) {
        // Exact substring prefix match
        if (clean.rfind(cmd.trigger, 0) == 0) {
            bestCmd = &cmd;
            bestConfidence = 1.0;
            if (clean.length() > cmd.trigger.length()) {
                bestParam = clean.substr(cmd.trigger.length() + 1);
            }
            break;
        }

        // Fuzzy edit distance match
        int dist = levenshteinDistance(clean, cmd.trigger);
        int maxLen = static_cast<int>(std::max(clean.length(), cmd.trigger.length()));
        double conf = 1.0 - (static_cast<double>(dist) / static_cast<double>(maxLen));

        if (conf > bestConfidence && conf >= 0.70) {
            bestConfidence = conf;
            bestCmd = &cmd;
        }
    }

    if (bestCmd && bestConfidence >= 0.70) {
        return {bestCmd->action, bestCmd->intent, bestConfidence, bestParam, true};
    }

    return {VoiceAction::Unknown, "No matching grammar rule found", bestConfidence, "", false};
}

bool VoiceCommandEngine::dispatch(const CommandMatch& match) const {
    if (!match.isRecognized) return false;

    auto it = m_handlers.find(match.action);
    if (it != m_handlers.end() && it->second) {
        it->second(match);
        return true;
    }
    return false;
}

} // namespace voice
