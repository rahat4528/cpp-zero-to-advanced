#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "AESGCMCipher.hpp"

using namespace crypto;

TEST_SUITE("AES-256-GCM Authenticated Encryption Engine") {
    TEST_CASE("Container Serialization and Deserialization") {
        EncryptedPayload p;
        p.salt.assign(16, 0x11);
        p.iv.assign(12, 0x22);
        p.tag.assign(16, 0x33);
        p.ciphertext = {0xAA, 0xBB, 0xCC, 0xDD};

        auto raw = p.serialize();
        CHECK(raw.size() == 4 + 16 + 12 + 16 + 4);

        auto restored = EncryptedPayload::deserialize(raw);
        REQUIRE(restored.has_value());
        CHECK(restored->salt == p.salt);
        CHECK(restored->iv == p.iv);
        CHECK(restored->tag == p.tag);
        CHECK(restored->ciphertext == p.ciphertext);
    }

    TEST_CASE("Encryption and Decryption Roundtrip") {
        std::string original = "The quick brown fox jumps over the lazy dog";
        std::vector<uint8_t> plain(original.begin(), original.end());
        std::string password = "StrongMasterPassword#456";

        auto container = AESGCMCipher::encryptBytes(plain, password);
        REQUIRE(container.has_value());
        CHECK(container->size() > plain.size());

        auto decrypted = AESGCMCipher::decryptBytes(*container, password);
        REQUIRE(decrypted.has_value());
        CHECK(*decrypted == plain);
    }

    TEST_CASE("AEAD Authentication Tag Rejection of Tampered Ciphertext") {
        std::string msg = "Mission Briefing Alpha";
        std::vector<uint8_t> plain(msg.begin(), msg.end());
        std::string password = "AuthTagSecretKey";

        auto container = AESGCMCipher::encryptBytes(plain, password);
        REQUIRE(container.has_value());

        // Modify 1 byte in container (ciphertext section)
        auto tampered = *container;
        tampered.back() ^= 0xFF;

        auto result = AESGCMCipher::decryptBytes(tampered, password);
        CHECK_FALSE(result.has_value()); // MUST fail authentication!
    }

    TEST_CASE("Wrong Password Rejection") {
        std::string msg = "Confidential Banking Record";
        std::vector<uint8_t> plain(msg.begin(), msg.end());

        auto container = AESGCMCipher::encryptBytes(plain, "CorrectKey");
        REQUIRE(container.has_value());

        auto failed = AESGCMCipher::decryptBytes(*container, "WrongKey");
        CHECK_FALSE(failed.has_value());
    }
}
