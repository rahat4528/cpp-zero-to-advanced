#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ResumeModel.hpp"

TEST_CASE("ResumeModel - Validation Logic") {
    resume::Resume validResume;
    validResume.personal.fullName = "Jane Doe";
    validResume.personal.email = "jane@example.com";
    CHECK(validResume.isValid() == true);

    resume::Resume emptyResume;
    CHECK(emptyResume.isValid() == false);
}

TEST_CASE("ResumeTemplateEngine - Markdown and HTML Outputs") {
    auto r = resume::ResumeTemplateEngine::createSampleResume();
    REQUIRE(r.isValid() == true);

    SUBCASE("Markdown Rendering") {
        std::string md = resume::ResumeTemplateEngine::renderMarkdown(r);
        CHECK(md.find("# Alex Mercer") != std::string::npos);
        CHECK(md.find("## Core Competencies") != std::string::npos);
        CHECK(md.find("`C++17/20`") != std::string::npos);
        CHECK(md.find("Apex Systems") != std::string::npos);
    }

    SUBCASE("HTML Rendering") {
        std::string html = resume::ResumeTemplateEngine::renderHTML(r);
        CHECK(html.find("<!DOCTYPE html>") != std::string::npos);
        CHECK(html.find("<h1>Alex Mercer</h1>") != std::string::npos);
        CHECK(html.find("<span class=\"badge\">C++17/20</span>") != std::string::npos);
    }

    SUBCASE("PlainText Rendering") {
        std::string txt = resume::ResumeTemplateEngine::renderPlainText(r);
        CHECK(txt.find("Alex Mercer - Senior Systems Software Engineer") != std::string::npos);
        CHECK(txt.find("[SUMMARY]") != std::string::npos);
    }
}
