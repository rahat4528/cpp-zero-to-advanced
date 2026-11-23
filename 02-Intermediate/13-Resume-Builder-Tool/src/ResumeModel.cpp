#include "ResumeModel.hpp"
#include <sstream>

namespace resume {

bool Resume::isValid() const {
    return !personal.fullName.empty() && !personal.email.empty();
}

std::string ResumeTemplateEngine::renderMarkdown(const Resume& r) {
    std::ostringstream oss;
    oss << "# " << r.personal.fullName << "\n";
    oss << "**" << r.personal.title << "**\n\n";
    oss << "Email: " << r.personal.email << " | Phone: " << r.personal.phone
        << " | Location: " << r.personal.location << "\n";
    if (!r.personal.github.empty()) oss << "GitHub: [" << r.personal.github << "](" << r.personal.github << ") | ";
    if (!r.personal.linkedIn.empty()) oss << "LinkedIn: [" << r.personal.linkedIn << "](" << r.personal.linkedIn << ")";
    oss << "\n\n---\n\n";

    if (!r.personal.summary.empty()) {
        oss << "## Professional Summary\n" << r.personal.summary << "\n\n";
    }

    if (!r.skills.empty()) {
        oss << "## Core Competencies\n";
        for (size_t i = 0; i < r.skills.size(); ++i) {
            oss << "`" << r.skills[i] << "`" << (i + 1 < r.skills.size() ? ", " : "\n\n");
        }
    }

    if (!r.experiences.empty()) {
        oss << "## Work Experience\n";
        for (const auto& exp : r.experiences) {
            oss << "### " << exp.role << " — *" << exp.company << "*\n";
            oss << "**Dates:** " << exp.startDate << " – " << exp.endDate << "\n\n";
            for (const auto& h : exp.highlights) {
                oss << "- " << h << "\n";
            }
            oss << "\n";
        }
    }

    if (!r.projects.empty()) {
        oss << "## Notable Projects\n";
        for (const auto& proj : r.projects) {
            oss << "### " << proj.name << " (" << proj.technologies << ")\n";
            oss << proj.description << "\n";
            if (!proj.link.empty()) oss << "Link: " << proj.link << "\n";
            oss << "\n";
        }
    }

    if (!r.education.empty()) {
        oss << "## Education\n";
        for (const auto& edu : r.education) {
            oss << "- **" << edu.degree << " in " << edu.fieldOfStudy << "**, " << edu.institution
                << " (" << edu.graduationYear << ")";
            if (!edu.gpa.empty()) oss << " — GPA: " << edu.gpa;
            oss << "\n";
        }
    }

    return oss.str();
}

std::string ResumeTemplateEngine::renderHTML(const Resume& r) {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    oss << "<meta charset=\"UTF-8\">\n<title>" << r.personal.fullName << " - Resume</title>\n";
    oss << "<style>\n";
    oss << "body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif; line-height: 1.6; color: #2d3748; max-width: 800px; margin: 40px auto; padding: 0 20px; }\n";
    oss << "h1 { margin-bottom: 4px; color: #1a202c; }\n";
    oss << "h2 { border-bottom: 2px solid #e2e8f0; padding-bottom: 4px; color: #2b6cb0; margin-top: 24px; }\n";
    oss << ".subtitle { color: #4a5568; font-weight: 500; font-size: 1.1em; margin-bottom: 8px; }\n";
    oss << ".contact { font-size: 0.9em; color: #718096; margin-bottom: 20px; }\n";
    oss << ".badge { display: inline-block; background: #edf2f7; color: #2d3748; padding: 3px 8px; border-radius: 4px; margin: 2px; font-size: 0.85em; font-family: monospace; }\n";
    oss << "</style>\n</head>\n<body>\n";

    oss << "<h1>" << r.personal.fullName << "</h1>\n";
    oss << "<div class=\"subtitle\">" << r.personal.title << "</div>\n";
    oss << "<div class=\"contact\">Email: " << r.personal.email << " | Phone: " << r.personal.phone
        << " | Location: " << r.personal.location << "</div>\n";

    if (!r.personal.summary.empty()) {
        oss << "<h2>Summary</h2>\n<p>" << r.personal.summary << "</p>\n";
    }

    if (!r.skills.empty()) {
        oss << "<h2>Skills</h2>\n<div>\n";
        for (const auto& s : r.skills) {
            oss << "<span class=\"badge\">" << s << "</span>\n";
        }
        oss << "</div>\n";
    }

    if (!r.experiences.empty()) {
        oss << "<h2>Experience</h2>\n";
        for (const auto& exp : r.experiences) {
            oss << "<div>\n<h3>" << exp.role << " &mdash; <em>" << exp.company << "</em></h3>\n";
            oss << "<small>" << exp.startDate << " &ndash; " << exp.endDate << "</small>\n<ul>\n";
            for (const auto& h : exp.highlights) {
                oss << "<li>" << h << "</li>\n";
            }
            oss << "</ul>\n</div>\n";
        }
    }

    oss << "</body>\n</html>\n";
    return oss.str();
}

std::string ResumeTemplateEngine::renderPlainText(const Resume& r) {
    std::ostringstream oss;
    oss << "=================================================================\n";
    oss << " " << r.personal.fullName << " - " << r.personal.title << "\n";
    oss << " Email: " << r.personal.email << " | Phone: " << r.personal.phone << "\n";
    oss << "=================================================================\n\n";
    oss << "[SUMMARY]\n" << r.personal.summary << "\n\n";

    if (!r.skills.empty()) {
        oss << "[SKILLS]\n";
        for (const auto& s : r.skills) oss << " * " << s << "\n";
        oss << "\n";
    }

    if (!r.experiences.empty()) {
        oss << "[EXPERIENCE]\n";
        for (const auto& exp : r.experiences) {
            oss << " * " << exp.role << " @ " << exp.company << " (" << exp.startDate << " to " << exp.endDate << ")\n";
            for (const auto& h : exp.highlights) {
                oss << "    - " << h << "\n";
            }
        }
    }
    return oss.str();
}

Resume ResumeTemplateEngine::createSampleResume() {
    Resume r;
    r.personal.fullName = "Alex Mercer";
    r.personal.title = "Senior Systems Software Engineer";
    r.personal.email = "alex.mercer@systems.dev";
    r.personal.phone = "+1 (555) 234-5678";
    r.personal.location = "Seattle, WA";
    r.personal.github = "https://github.com/alexmercer";
    r.personal.linkedIn = "https://linkedin.com/in/alexmercer";
    r.personal.summary = "High-performance C++ engineer with 8+ years experience designing low-latency distributed engines and embedded firmware.";

    r.skills = {"C++17/20", "CMake", "Linux Sockets", "Multithreading", "SQLite", "Docker", "GDB/Valgrind"};

    Experience exp1;
    exp1.company = "Apex Systems";
    exp1.role = "Lead C++ Architect";
    exp1.startDate = "2021";
    exp1.endDate = "Present";
    exp1.highlights = {
        "Architected real-time order-matching engine delivering < 15 microsecond latency.",
        "Refactored legacy codebase into modern C++20 modules, reducing compilation times by 40%."
    };
    r.experiences.push_back(exp1);

    Education edu;
    edu.institution = "University of Washington";
    edu.degree = "B.S.";
    edu.fieldOfStudy = "Computer Engineering";
    edu.graduationYear = "2018";
    edu.gpa = "3.88";
    r.education.push_back(edu);

    return r;
}

} // namespace resume
