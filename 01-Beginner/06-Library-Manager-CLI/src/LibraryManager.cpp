#include "LibraryManager.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace library_manager {

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

bool LibraryManager::addBook(const Book& book) {
    if (book.title.empty() || book.author.empty()) {
        return false;
    }

    Book b = book;
    if (b.id.empty()) {
        b.id = "B" + std::to_string(m_catalog.size() + 101);
    }

    // Verify unique ID
    for (const auto& item : m_catalog) {
        if (item.id == b.id) {
            return false;
        }
    }

    m_catalog.push_back(b);
    return true;
}

bool LibraryManager::removeBook(const std::string& bookId) {
    auto it = std::remove_if(m_catalog.begin(), m_catalog.end(), [&](const Book& b) {
        return b.id == bookId;
    });
    if (it != m_catalog.end()) {
        m_catalog.erase(it, m_catalog.end());
        return true;
    }
    return false;
}

std::optional<Book> LibraryManager::findBookById(const std::string& bookId) const {
    for (const auto& b : m_catalog) {
        if (b.id == bookId) return b;
    }
    return std::nullopt;
}

bool LibraryManager::checkoutBook(const std::string& bookId, const std::string& patronName) {
    if (patronName.empty()) return false;

    for (auto& b : m_catalog) {
        if (b.id == bookId) {
            if (b.isCheckedOut) {
                return false; // Already checked out
            }
            b.isCheckedOut = true;
            b.borrowerName = patronName;
            return true;
        }
    }
    return false; // Book not found
}

bool LibraryManager::returnBook(const std::string& bookId) {
    for (auto& b : m_catalog) {
        if (b.id == bookId) {
            if (!b.isCheckedOut) {
                return false; // Already returned
            }
            b.isCheckedOut = false;
            b.borrowerName.clear();
            return true;
        }
    }
    return false;
}

std::vector<Book> LibraryManager::getAllBooks() const {
    return m_catalog;
}

std::vector<Book> LibraryManager::search(const std::string& query) const {
    std::vector<Book> results;
    std::string lq = toLower(query);
    for (const auto& b : m_catalog) {
        if (toLower(b.title).find(lq) != std::string::npos ||
            toLower(b.author).find(lq) != std::string::npos ||
            toLower(b.isbn).find(lq) != std::string::npos ||
            toLower(b.category).find(lq) != std::string::npos ||
            toLower(b.borrowerName).find(lq) != std::string::npos) {
            results.push_back(b);
        }
    }
    return results;
}

std::vector<Book> LibraryManager::getCheckedOutBooks() const {
    std::vector<Book> results;
    for (const auto& b : m_catalog) {
        if (b.isCheckedOut) results.push_back(b);
    }
    return results;
}

std::vector<Book> LibraryManager::getAvailableBooks() const {
    std::vector<Book> results;
    for (const auto& b : m_catalog) {
        if (!b.isCheckedOut) results.push_back(b);
    }
    return results;
}

std::string LibraryManager::serializeBook(const Book& book) {
    std::ostringstream oss;
    oss << book.id << "|"
        << book.title << "|"
        << book.author << "|"
        << book.isbn << "|"
        << book.category << "|"
        << (book.isCheckedOut ? "1" : "0") << "|"
        << book.borrowerName;
    return oss.str();
}

std::optional<Book> LibraryManager::deserializeBook(const std::string& line) {
    std::istringstream iss(line);
    Book b;
    std::string checkedStr;

    if (!std::getline(iss, b.id, '|')) return std::nullopt;
    if (!std::getline(iss, b.title, '|')) return std::nullopt;
    if (!std::getline(iss, b.author, '|')) return std::nullopt;
    if (!std::getline(iss, b.isbn, '|')) return std::nullopt;
    if (!std::getline(iss, b.category, '|')) return std::nullopt;
    if (!std::getline(iss, checkedStr, '|')) return std::nullopt;
    std::getline(iss, b.borrowerName);

    b.isCheckedOut = (checkedStr == "1");
    if (b.title.empty() || b.author.empty()) return std::nullopt;
    return b;
}

bool LibraryManager::saveToFile(const std::string& filepath) const {
    std::ofstream out(filepath);
    if (!out.is_open()) return false;

    out << "id|title|author|isbn|category|isCheckedOut|borrower\n";
    for (const auto& b : m_catalog) {
        out << serializeBook(b) << "\n";
    }
    return true;
}

bool LibraryManager::loadFromFile(const std::string& filepath) {
    std::ifstream in(filepath);
    if (!in.is_open()) return false;

    std::string line;
    if (std::getline(in, line)) {
        if (line.find("id|title") == std::string::npos) {
            auto bOpt = deserializeBook(line);
            if (bOpt) addBook(*bOpt);
        }
    }

    while (std::getline(in, line)) {
        if (line.empty()) continue;
        auto bOpt = deserializeBook(line);
        if (bOpt) {
            addBook(*bOpt);
        }
    }
    return true;
}

} // namespace library_manager
