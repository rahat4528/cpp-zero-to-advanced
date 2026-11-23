#include "LibraryManager.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

void printHelp() {
    std::cout << "Library Manager CLI\n"
              << "Usage:\n"
              << "  LibraryManager --list                               List all books\n"
              << "  LibraryManager --add \"Title|Author|ISBN|Category\"   Add book\n"
              << "  LibraryManager --borrow <id> --patron <name>        Borrow book\n"
              << "  LibraryManager --return <id>                        Return book\n"
              << "  LibraryManager --search <query>                     Search catalog\n"
              << "  LibraryManager --file <path>                        Specify catalog file\n"
              << "  LibraryManager --help                               Show help\n";
}

void printTable(const std::vector<library_manager::Book>& books) {
    if (books.empty()) {
        std::cout << "No catalog records found.\n";
        return;
    }
    std::cout << "\n====================================================================================================\n";
    std::cout << std::left << std::setw(8) << "ID"
              << std::setw(30) << "TITLE"
              << std::setw(22) << "AUTHOR"
              << std::setw(16) << "ISBN"
              << std::setw(12) << "STATUS"
              << "BORROWER\n";
    std::cout << "----------------------------------------------------------------------------------------------------\n";
    for (const auto& b : books) {
        std::cout << std::left << std::setw(8) << b.id
                  << std::setw(30) << b.title
                  << std::setw(22) << b.author
                  << std::setw(16) << b.isbn
                  << std::setw(12) << (b.isCheckedOut ? "CHECKED OUT" : "AVAILABLE")
                  << (b.isCheckedOut ? b.borrowerName : "-") << "\n";
    }
    std::cout << "====================================================================================================\n";
}

int main(int argc, char* argv[]) {
    library_manager::LibraryManager lib;
    std::string filePath = "library_catalog.txt";

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) {
            filePath = argv[++i];
        }
    }

    lib.loadFromFile(filePath);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--list") {
            printTable(lib.getAllBooks());
            return 0;
        } else if (arg == "--search" && i + 1 < argc) {
            std::string q = argv[++i];
            printTable(lib.search(q));
            return 0;
        } else if (arg == "--borrow" && i + 1 < argc) {
            std::string bookId = argv[++i];
            std::string patron = "Anonymous Patron";
            if (i + 1 < argc && std::string(argv[i + 1]) == "--patron" && i + 2 < argc) {
                i++;
                patron = argv[++i];
            }
            if (lib.checkoutBook(bookId, patron)) {
                lib.saveToFile(filePath);
                std::cout << "Book " << bookId << " successfully checked out to " << patron << ".\n";
            } else {
                std::cerr << "Error: Book is either not found or already checked out.\n";
                return 1;
            }
            return 0;
        } else if (arg == "--return" && i + 1 < argc) {
            std::string bookId = argv[++i];
            if (lib.returnBook(bookId)) {
                lib.saveToFile(filePath);
                std::cout << "Book " << bookId << " successfully returned.\n";
            } else {
                std::cerr << "Error: Book is not currently checked out or invalid ID.\n";
                return 1;
            }
            return 0;
        } else if (arg == "--add" && i + 1 < argc) {
            std::string raw = argv[++i];
            std::stringstream ss(raw);
            library_manager::Book b;
            std::getline(ss, b.title, '|');
            std::getline(ss, b.author, '|');
            std::getline(ss, b.isbn, '|');
            std::getline(ss, b.category, '|');
            if (lib.addBook(b)) {
                lib.saveToFile(filePath);
                std::cout << "Book '" << b.title << "' added successfully.\n";
            } else {
                std::cerr << "Error: Invalid book record (title and author required).\n";
                return 1;
            }
            return 0;
        }
    }

    std::cout << "=== Library Manager CLI ===\n";
    printTable(lib.getAllBooks());
    return 0;
}
