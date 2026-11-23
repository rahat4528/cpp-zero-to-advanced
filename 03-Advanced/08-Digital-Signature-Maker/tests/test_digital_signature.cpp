#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "CryptoEngine.hpp"

using namespace crypto;

TEST_SUITE("OpenSSL Digital Signature Engine") {
    TEST_CASE("Base64 Encoding and Decoding Roundtrip") {
        std::vector<uint8_t> raw = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'};
        std::string encoded = CryptoEngine::base64Encode(raw);
        CHECK_FALSE(encoded.empty());

        auto decoded = CryptoEngine::base64Decode(encoded);
        REQUIRE(decoded.size() == raw.size());
        CHECK(decoded == raw);
    }

    TEST_CASE("RSA Keypair Generation and PEM Export") {
        auto keyPair = CryptoEngine::generateRSAKeyPair(2048);
        CHECK(keyPair.privateKeyPem.find("-----BEGIN PRIVATE KEY-----") != std::string::npos);
        CHECK(keyPair.publicKeyPem.find("-----BEGIN PUBLIC KEY-----") != std::string::npos);
    }

    TEST_CASE("Message Signing and Verification") {
        auto keyPair = CryptoEngine::generateRSAKeyPair(2048);
        std::string doc = "CONFIDENTIAL: System architecture specification v2.0";

        std::string sig = CryptoEngine::signMessage(doc, keyPair.privateKeyPem);
        CHECK_FALSE(sig.empty());

        // Valid signature verifies true
        CHECK(CryptoEngine::verifySignature(doc, sig, keyPair.publicKeyPem));

        // Tampered message fails verification
        CHECK_FALSE(CryptoEngine::verifySignature(doc + "!", sig, keyPair.publicKeyPem));

        // Signature on wrong key fails
        auto otherKeyPair = CryptoEngine::generateRSAKeyPair(2048);
        CHECK_FALSE(CryptoEngine::verifySignature(doc, sig, otherKeyPair.publicKeyPem));
    }
}
