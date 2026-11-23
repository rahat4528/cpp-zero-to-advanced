#pragma once

#include <string>
#include <vector>
#include <optional>

namespace library_manager {

struct Book {
    std::string id;
    std::string title;
    std::string author;
    std::string isbn;
    std::string category;
    bool isCheckedOut{false};
    std::string borrowerName;
};

class LibraryManager {
public:
    LibraryManager() = default;

    // Book catalog management
    bool addBook(const Book& book);
    bool removeBook(const std::string& bookId);
    std::optional<Book> findBookById(const std::string& bookId) const;

    // Circulation operations
    bool checkoutBook(const std::string& bookId, const std::string& patronName);
    bool returnBook(const std::string& bookId);

    // Queries & Searching
    std::vector<Book> getAllBooks() const;
    std::vector<Book> search(const std::string& query) const;
    std::vector<Book> getCheckedOutBooks() const;
    std::vector<Book> getAvailableBooks() const;

    // Persistence
    bool saveToFile(const std::string& filepath) const;
    bool loadFromFile(const std::string& filepath);

    static std::string serializeBook(const Book& book);
    static std::optional<Book> deserializeBook(const std::string& line);

    size_t size() const { return m_catalog.size(); }
    void clear() { m_catalog.clear(); }

private:
    std::vector<Book> m_catalog;
};

} // namespace library_manager
