#pragma once

#include <string>
#include <vector>
#include <optional>

namespace contact_saver {

struct Contact {
    std::string id;
    std::string name;
    std::string phone;
    std::string email;
    std::string notes;
};

class ContactBook {
public:
    ContactBook() = default;

    // Contact CRUD operations
    bool addContact(const Contact& contact);
    bool removeContact(const std::string& idOrName);
    std::optional<Contact> getContact(const std::string& id) const;
    std::vector<Contact> getAllContacts() const;

    // Search and Filter
    std::vector<Contact> search(const std::string& query) const;

    // File Persistence (CSV)
    bool saveToCsv(const std::string& filepath) const;
    bool loadFromCsv(const std::string& filepath);

    // Serialization helpers
    static std::string serializeContact(const Contact& contact);
    static std::optional<Contact> deserializeContact(const std::string& line);

    // Validation
    static bool isValidPhone(const std::string& phone);
    static bool isValidEmail(const std::string& email);

    size_t size() const { return m_contacts.size(); }
    void clear() { m_contacts.clear(); }

private:
    std::vector<Contact> m_contacts;
};

} // namespace contact_saver
