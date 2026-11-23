#include "MadLibsGenerator.h"

#include <fstream>
#include <sstream>

namespace mad_libs {

void MadLibsEngine::setTemplate(const std::string& templateText) {
    m_templateText = templateText;
}

std::vector<std::string> MadLibsEngine::extractPlaceholders() const {
    std::vector<std::string> placeholders;
    size_t pos = 0;

    while (pos < m_templateText.size()) {
        size_t openBracket = m_templateText.find('<', pos);
        if (openBracket == std::string::npos) break;

        size_t closeBracket = m_templateText.find('>', openBracket);
        if (closeBracket == std::string::npos) break;

        std::string tag = m_templateText.substr(openBracket + 1, closeBracket - openBracket - 1);
        if (!tag.empty()) {
            placeholders.push_back(tag);
        }
        pos = closeBracket + 1;
    }
    return placeholders;
}

std::string MadLibsEngine::renderStory(const std::map<std::string, std::string>& replacements) const {
    std::string result;
    size_t pos = 0;

    while (pos < m_templateText.size()) {
        size_t openBracket = m_templateText.find('<', pos);
        if (openBracket == std::string::npos) {
            result += m_templateText.substr(pos);
            break;
        }

        result += m_templateText.substr(pos, openBracket - pos);
        size_t closeBracket = m_templateText.find('>', openBracket);
        if (closeBracket == std::string::npos) {
            result += m_templateText.substr(openBracket);
            break;
        }

        std::string tag = m_templateText.substr(openBracket + 1, closeBracket - openBracket - 1);
        auto it = replacements.find(tag);
        if (it != replacements.end()) {
            result += it->second;
        } else {
            // Keep placeholder unreplaced if missing
            result += "<" + tag + ">";
        }
        pos = closeBracket + 1;
    }

    return result;
}

std::map<std::string, std::string> MadLibsEngine::parseReplacementString(const std::string& input) {
    std::map<std::string, std::string> replacements;
    std::istringstream iss(input);
    std::string pair;

    while (std::getline(iss, pair, ',')) {
        size_t eqPos = pair.find('=');
        if (eqPos != std::string::npos && eqPos > 0 && eqPos + 1 < pair.size()) {
            std::string key = pair.substr(0, eqPos);
            std::string val = pair.substr(eqPos + 1);
            replacements[key] = val;
        }
    }
    return replacements;
}

bool MadLibsEngine::loadTemplateFromFile(const std::string& filepath) {
    std::ifstream in(filepath);
    if (!in.is_open()) return false;

    std::ostringstream ss;
    ss << in.rdbuf();
    m_templateText = ss.str();
    return !m_templateText.empty();
}

bool MadLibsEngine::exportStory(const std::string& filepath, const std::string& renderedStory) const {
    std::ofstream out(filepath);
    if (!out.is_open()) return false;
    out << renderedStory;
    return true;
}

} // namespace mad_libs
