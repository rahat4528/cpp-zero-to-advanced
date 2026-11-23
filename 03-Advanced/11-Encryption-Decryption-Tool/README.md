# 11-Encryption-Decryption-Tool

## 1. Project Overview
11-Encryption-Decryption-Tool is an enterprise-grade cryptographic utility built in C++17 leveraging the OpenSSL `EVP` API. It implements Authenticated Encryption with Associated Data (AEAD) via AES-256-GCM, password key derivation via PBKDF2 with HMAC-SHA256 (100,000 iterations), randomized salts and initialization vectors (IVs), and strict authentication tag verification.

## 2. Problem Statement
Legacy symmetric encryption schemes (such as AES-CBC or AES-ECB) fail to provide integrity guarantees without separate HMAC constructions, leaving payloads vulnerable to padding oracle and chosen-ciphertext bit-flipping attacks. Modern security architectures require authenticated ciphers like AES-GCM that guarantee both confidentiality and authenticity in a single unified primitive.

## 3. Objectives
- Implement AES-256-GCM authenticated symmetric encryption and decryption using OpenSSL `EVP_CIPHER_CTX`.
- Derive high-entropy 256-bit symmetric keys from user passphrases using PBKDF2-HMAC-SHA256 with 100,000 rounds and random 16-byte salts.
- Generate cryptographically secure 96-bit (12-byte) initialization vectors via `RAND_bytes`.
- Validate 128-bit (16-byte) GCM authentication tags (`EVP_CTRL_GCM_SET_TAG`), guaranteeing rejection of any tampered ciphertext.
- Sanitize sensitive memory buffers using `OPENSSL_cleanse`.

## 4. Features
- **AEAD Security Model**: Protects against passive eavesdropping and active ciphertext modification.
- **Self-Contained Container Format**: Prefixes files with magic bytes (`A256`), 16-byte salt, 12-byte IV, and 16-byte tag.
- **Zero-Tolerance Tamper Rejection**: Altering any bit of ciphertext or tag causes decryption to abort with zero plaintext leakage.
- **Memory Cleansing**: Sensitive key material is zeroed immediately after cipher initialization.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::string`, `std::optional`, `std::cstdint`).
- **Cryptographic Library**: OpenSSL (`libcrypto`, `openssl/evp.h`, `openssl/rand.h`).
- **External Services**: None. Fully offline and self-contained.
- **Database**: None required.
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
11-Encryption-Decryption-Tool/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   └── AESGCMCipher.hpp
├── src/
│   ├── AESGCMCipher.cpp
│   └── main.cpp
└── tests/
    └── test_encryption_tool.cpp
```

## 7. Installation / Prerequisites
- CMake 3.20 or newer.
- GCC 11+, Clang 14+, or MSVC 2022.
- OpenSSL development library (`libcrypto`).
- doctest headers.

## 8. Build and Run Instructions
### Windows & Linux Build
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Running the Application
```bash
./build/EncryptionTool
```

### Automated Verification Mode
```bash
./build/EncryptionTool --test-run
```

## 9. Testing Instructions
Run CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
========================================
 AES-256-GCM Encryption/Decryption Tool 
========================================
Plaintext payload: "Top Secret: Autonomous AI Kernel Architecture Specification 2026"
Encrypting with AES-256-GCM (PBKDF2-SHA256, 100k iterations)...
Encrypted container size: 109 bytes

Decrypting with correct password...
Decryption SUCCESS: "Top Secret: Autonomous AI Kernel Architecture Specification 2026"

Testing tamper resistance (modifying 1 ciphertext byte)...
Tampered payload decryption: SUCCESS (TAG REJECTED TAMPERING)
Testing wrong password rejection...
Wrong password decryption: SUCCESS (REJECTED WRONG KEY)

[Test Mode] AES-256-GCM authenticated encryption engine verified.
```

## 11. Known Limitations
- Passphrases should be at least 12 characters with high entropy to maximize PBKDF2 security.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows linking OpenSSL `libcrypto` with 0 warnings.
- **Run**: Verified AES-256-GCM encryption, decryption, authentication tag tamper rejection, and PBKDF2 derivation with `--test-run`.
- **Tested**: All 4 test cases passed under CTest.
