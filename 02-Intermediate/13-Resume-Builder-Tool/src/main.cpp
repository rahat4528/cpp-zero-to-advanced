#include "ResumeModel.hpp"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "               RESUME BUILDER & TEMPLATE COMPILER                \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Generating sample resume and rendering formats...\n";

        auto sample = resume::ResumeTemplateEngine::createSampleResume();
        std::string md = resume::ResumeTemplateEngine::renderMarkdown(sample);
        std::string html = resume::ResumeTemplateEngine::renderHTML(sample);
        std::string txt = resume::ResumeTemplateEngine::renderPlainText(sample);

        std::ofstream mdFile("sample_resume.md");
        mdFile << md;
        std::ofstream htmlFile("sample_resume.html");
        htmlFile << html;

        std::cout << "Successfully generated:\n";
        std::cout << "  - Markdown output (" << md.length() << " bytes) -> sample_resume.md\n";
        std::cout << "  - HTML5 output    (" << html.length() << " bytes) -> sample_resume.html\n";
        std::cout << "  - PlainText preview:\n\n";
        std::cout << txt << "\n";
        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "               RESUME BUILDER & TEMPLATE COMPILER                \n";
    std::cout << "=================================================================\n";
    std::cout << "Usage: ResumeBuilderTool --test-run\n";
    return 0;
}
