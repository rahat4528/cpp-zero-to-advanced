#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "VoiceCommandEngine.hpp"

TEST_CASE("VoiceCommandEngine - Exact and Fuzzy Utterance Matching") {
    voice::VoiceCommandEngine engine;
    CHECK(engine.getRegisteredCommandCount() >= 8);

    SUBCASE("Exact Grammar Match") {
        auto m = engine.processUtterance("volume up");
        CHECK(m.isRecognized == true);
        CHECK(m.action == voice::VoiceAction::VolumeUp);
        CHECK(m.confidence == doctest::Approx(1.0));
    }

    SUBCASE("Fuzzy Acoustic Match") {
        // "volum up" (1 missing letter, edit distance 1)
        auto m = engine.processUtterance("volum up");
        CHECK(m.isRecognized == true);
        CHECK(m.action == voice::VoiceAction::VolumeUp);
        CHECK(m.confidence >= 0.80);
    }

    SUBCASE("Prefix with Parameter Match") {
        auto m = engine.processUtterance("open application terminal");
        CHECK(m.isRecognized == true);
        CHECK(m.action == voice::VoiceAction::OpenApp);
        CHECK(m.parameter == "terminal");
    }

    SUBCASE("Unrecognized Noise") {
        auto m = engine.processUtterance("completely unrelated gibberish noise");
        CHECK(m.isRecognized == false);
        CHECK(m.action == voice::VoiceAction::Unknown);
    }
}

TEST_CASE("VoiceCommandEngine - Handler Dispatch") {
    voice::VoiceCommandEngine engine;
    bool handlerCalled = false;

    engine.setActionHandler(voice::VoiceAction::MuteAudio, [&](const voice::CommandMatch& m) {
        CHECK(m.action == voice::VoiceAction::MuteAudio);
        handlerCalled = true;
    });

    auto m = engine.processUtterance("mute");
    CHECK(engine.dispatch(m) == true);
    CHECK(handlerCalled == true);
}
