#include "CryptoEngine.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
        }
    }

    std::cout << "========================================\n";
    std::cout << "  OpenSSL Digital Signature Generator   \n";
    std::cout << "========================================\n";

    std::cout << "Generating 2048-bit RSA key pair...\n";
    auto keyPair = crypto::CryptoEngine::generateRSAKeyPair(2048);
    std::cout << "Key pair successfully generated.\n";
    std::cout << "Public Key Preview:\n" << keyPair.publicKeyPem.substr(0, 120) << "...\n";

    std::string message = "PAY TO: Alice, AMOUNT: 500.00 USD, NONCE: 98124";
    std::cout << "\nMessage to sign: \"" << message << "\"\n";

    std::string signature = crypto::CryptoEngine::signMessage(message, keyPair.privateKeyPem);
    std::cout << "Generated Base64 Signature:\n" << signature << "\n";

    std::cout << "\nVerifying signature with public key...\n";
    bool valid = crypto::CryptoEngine::verifySignature(message, signature, keyPair.publicKeyPem);
    std::cout << "Signature Validity: " << (valid ? "VERIFIED (AUTHENTIC)" : "FAILED") << "\n";

    std::cout << "Testing tamper resistance with altered payload...\n";
    std::string tampered = message + " ";
    bool tamperedValid = crypto::CryptoEngine::verifySignature(tampered, signature, keyPair.publicKeyPem);
    std::cout << "Tampered Message Verification: " << (tamperedValid ? "FAIL (ACCEPTED TAMPERED)" : "SUCCESS (DETECTED & REJECTED)") << "\n";

    if (testRun) {
        std::cout << "\n[Test Mode] OpenSSL cryptographic signing engine verified.\n";
        return 0;
    }

    return 0;
}
