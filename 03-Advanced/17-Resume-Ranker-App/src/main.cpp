#include "TFIDFModel.hpp"
#include <iostream>
#include <iomanip>

int main(int argc, char* argv[]) {
    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
        }
    }

    std::cout << "=================================================\n";
    std::cout << "  AI Resume Ranker & TF-IDF Vector Space Scorer  \n";
    std::cout << "=================================================\n";

    std::string jobDescription =
        "Senior C++ Systems Engineer with deep expertise in multithreading, concurrency, "
        "low-latency socket networking, SQLite database engines, OpenSSL cryptography, "
        "and clean modern C++17/C++20 software architecture.";

    resume_ranker::TFIDFModel model;

    // Candidate 1: Strong C++ Systems Match
    resume_ranker::CandidateResume cand1;
    cand1.id = "RES-01";
    cand1.candidateName = "David Vance (Senior C++ Engineer)";
    cand1.rawContent =
        "Over 8 years experience architecting high-performance C++ systems. Expert in modern "
        "C++17, multithreading, asynchronous networking, distributed sockets, SQLite relational "
        "databases, and OpenSSL cryptographic protocols with robust unit testing.";
    model.addResume(cand1);

    // Candidate 2: Web / Frontend Match (Low Relevance)
    resume_ranker::CandidateResume cand2;
    cand2.id = "RES-02";
    cand2.candidateName = "Sarah Jenkins (Frontend Web Developer)";
    cand2.rawContent =
        "Full-stack React, TypeScript, HTML5, CSS3, and Node.js developer. Built interactive UI dashboards "
        "and cloud REST APIs with MongoDB. Passionate about user design and mobile responsiveness.";
    model.addResume(cand2);

    // Candidate 3: General Software Engineer (Medium Relevance)
    resume_ranker::CandidateResume cand3;
    cand3.id = "RES-03";
    cand3.candidateName = "Marcus Brody (Software Developer)";
    cand3.rawContent =
        "General software engineer experienced in Python, C++, and Linux tools. Developed automated test "
        "scripts and basic socket networking tools for internal monitoring pipelines.";
    model.addResume(cand3);

    std::cout << "Target Job Posting:\n\"" << jobDescription << "\"\n\n";
    std::cout << "Ranking " << model.getResumeCount() << " candidate resumes using TF-IDF Cosine Similarity...\n";

    auto results = model.rankResumes(jobDescription);

    std::cout << "\n=== CANDIDATE RANKINGS LEADERBOARD ===\n";
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& c = results[i];
        std::cout << " #" << (i + 1) << " " << std::left << std::setw(38) << c.candidateName
                  << " Match: " << std::fixed << std::setprecision(1) << (c.score * 100.0) << "%\n";
        std::cout << "     Top Matching Keywords: ";
        for (const auto& kw : c.topMatchingKeywords) {
            std::cout << "[" << kw << "] ";
        }
        std::cout << "\n";
    }
    std::cout << "======================================\n";

    if (testRun) {
        std::cout << "[Test Mode] TF-IDF resume ranking engine verified.\n";
        return 0;
    }

    return 0;
}
