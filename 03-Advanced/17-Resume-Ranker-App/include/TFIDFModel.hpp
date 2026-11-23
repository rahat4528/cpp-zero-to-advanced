#pragma once

#include "TextTokenizer.hpp"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

namespace resume_ranker {

struct CandidateResume {
    std::string id;
    std::string candidateName;
    std::string rawContent;
    std::vector<std::string> tokens;
};

struct RankedCandidate {
    std::string candidateName;
    double score{0.0};
    std::vector<std::string> topMatchingKeywords;
};

class TFIDFModel {
public:
    TFIDFModel() = default;

    void addResume(const CandidateResume& resume);
    void clear();
    [[nodiscard]] size_t getResumeCount() const noexcept;

    [[nodiscard]] std::vector<RankedCandidate> rankResumes(const std::string& jobDescription) const;

private:
    std::vector<CandidateResume> m_resumes;
};

} // namespace resume_ranker
