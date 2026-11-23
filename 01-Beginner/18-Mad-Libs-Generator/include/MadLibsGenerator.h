#pragma once

#include <map>
#include <string>
#include <vector>

namespace mad_libs {

class MadLibsEngine {
public:
    MadLibsEngine() = default;

    // Sets raw template text (e.g. "One day, <name> decided to code in <language>.")
    void setTemplate(const std::string& templateText);
    const std::string& getTemplate() const { return m_templateText; }

    // Extracts ordered list of all placeholders (e.g. ["name", "language"])
    std::vector<std::string> extractPlaceholders() const;

    // Renders the story by substituting provided replacements map
    std::string renderStory(const std::map<std::string, std::string>& replacements) const;

    // File loading and saving
    bool loadTemplateFromFile(const std::string& filepath);
    bool exportStory(const std::string& filepath, const std::string& renderedStory) const;

    // Parses command-line inputs: "noun=dragon,verb=flew,adjective=fearsome"
    static std::map<std::string, std::string> parseReplacementString(const std::string& input);

private:
    std::string m_templateText;
};

} // namespace mad_libs
