# 08-Digital-Signature-Maker

## 1. Project Overview
08-Digital-Signature-Maker is a secure, high-assurance digital signature generation and verification engine built in standard C++17 using the OpenSSL 3.x/1.1.x `EVP` cryptographic API. It supports RSA asymmetric key generation, SHA-256 message hashing, Base64 signature packaging, and strict non-repudiation verification.

## 2. Problem Statement
In distributed software and financial systems, digital communications are susceptible to man-in-the-middle tampering, spoofing, and repudiation. A robust asymmetric cryptographic pipeline is essential to guarantee data integrity (payload has not been altered), authenticity (origin is confirmed by private key possession), and non-repudiation.

## 3. Objectives
- Implement 2048-bit RSA key pair synthesis using OpenSSL `EVP_PKEY_CTX`.
- Export private and public cryptographic keys to standardized PEM format via OpenSSL `BIO`.
- Implement asymmetric digital signature creation using SHA-256 digests (`EVP_DigestSignInit`/`Update`/`Final`).
- Implement signature verification (`EVP_DigestVerifyInit`/`Update`/`Final`) with tamper detection.
- Provide automated CI verification mode (`--test-run`) and complete doctest coverage.

## 4. Features
- **Modern OpenSSL EVP API**: Avoids legacy low-level primitives in favor of standard `EVP_PKEY` and `EVP_MD_CTX` interfaces.
- **Base64 Armor**: Signatures are serialized as clean Base64 strings for simple transport via JSON/HTTP/email.
- **Zero-Tolerance Tamper Detection**: Any modification of even a single byte or space in the signed document causes signature verification to reject the payload.
- **Independent Key Verification**: Rejects signatures signed by mismatched or unauthorized key pairs.

## 5. Technology Stack
- **Language**: C++17 (`std::vector`, `std::string`, `std::runtime_error`, `std::cstdint`).
- **Cryptographic Library**: OpenSSL (`libcrypto`, `openssl/evp.h`, `openssl/pem.h`).
- **External Services**: None. Operates completely offline.
- **Database**: None required.
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
08-Digital-Signature-Maker/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   └── CryptoEngine.hpp
├── src/
│   ├── CryptoEngine.cpp
│   └── main.cpp
└── tests/
    └── test_digital_signature.cpp
```

## 7. Installation / Prerequisites
- CMake 3.20 or newer.
- GCC 11+, Clang 14+, or MSVC 2022.
- OpenSSL development libraries (`libcrypto`).
- doctest headers.

## 8. Build and Run Instructions
### Windows & Linux Build
```bash
cmake -S . -B build -G Ninja
cmake --build build
```

### Running the Application
```bash
./build/DigitalSignatureMaker
```

### Automated Verification Mode
```bash
./build/DigitalSignatureMaker --test-run
```

## 9. Testing Instructions
Run CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
========================================
  OpenSSL Digital Signature Generator   
========================================
Generating 2048-bit RSA key pair...
Key pair successfully generated.
Public Key Preview:
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA...
Message to sign: "PAY TO: Alice, AMOUNT: 500.00 USD, NONCE: 98124"
Generated Base64 Signature:
a8eF92...==

Verifying signature with public key...
Signature Validity: VERIFIED (AUTHENTIC)
Testing tamper resistance with altered payload...
Tampered Message Verification: SUCCESS (DETECTED & REJECTED)

[Test Mode] OpenSSL cryptographic signing engine verified.
```

## 11. Known Limitations
- Current implementation defaults to 2048-bit RSA; Ed25519 (EdDSA) can be configured for environments requiring shorter signatures.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows linking OpenSSL `libcrypto.dll.a` with 0 warnings.
- **Run**: Verified RSA generation, signing, verification, and tamper detection with `--test-run`.
- **Tested**: All 3 test suites passed under CTest.
