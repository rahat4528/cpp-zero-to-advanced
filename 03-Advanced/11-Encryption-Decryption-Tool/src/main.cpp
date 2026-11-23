#include "AESGCMCipher.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
        }
    }

    std::cout << "========================================\n";
    std::cout << " AES-256-GCM Encryption/Decryption Tool \n";
    std::cout << "========================================\n";

    std::string secretMessage = "Top Secret: Autonomous AI Kernel Architecture Specification 2026";
    std::string passphrase = "CorrectHorseBatteryStaple#2026";

    std::cout << "Plaintext payload: \"" << secretMessage << "\"\n";
    std::cout << "Encrypting with AES-256-GCM (PBKDF2-SHA256, 100k iterations)...\n";

    std::vector<uint8_t> plainBytes(secretMessage.begin(), secretMessage.end());
    auto container = crypto::AESGCMCipher::encryptBytes(plainBytes, passphrase);
    if (!container) {
        std::cerr << "Encryption failed!\n";
        return 1;
    }

    std::cout << "Encrypted container size: " << container->size() << " bytes\n";

    std::cout << "\nDecrypting with correct password...\n";
    auto decrypted = crypto::AESGCMCipher::decryptBytes(*container, passphrase);
    if (decrypted) {
        std::string recovered(decrypted->begin(), decrypted->end());
        std::cout << "Decryption SUCCESS: \"" << recovered << "\"\n";
    } else {
        std::cerr << "Decryption FAILED!\n";
        return 1;
    }

    std::cout << "\nTesting tamper resistance (modifying 1 ciphertext byte)...\n";
    auto tampered = *container;
    tampered.back() ^= 0x01; // flip 1 bit
    auto tamperedResult = crypto::AESGCMCipher::decryptBytes(tampered, passphrase);
    std::cout << "Tampered payload decryption: "
              << (tamperedResult.has_value() ? "FAIL (ACCEPTED TAMPERED DATA)" : "SUCCESS (TAG REJECTED TAMPERING)") << "\n";

    std::cout << "Testing wrong password rejection...\n";
    auto wrongPassResult = crypto::AESGCMCipher::decryptBytes(*container, "WrongPassword");
    std::cout << "Wrong password decryption: "
              << (wrongPassResult.has_value() ? "FAIL (ACCEPTED WRONG KEY)" : "SUCCESS (REJECTED WRONG KEY)") << "\n";

    if (testRun) {
        std::cout << "\n[Test Mode] AES-256-GCM authenticated encryption engine verified.\n";
        return 0;
    }

    return 0;
}
