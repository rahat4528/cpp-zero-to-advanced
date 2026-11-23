# 13-Cloud-Storage-App

## 1. Project Overview
13-Cloud-Storage-App is a high-reliability client-server cloud object storage system built in standard C++17. Designed to model modern distributed storage protocols (such as Amazon S3 / Google Cloud Storage), the application implements authenticated session token issuance, multi-part chunked file uploading with offset tracking, SHA-256 chunk hash reconciliation, and isolated tenant storage vaults.

## 2. Problem Statement
Uploading multi-gigabyte or large datasets over wide-area networks in a single continuous stream leads to total transfer failure if network interruptions occur. Cloud storage platforms mitigate this by dividing files into discrete chunks, streaming chunks independently with content hashes, and committing the file only upon full digest verification.

## 3. Objectives
- Implement non-blocking multi-threaded TCP socket communication (`SocketCompat.hpp`) across Windows Winsock2 and Linux POSIX.
- Implement token-based credential authentication (`LOGIN <user> <pass>` -> `TOKEN <uuid>`).
- Implement multi-part chunked upload pipeline (`UPLOAD_INIT`, `UPLOAD_CHUNK`, `UPLOAD_FINISH`) with cryptographic SHA-256 integrity verification.
- Implement streaming file retrieval (`DOWNLOAD`) with byte-level checksum validation.
- Provide automated CI test execution via `--test-run` and full doctest coverage.

## 4. Features
- **Chunked Transfer Protocol**: Files are split into configurable chunk sizes (e.g. 16 KB - 64 KB) with discrete offsets.
- **Strict Cryptographic Integrity**: The server computes a SHA-256 digest across assembled chunks, rejecting uploads if a single bit is corrupted.
- **Tenant Isolation**: Independent in-memory/file namespaces preventing cross-tenant access.
- **Graceful Thread Management**: Detached worker pools with cooperative cancellation on server shutdown.

## 5. Technology Stack
- **Language**: C++17 (`std::thread`, `std::mutex`, `std::vector`, `std::map`, `std::optional`).
- **Networking**: BSD/POSIX Sockets (Linux); Winsock 2 (`ws2_32`) on Windows.
- **Cryptography**: OpenSSL `libcrypto` (`EVP_MD_CTX`, `EVP_sha256`).
- **Database**: None required.
- **Testing**: `doctest` unit testing framework.

## 6. Folder Structure
```
13-Cloud-Storage-App/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── include/
│   ├── CloudClient.hpp
│   ├── CloudProtocol.hpp
│   ├── CloudServer.hpp
│   └── SocketCompat.hpp
├── src/
│   ├── CloudClient.cpp
│   ├── CloudServer.cpp
│   ├── client_main.cpp
│   └── server_main.cpp
└── tests/
    └── test_cloud_storage.cpp
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

### Running Server
```bash
./build/CloudServer --port 8080
```

### Running Client
```bash
./build/CloudClient
```

### Automated Verification Mode
```bash
./build/CloudServer --test-run
./build/CloudClient --test-run
```

## 9. Testing Instructions
Run CTest:
```bash
ctest --test-dir build --output-on-failure
```

## 10. Console Output Sample
```
========================================
     Cloud Storage Client Engine        
========================================
[Test Mode] Running automated loopback client-server verification...
Authenticating as 'developer'...
Session established with Token: b7a1f...
Uploading 'readme.txt' (54 bytes) in 16-byte chunks...
Chunked upload and SHA-256 validation verified by server.
Downloading 'readme.txt'...
Download SUCCESS: Content matches byte-for-byte!
[Test Mode] Cloud client-server test passed.
```

## 11. Known Limitations
- Current session tokens are held in-memory; clustered deployments typically store token state in Redis or relational databases.

## 12. Testing Status
- **Built**: Yes, with GCC 16.1.0 on Windows linking OpenSSL and `ws2_32` with 0 warnings.
- **Run**: Verified multi-part chunked upload, SHA-256 reconciliation, and download with `--test-run`.
- **Tested**: All unit test suites passed under CTest.
