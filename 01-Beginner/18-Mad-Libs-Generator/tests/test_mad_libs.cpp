#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "MadLibsGenerator.h"

#include <cstdio>

using namespace mad_libs;

TEST_CASE("Placeholder extraction") {
    MadLibsEngine engine;
    engine.setTemplate("The <color> <animal> jumped over the <obstacle>.");

    auto tags = engine.extractPlaceholders();
    REQUIRE(tags.size() == 3);
    CHECK(tags[0] == "color");
    CHECK(tags[1] == "animal");
    CHECK(tags[2] == "obstacle");
}

TEST_CASE("Story rendering with full substitutions") {
    MadLibsEngine engine;
    engine.setTemplate("Hello <name>, welcome to <city>!");

    std::map<std::string, std::string> rep = {
        {"name", "Bjarne"},
        {"city", "Copenhagen"}
    };

    std::string rendered = engine.renderStory(rep);
    CHECK(rendered == "Hello Bjarne, welcome to Copenhagen!");
}

TEST_CASE("Partial substitution preserves unfilled placeholders") {
    MadLibsEngine engine;
    engine.setTemplate("<greeting> world, my name is <name>.");

    std::map<std::string, std::string> rep = {
        {"name", "Alice"}
    };

    std::string rendered = engine.renderStory(rep);
    CHECK(rendered == "<greeting> world, my name is Alice.");
}

TEST_CASE("Replacement string parsing") {
    std::string input = "noun=dragon,verb=flew,adjective=giant";
    auto rep = MadLibsEngine::parseReplacementString(input);

    CHECK(rep.size() == 3);
    CHECK(rep["noun"] == "dragon");
    CHECK(rep["verb"] == "flew");
    CHECK(rep["adjective"] == "giant");
}

TEST_CASE("File export") {
    const std::string path = "temp_madlib_test.txt";
    MadLibsEngine engine;
    CHECK(engine.exportStory(path, "Rendered story content") == true);

    std::remove(path.c_str());
}
