#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <optional>

namespace crypto {

struct EncryptedPayload {
    std::vector<uint8_t> salt;
    std::vector<uint8_t> iv;
    std::vector<uint8_t> tag;
    std::vector<uint8_t> ciphertext;

    [[nodiscard]] std::vector<uint8_t> serialize() const;
    [[nodiscard]] static std::optional<EncryptedPayload> deserialize(const std::vector<uint8_t>& raw);
};

class AESGCMCipher {
public:
    static constexpr size_t SALT_LEN = 16;
    static constexpr size_t IV_LEN = 12;
    static constexpr size_t TAG_LEN = 16;
    static constexpr size_t KEY_LEN = 32; // 256 bits
    static constexpr int PBKDF2_ITERATIONS = 100000;

    [[nodiscard]] static std::vector<uint8_t> deriveKey(const std::string& passphrase, const std::vector<uint8_t>& salt);

    [[nodiscard]] static std::optional<EncryptedPayload> encrypt(const std::vector<uint8_t>& plaintext, const std::string& passphrase);
    [[nodiscard]] static std::optional<std::vector<uint8_t>> decrypt(const EncryptedPayload& payload, const std::string& passphrase);

    [[nodiscard]] static std::optional<std::vector<uint8_t>> encryptBytes(const std::vector<uint8_t>& plaintext, const std::string& passphrase);
    [[nodiscard]] static std::optional<std::vector<uint8_t>> decryptBytes(const std::vector<uint8_t>& rawContainer, const std::string& passphrase);
};

} // namespace crypto
