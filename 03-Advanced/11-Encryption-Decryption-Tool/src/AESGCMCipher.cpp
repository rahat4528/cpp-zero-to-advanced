#include "AESGCMCipher.hpp"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <cstring>

namespace crypto {

static const uint8_t MAGIC[4] = {'A', '2', '5', '6'};

std::vector<uint8_t> EncryptedPayload::serialize() const {
    std::vector<uint8_t> out;
    out.reserve(4 + salt.size() + iv.size() + tag.size() + ciphertext.size());
    out.insert(out.end(), MAGIC, MAGIC + 4);
    out.insert(out.end(), salt.begin(), salt.end());
    out.insert(out.end(), iv.begin(), iv.end());
    out.insert(out.end(), tag.begin(), tag.end());
    out.insert(out.end(), ciphertext.begin(), ciphertext.end());
    return out;
}

std::optional<EncryptedPayload> EncryptedPayload::deserialize(const std::vector<uint8_t>& raw) {
    constexpr size_t HEADER_LEN = 4 + AESGCMCipher::SALT_LEN + AESGCMCipher::IV_LEN + AESGCMCipher::TAG_LEN;
    if (raw.size() < HEADER_LEN) return std::nullopt;

    if (std::memcmp(raw.data(), MAGIC, 4) != 0) return std::nullopt;

    EncryptedPayload p;
    size_t offset = 4;

    p.salt.assign(raw.begin() + offset, raw.begin() + offset + AESGCMCipher::SALT_LEN);
    offset += AESGCMCipher::SALT_LEN;

    p.iv.assign(raw.begin() + offset, raw.begin() + offset + AESGCMCipher::IV_LEN);
    offset += AESGCMCipher::IV_LEN;

    p.tag.assign(raw.begin() + offset, raw.begin() + offset + AESGCMCipher::TAG_LEN);
    offset += AESGCMCipher::TAG_LEN;

    p.ciphertext.assign(raw.begin() + offset, raw.end());
    return p;
}

std::vector<uint8_t> AESGCMCipher::deriveKey(const std::string& passphrase, const std::vector<uint8_t>& salt) {
    std::vector<uint8_t> key(KEY_LEN);
    PKCS5_PBKDF2_HMAC(
        passphrase.data(), static_cast<int>(passphrase.size()),
        salt.data(), static_cast<int>(salt.size()),
        PBKDF2_ITERATIONS,
        EVP_sha256(),
        static_cast<int>(key.size()),
        key.data()
    );
    return key;
}

std::optional<EncryptedPayload> AESGCMCipher::encrypt(const std::vector<uint8_t>& plaintext, const std::string& passphrase) {
    EncryptedPayload payload;
    payload.salt.resize(SALT_LEN);
    payload.iv.resize(IV_LEN);
    payload.tag.resize(TAG_LEN);

    if (RAND_bytes(payload.salt.data(), static_cast<int>(SALT_LEN)) != 1 ||
        RAND_bytes(payload.iv.data(), static_cast<int>(IV_LEN)) != 1) {
        return std::nullopt;
    }

    auto key = deriveKey(passphrase, payload.salt);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        OPENSSL_cleanse(key.data(), key.size());
        return std::nullopt;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1 ||
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(IV_LEN), nullptr) != 1 ||
        EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.data(), payload.iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        OPENSSL_cleanse(key.data(), key.size());
        return std::nullopt;
    }

    payload.ciphertext.resize(plaintext.size() + 16);
    int outLen1 = 0;
    if (EVP_EncryptUpdate(ctx, payload.ciphertext.data(), &outLen1, plaintext.data(), static_cast<int>(plaintext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        OPENSSL_cleanse(key.data(), key.size());
        return std::nullopt;
    }

    int outLen2 = 0;
    if (EVP_EncryptFinal_ex(ctx, payload.ciphertext.data() + outLen1, &outLen2) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        OPENSSL_cleanse(key.data(), key.size());
        return std::nullopt;
    }
    payload.ciphertext.resize(static_cast<size_t>(outLen1 + outLen2));

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, static_cast<int>(TAG_LEN), payload.tag.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        OPENSSL_cleanse(key.data(), key.size());
        return std::nullopt;
    }

    EVP_CIPHER_CTX_free(ctx);
    OPENSSL_cleanse(key.data(), key.size());
    return payload;
}

std::optional<std::vector<uint8_t>> AESGCMCipher::decrypt(const EncryptedPayload& payload, const std::string& passphrase) {
    if (payload.salt.size() != SALT_LEN || payload.iv.size() != IV_LEN || payload.tag.size() != TAG_LEN) {
        return std::nullopt;
    }

    auto key = deriveKey(passphrase, payload.salt);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        OPENSSL_cleanse(key.data(), key.size());
        return std::nullopt;
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1 ||
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(IV_LEN), nullptr) != 1 ||
        EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), payload.iv.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        OPENSSL_cleanse(key.data(), key.size());
        return std::nullopt;
    }

    std::vector<uint8_t> plaintext(payload.ciphertext.size());
    int outLen1 = 0;
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &outLen1, payload.ciphertext.data(), static_cast<int>(payload.ciphertext.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        OPENSSL_cleanse(key.data(), key.size());
        return std::nullopt;
    }

    // Set expected GCM authentication tag
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, static_cast<int>(TAG_LEN), const_cast<uint8_t*>(payload.tag.data())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        OPENSSL_cleanse(key.data(), key.size());
        return std::nullopt;
    }

    int outLen2 = 0;
    int finalRes = EVP_DecryptFinal_ex(ctx, plaintext.data() + outLen1, &outLen2);
    EVP_CIPHER_CTX_free(ctx);
    OPENSSL_cleanse(key.data(), key.size());

    if (finalRes <= 0) {
        // Authentication tag verification failed!
        return std::nullopt;
    }

    plaintext.resize(static_cast<size_t>(outLen1 + outLen2));
    return plaintext;
}

std::optional<std::vector<uint8_t>> AESGCMCipher::encryptBytes(const std::vector<uint8_t>& plaintext, const std::string& passphrase) {
    auto p = encrypt(plaintext, passphrase);
    if (!p) return std::nullopt;
    return p->serialize();
}

std::optional<std::vector<uint8_t>> AESGCMCipher::decryptBytes(const std::vector<uint8_t>& rawContainer, const std::string& passphrase) {
    auto p = EncryptedPayload::deserialize(rawContainer);
    if (!p) return std::nullopt;
    return decrypt(*p, passphrase);
}

} // namespace crypto
