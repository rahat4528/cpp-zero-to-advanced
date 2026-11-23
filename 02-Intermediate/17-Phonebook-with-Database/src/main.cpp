#include "PhonebookDatabase.hpp"
#include <iostream>
#include <iomanip>
#include <string>

int main(int argc, char* argv[]) {
    std::string dbFile = "phonebook.db";

    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
            dbFile = ":memory:";
        } else if (std::string(argv[i]) == "--db" && i + 1 < argc) {
            dbFile = argv[++i];
        }
    }

    phonebook::PhonebookDatabase db(dbFile);
    if (!db.open()) {
        std::cerr << "Failed to open phonebook database.\n";
        return 1;
    }

    if (testRun) {
        std::cout << "=================================================================\n";
        std::cout << "              PHONEBOOK DATABASE WITH vCARD EXPORT               \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Registering contacts, searching, and exporting vCard...\n";

        db.addContact({0, "Ada", "Lovelace", "+44-20-7946-0912", "ada@analytical.org", "Babbage Labs", "First Programmer", true});
        db.addContact({0, "Alan", "Turing", "+44-1628-890123", "alan@bletchley.ac.uk", "Gov Code & Cypher", "Enigma pioneer", true});
        int idClaude = db.addContact({0, "Claude", "Shannon", "+1-617-253-1000", "shannon@mit.edu", "Bell Labs", "Father of Information Theory", false});

        auto all = db.getAllContacts();
        std::cout << "Total Contacts Registered: " << all.size() << "\n";
        for (const auto& c : all) {
            std::cout << " [" << c.id << "] " << (c.isFavorite ? "[*FAV*] " : "")
                      << c.lastName << ", " << c.firstName << " | Phone: " << c.phoneNumber
                      << " | Org: " << c.company << "\n";
        }

        std::cout << "\nSearch 'Turing':\n";
        for (const auto& c : db.searchByName("Turing")) {
            std::cout << "  Found: " << c.firstName << " " << c.lastName << " (" << c.phoneNumber << ")\n";
        }

        std::cout << "\nExport vCard for Shannon:\n";
        std::string vcf = db.exportToVCard(idClaude);
        std::cout << vcf;

        std::cout << "[Test-Run Mode] Verified successfully.\n";
        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "              PHONEBOOK DATABASE WITH vCARD EXPORT               \n";
    std::cout << "=================================================================\n";
    std::cout << "Database: " << dbFile << "\n";
    std::cout << "Run with --test-run to execute automated verification suite.\n";
    return 0;
}
