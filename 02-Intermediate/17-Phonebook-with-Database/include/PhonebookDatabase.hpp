#pragma once

#include <string>
#include <vector>
#include <optional>
#include <sqlite3.h>

namespace phonebook {

struct ContactRecord {
    int id{0};
    std::string firstName;
    std::string lastName;
    std::string phoneNumber;
    std::string email;
    std::string company;
    std::string notes;
    bool isFavorite{false};
};

class PhonebookDatabase {
public:
    explicit PhonebookDatabase(const std::string& dbPath);
    ~PhonebookDatabase();

    bool open();
    void close();

    int addContact(const ContactRecord& contact);
    bool updateContact(const ContactRecord& contact);
    bool deleteContact(int id);
    std::optional<ContactRecord> getContact(int id);

    std::vector<ContactRecord> getAllContacts();
    std::vector<ContactRecord> searchByName(const std::string& query);
    std::vector<ContactRecord> searchByPhone(const std::string& query);
    std::vector<ContactRecord> getFavorites();

    // vCard 3.0 export and import
    std::string exportToVCard(int id);
    bool importFromVCard(const std::string& vcardText);

private:
    void initSchema();

    std::string m_dbPath;
    sqlite3* m_db{nullptr};
};

} // namespace phonebook
