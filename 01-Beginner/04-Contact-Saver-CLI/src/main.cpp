#include "ContactSaver.h"

#include <iostream>
#include <iomanip>
#include <string>

void printHelp() {
    std::cout << "Contact Saver CLI\n"
              << "Usage:\n"
              << "  ContactSaver --list                       List all contacts\n"
              << "  ContactSaver --add \"Name|Phone|Email|Notes\" Add new contact\n"
              << "  ContactSaver --search <query>             Search contacts\n"
              << "  ContactSaver --delete <id/name>           Delete a contact\n"
              << "  ContactSaver --file <filepath>            Specify custom CSV storage file\n"
              << "  ContactSaver --help                       Show this help message\n";
}

void printContactTable(const std::vector<contact_saver::Contact>& contacts) {
    if (contacts.empty()) {
        std::cout << "No contacts found.\n";
        return;
    }
    std::cout << "\n========================================================================================\n";
    std::cout << std::left << std::setw(8) << "ID"
              << std::setw(24) << "NAME"
              << std::setw(18) << "PHONE"
              << std::setw(26) << "EMAIL"
              << "NOTES\n";
    std::cout << "----------------------------------------------------------------------------------------\n";
    for (const auto& c : contacts) {
        std::cout << std::left << std::setw(8) << c.id
                  << std::setw(24) << c.name
                  << std::setw(18) << c.phone
                  << std::setw(26) << c.email
                  << c.notes << "\n";
    }
    std::cout << "========================================================================================\n";
}

int main(int argc, char* argv[]) {
    contact_saver::ContactBook book;
    std::string csvPath = "contacts.csv";

    // Check for custom file argument first
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) {
            csvPath = argv[++i];
        }
    }

    book.loadFromCsv(csvPath);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--list") {
            printContactTable(book.getAllContacts());
            return 0;
        } else if (arg == "--search" && i + 1 < argc) {
            std::string q = argv[++i];
            printContactTable(book.search(q));
            return 0;
        } else if (arg == "--delete" && i + 1 < argc) {
            std::string target = argv[++i];
            if (book.removeContact(target)) {
                book.saveToCsv(csvPath);
                std::cout << "Contact '" << target << "' deleted successfully.\n";
            } else {
                std::cerr << "Contact not found.\n";
                return 1;
            }
            return 0;
        } else if (arg == "--add" && i + 1 < argc) {
            std::string payload = argv[++i];
            // Format: "Name|Phone|Email|Notes"
            std::stringstream ss(payload);
            contact_saver::Contact c;
            std::getline(ss, c.name, '|');
            std::getline(ss, c.phone, '|');
            std::getline(ss, c.email, '|');
            std::getline(ss, c.notes, '|');

            if (book.addContact(c)) {
                book.saveToCsv(csvPath);
                std::cout << "Contact '" << c.name << "' saved successfully.\n";
                return 0;
            } else {
                std::cerr << "Error: Invalid contact info (valid name and phone required).\n";
                return 1;
            }
        }
    }

    // Interactive fallback
    std::cout << "=== Contact Saver CLI ===\n";
    std::cout << "Currently loaded contacts: " << book.size() << "\n";
    printContactTable(book.getAllContacts());
    return 0;
}
