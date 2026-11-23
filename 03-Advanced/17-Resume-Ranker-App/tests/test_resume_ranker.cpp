#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "TextTokenizer.hpp"
#include "TFIDFModel.hpp"

using namespace resume_ranker;

TEST_SUITE("Resume Ranker and TF-IDF Text Processing") {
    TEST_CASE("Text Tokenization and Stopword Elimination") {
        std::string raw = "The engineer is designing and developing robust C++ systems in production.";
        auto tokens = TextTokenizer::tokenize(raw);

        // Stop words ("the", "is", "and", "in") must be removed
        for (const auto& t : tokens) {
            CHECK_FALSE(TextTokenizer::isStopWord(t));
        }

        // Stemming checks: "developing" -> "develop", "systems" -> "system"
        CHECK(std::find(tokens.begin(), tokens.end(), "develop") != tokens.end());
        CHECK(std::find(tokens.begin(), tokens.end(), "system") != tokens.end());
        CHECK(std::find(tokens.begin(), tokens.end(), "c++") != tokens.end());
    }

    TEST_CASE("TF-IDF Cosine Similarity Ranking Discrimination") {
        TFIDFModel model;

        CandidateResume cppDev;
        cppDev.id = "1";
        cppDev.candidateName = "C++ Expert";
        cppDev.rawContent = "C++ engineer specializing in high-performance multithreading, concurrency, and algorithms.";
        model.addResume(cppDev);

        CandidateResume webDev;
        webDev.id = "2";
        webDev.candidateName = "Web Dev";
        webDev.rawContent = "Frontend web developer working with CSS, HTML, TypeScript, and user experience design.";
        model.addResume(webDev);

        std::string query = "Looking for a C++ software engineer with multithreading experience.";
        auto ranked = model.rankResumes(query);

        REQUIRE(ranked.size() == 2);
        CHECK(ranked[0].candidateName == "C++ Expert");
        CHECK(ranked[0].score > ranked[1].score);
        CHECK(ranked[0].score > 0.30);
        CHECK(ranked[1].score < 0.20);
    }
}
