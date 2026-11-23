#pragma once

#include <string>
#include <vector>

namespace resume {

struct PersonalInfo {
    std::string fullName;
    std::string title;
    std::string email;
    std::string phone;
    std::string location;
    std::string linkedIn;
    std::string github;
    std::string summary;
};

struct Education {
    std::string institution;
    std::string degree;
    std::string fieldOfStudy;
    std::string graduationYear;
    std::string gpa;
};

struct Experience {
    std::string company;
    std::string role;
    std::string startDate;
    std::string endDate;
    std::vector<std::string> highlights;
};

struct ProjectItem {
    std::string name;
    std::string technologies;
    std::string description;
    std::string link;
};

struct Resume {
    PersonalInfo personal;
    std::vector<Education> education;
    std::vector<Experience> experiences;
    std::vector<std::string> skills;
    std::vector<ProjectItem> projects;

    [[nodiscard]] bool isValid() const;
};

class ResumeTemplateEngine {
public:
    static std::string renderMarkdown(const Resume& resume);
    static std::string renderHTML(const Resume& resume);
    static std::string renderPlainText(const Resume& resume);

    static Resume createSampleResume();
};

} // namespace resume
