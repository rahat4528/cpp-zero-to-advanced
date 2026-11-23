#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "ChatProtocol.hpp"

TEST_CASE("ChatProtocol - Serialization and Deserialization") {
    using namespace chat_protocol;

    SUBCASE("JOIN Message") {
        ChatMessage msg{MessageType::JOIN, "Alice", "", ""};
        std::string raw = msg.serialize();
        CHECK(raw == "JOIN|Alice\n");

        auto parsed = ChatMessage::deserialize(raw);
        REQUIRE(parsed.has_value());
        CHECK(parsed->type == MessageType::JOIN);
        CHECK(parsed->sender == "Alice");
    }

    SUBCASE("Broadcast MSG Message") {
        ChatMessage msg{MessageType::MSG, "Bob", "", "Hello world! How are you?"};
        std::string raw = msg.serialize();
        CHECK(raw == "MSG|Bob|Hello world! How are you?\n");

        auto parsed = ChatMessage::deserialize(raw);
        REQUIRE(parsed.has_value());
        CHECK(parsed->type == MessageType::MSG);
        CHECK(parsed->sender == "Bob");
        CHECK(parsed->text == "Hello world! How are you?");
    }

    SUBCASE("Whisper Message") {
        ChatMessage msg{MessageType::WHISPER, "Alice", "Bob", "Psst, secret code is 42"};
        std::string raw = msg.serialize();
        CHECK(raw == "WHISPER|Alice|Bob|Psst, secret code is 42\n");

        auto parsed = ChatMessage::deserialize(raw);
        REQUIRE(parsed.has_value());
        CHECK(parsed->type == MessageType::WHISPER);
        CHECK(parsed->sender == "Alice");
        CHECK(parsed->recipient == "Bob");
        CHECK(parsed->text == "Psst, secret code is 42");
    }

    SUBCASE("Message containing pipe character") {
        ChatMessage msg{MessageType::MSG, "Dev", "", "Use a|b instead of c|d"};
        std::string raw = msg.serialize();
        auto parsed = ChatMessage::deserialize(raw);
        REQUIRE(parsed.has_value());
        CHECK(parsed->text == "Use a|b instead of c|d");
    }

    SUBCASE("Malformed Message Handling") {
        auto emptyParsed = ChatMessage::deserialize("");
        CHECK(emptyParsed.has_value() == false);

        auto invalidParsed = ChatMessage::deserialize("FOOBAR");
        CHECK(invalidParsed.has_value() == false);
    }
}
