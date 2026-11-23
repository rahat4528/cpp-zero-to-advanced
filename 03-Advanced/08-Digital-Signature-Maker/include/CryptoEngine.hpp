#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace crypto {

struct KeyPair {
    std::string privateKeyPem;
    std::string publicKeyPem;
};

class CryptoEngine {
public:
    [[nodiscard]] static KeyPair generateRSAKeyPair(int bits = 2048);
    [[nodiscard]] static std::string signMessage(const std::string& message, const std::string& privateKeyPem);
    [[nodiscard]] static bool verifySignature(const std::string& message, const std::string& base64Signature, const std::string& publicKeyPem);

    [[nodiscard]] static std::string base64Encode(const std::vector<uint8_t>& data);
    [[nodiscard]] static std::vector<uint8_t> base64Decode(const std::string& encoded);
};

} // namespace crypto
