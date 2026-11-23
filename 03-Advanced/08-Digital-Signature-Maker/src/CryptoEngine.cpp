#include "CryptoEngine.hpp"
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <stdexcept>
#include <iostream>

namespace crypto {

std::string CryptoEngine::base64Encode(const std::vector<uint8_t>& data) {
    if (data.empty()) return "";
    int encodedLen = (static_cast<int>(data.size()) + 2) / 3 * 4;
    std::string out(static_cast<size_t>(encodedLen), '\0');
    EVP_EncodeBlock(reinterpret_cast<unsigned char*>(&out[0]), data.data(), static_cast<int>(data.size()));
    return out;
}

std::vector<uint8_t> CryptoEngine::base64Decode(const std::string& encoded) {
    if (encoded.empty()) return {};
    int maxLen = (static_cast<int>(encoded.size()) / 4) * 3;
    std::vector<uint8_t> out(static_cast<size_t>(maxLen));
    int len = EVP_DecodeBlock(out.data(), reinterpret_cast<const unsigned char*>(encoded.data()), static_cast<int>(encoded.size()));
    if (len < 0) return {};

    // Adjust for base64 padding '='
    if (encoded.size() >= 2) {
        if (encoded.back() == '=') len--;
        if (encoded[encoded.size() - 2] == '=') len--;
    }
    if (len > 0) {
        out.resize(static_cast<size_t>(len));
    } else {
        out.clear();
    }
    return out;
}

KeyPair CryptoEngine::generateRSAKeyPair(int bits) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx) throw std::runtime_error("Failed to create EVP_PKEY_CTX");

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Failed to init keygen");
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, bits) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Failed to set RSA bit size");
    }

    EVP_PKEY* pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        throw std::runtime_error("Key generation failed");
    }
    EVP_PKEY_CTX_free(ctx);

    // Write Private Key to PEM
    BIO* privBio = BIO_new(BIO_s_mem());
    PEM_write_bio_PrivateKey(privBio, pkey, nullptr, nullptr, 0, nullptr, nullptr);
    char* privData = nullptr;
    long privLen = BIO_get_mem_data(privBio, &privData);
    std::string privPem(privData, static_cast<size_t>(privLen));
    BIO_free(privBio);

    // Write Public Key to PEM
    BIO* pubBio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(pubBio, pkey);
    char* pubData = nullptr;
    long pubLen = BIO_get_mem_data(pubBio, &pubData);
    std::string pubPem(pubData, static_cast<size_t>(pubLen));
    BIO_free(pubBio);

    EVP_PKEY_free(pkey);

    return {privPem, pubPem};
}

std::string CryptoEngine::signMessage(const std::string& message, const std::string& privateKeyPem) {
    BIO* bio = BIO_new_mem_buf(privateKeyPem.data(), static_cast<int>(privateKeyPem.size()));
    if (!bio) return "";

    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if (!pkey) return "";

    EVP_MD_CTX* mdCtx = EVP_MD_CTX_new();
    if (!mdCtx) {
        EVP_PKEY_free(pkey);
        return "";
    }

    if (EVP_DigestSignInit(mdCtx, nullptr, EVP_sha256(), nullptr, pkey) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        EVP_PKEY_free(pkey);
        return "";
    }

    if (EVP_DigestSignUpdate(mdCtx, message.data(), message.size()) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        EVP_PKEY_free(pkey);
        return "";
    }

    size_t sigLen = 0;
    if (EVP_DigestSignFinal(mdCtx, nullptr, &sigLen) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        EVP_PKEY_free(pkey);
        return "";
    }

    std::vector<uint8_t> sig(sigLen);
    if (EVP_DigestSignFinal(mdCtx, sig.data(), &sigLen) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        EVP_PKEY_free(pkey);
        return "";
    }
    sig.resize(sigLen);

    EVP_MD_CTX_free(mdCtx);
    EVP_PKEY_free(pkey);

    return base64Encode(sig);
}

bool CryptoEngine::verifySignature(const std::string& message, const std::string& base64Signature, const std::string& publicKeyPem) {
    BIO* bio = BIO_new_mem_buf(publicKeyPem.data(), static_cast<int>(publicKeyPem.size()));
    if (!bio) return false;

    EVP_PKEY* pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    if (!pkey) return false;

    std::vector<uint8_t> rawSig = base64Decode(base64Signature);
    if (rawSig.empty()) {
        EVP_PKEY_free(pkey);
        return false;
    }

    EVP_MD_CTX* mdCtx = EVP_MD_CTX_new();
    if (!mdCtx) {
        EVP_PKEY_free(pkey);
        return false;
    }

    if (EVP_DigestVerifyInit(mdCtx, nullptr, EVP_sha256(), nullptr, pkey) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        EVP_PKEY_free(pkey);
        return false;
    }

    if (EVP_DigestVerifyUpdate(mdCtx, message.data(), message.size()) <= 0) {
        EVP_MD_CTX_free(mdCtx);
        EVP_PKEY_free(pkey);
        return false;
    }

    int res = EVP_DigestVerifyFinal(mdCtx, rawSig.data(), rawSig.size());
    EVP_MD_CTX_free(mdCtx);
    EVP_PKEY_free(pkey);

    return res == 1;
}

} // namespace crypto
