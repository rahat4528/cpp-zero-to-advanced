#include "MadLibsGenerator.h"

#include <iostream>
#include <map>
#include <string>

void printHelp() {
    std::cout << "Mad Libs Story Generator CLI\n"
              << "Usage:\n"
              << "  MadLibsGenerator                                Interactive prompt mode\n"
              << "  MadLibsGenerator --template <path>              Load custom template\n"
              << "  MadLibsGenerator --inputs \"key1=val1,key2=val2\" Automated substitution\n"
              << "  MadLibsGenerator --export <path>                Save final story\n"
              << "  MadLibsGenerator --help                         Show help\n"
              << "Example:\n"
              << "  MadLibsGenerator --inputs \"adjective=sunny,programmer_name=Ada,project_type=compiler,beverage=coffee,hardware_item=keyboard,bug_type=memory,line_number=42,debugging_technique=GDB,speed_adjective=blazing,best_practice=RAII\"\n";
}

int main(int argc, char* argv[]) {
    std::string templatePath = "resources/story.txt";
    std::string inputsStr;
    std::string exportPath;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--template" && i + 1 < argc) {
            templatePath = argv[++i];
        } else if (arg == "--inputs" && i + 1 < argc) {
            inputsStr = argv[++i];
        } else if (arg == "--export" && i + 1 < argc) {
            exportPath = argv[++i];
        }
    }

    mad_libs::MadLibsEngine engine;
    if (!engine.loadTemplateFromFile(templatePath)) {
        engine.setTemplate("One day, <name> decided to write a program in <language>. It was very <adjective>!");
    }

    if (!inputsStr.empty()) {
        auto replacements = mad_libs::MadLibsEngine::parseReplacementString(inputsStr);
        std::string story = engine.renderStory(replacements);
        std::cout << "\n====================================================\n"
                  << "                 YOUR COMPLETED STORY               \n"
                  << "====================================================\n"
                  << story << "\n"
                  << "====================================================\n";
        if (!exportPath.empty()) {
            engine.exportStory(exportPath, story);
            std::cout << "Story exported to " << exportPath << "\n";
        }
        return 0;
    }

    auto placeholders = engine.extractPlaceholders();
    std::cout << "=== Mad Libs Generator ===\n";
    std::cout << "Please provide words for the " << placeholders.size() << " story prompts:\n\n";

    std::map<std::string, std::string> userAnswers;
    for (const auto& tag : placeholders) {
        if (userAnswers.find(tag) == userAnswers.end()) {
            std::cout << "Enter a/an [" << tag << "]: ";
            std::string answer;
            std::getline(std::cin, answer);
            userAnswers[tag] = answer;
        }
    }

    std::string finalStory = engine.renderStory(userAnswers);
    std::cout << "\n====================================================\n"
              << "                 YOUR COMPLETED STORY               \n"
              << "====================================================\n"
              << finalStory << "\n"
              << "====================================================\n";

    if (!exportPath.empty()) {
        engine.exportStory(exportPath, finalStory);
        std::cout << "Story saved to: " << exportPath << "\n";
    }

    return 0;
}
