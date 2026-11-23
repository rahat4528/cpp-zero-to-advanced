#include "PhonebookDatabase.hpp"
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace phonebook {

PhonebookDatabase::PhonebookDatabase(const std::string& dbPath) : m_dbPath(dbPath) {}

PhonebookDatabase::~PhonebookDatabase() {
    close();
}

bool PhonebookDatabase::open() {
    if (m_db) return true;
    if (sqlite3_open(m_dbPath.c_str(), &m_db) != SQLITE_OK) {
        std::cerr << "SQLite open error: " << sqlite3_errmsg(m_db) << "\n";
        m_db = nullptr;
        return false;
    }
    initSchema();
    return true;
}

void PhonebookDatabase::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

void PhonebookDatabase::initSchema() {
    const char* schema = R"(
        CREATE TABLE IF NOT EXISTS contacts (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            phone_number TEXT NOT NULL,
            email TEXT,
            company TEXT,
            notes TEXT,
            is_favorite INTEGER NOT NULL DEFAULT 0
        );
        CREATE INDEX IF NOT EXISTS idx_contacts_name ON contacts(last_name, first_name);
        CREATE INDEX IF NOT EXISTS idx_contacts_phone ON contacts(phone_number);
    )";

    char* err = nullptr;
    if (sqlite3_exec(m_db, schema, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string errStr = err ? err : "Unknown error";
        sqlite3_free(err);
        throw std::runtime_error("Failed to init phonebook schema: " + errStr);
    }
}

int PhonebookDatabase::addContact(const ContactRecord& c) {
    if (!m_db) return -1;

    const char* sql = "INSERT INTO contacts (first_name, last_name, phone_number, email, company, notes, is_favorite) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return -1;
    }

    sqlite3_bind_text(stmt, 1, c.firstName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, c.lastName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, c.phoneNumber.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, c.email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, c.company.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, c.notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, c.isFavorite ? 1 : 0);

    int newId = -1;
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        newId = static_cast<int>(sqlite3_last_insert_rowid(m_db));
    }
    sqlite3_finalize(stmt);
    return newId;
}

bool PhonebookDatabase::updateContact(const ContactRecord& c) {
    if (!m_db) return false;

    const char* sql = "UPDATE contacts SET first_name = ?, last_name = ?, phone_number = ?, "
                      "email = ?, company = ?, notes = ?, is_favorite = ? WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, c.firstName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, c.lastName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, c.phoneNumber.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, c.email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, c.company.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, c.notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 7, c.isFavorite ? 1 : 0);
    sqlite3_bind_int(stmt, 8, c.id);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    int changes = sqlite3_changes(m_db);
    sqlite3_finalize(stmt);
    return ok && (changes > 0);
}

bool PhonebookDatabase::deleteContact(int id) {
    if (!m_db) return false;

    const char* sql = "DELETE FROM contacts WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_int(stmt, 1, id);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    int changes = sqlite3_changes(m_db);
    sqlite3_finalize(stmt);
    return ok && (changes > 0);
}

static ContactRecord extractContact(sqlite3_stmt* stmt) {
    ContactRecord c;
    c.id = sqlite3_column_int(stmt, 0);
    c.firstName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    c.lastName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
    c.phoneNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
    const auto* email = sqlite3_column_text(stmt, 4);
    c.email = email ? reinterpret_cast<const char*>(email) : "";
    const auto* comp = sqlite3_column_text(stmt, 5);
    c.company = comp ? reinterpret_cast<const char*>(comp) : "";
    const auto* notes = sqlite3_column_text(stmt, 6);
    c.notes = notes ? reinterpret_cast<const char*>(notes) : "";
    c.isFavorite = (sqlite3_column_int(stmt, 7) != 0);
    return c;
}

std::optional<ContactRecord> PhonebookDatabase::getContact(int id) {
    if (!m_db) return std::nullopt;

    const char* sql = "SELECT id, first_name, last_name, phone_number, email, company, notes, is_favorite FROM contacts WHERE id = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }
    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        ContactRecord c = extractContact(stmt);
        sqlite3_finalize(stmt);
        return c;
    }

    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::vector<ContactRecord> PhonebookDatabase::getAllContacts() {
    std::vector<ContactRecord> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, first_name, last_name, phone_number, email, company, notes, is_favorite FROM contacts ORDER BY last_name, first_name;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return list;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list.push_back(extractContact(stmt));
    }
    sqlite3_finalize(stmt);
    return list;
}

std::vector<ContactRecord> PhonebookDatabase::searchByName(const std::string& query) {
    std::vector<ContactRecord> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, first_name, last_name, phone_number, email, company, notes, is_favorite "
                      "FROM contacts WHERE first_name LIKE ? OR last_name LIKE ? ORDER BY last_name, first_name;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return list;
    }

    std::string pattern = "%" + query + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, pattern.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list.push_back(extractContact(stmt));
    }
    sqlite3_finalize(stmt);
    return list;
}

std::vector<ContactRecord> PhonebookDatabase::searchByPhone(const std::string& query) {
    std::vector<ContactRecord> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, first_name, last_name, phone_number, email, company, notes, is_favorite "
                      "FROM contacts WHERE phone_number LIKE ? ORDER BY last_name, first_name;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return list;
    }

    std::string pattern = "%" + query + "%";
    sqlite3_bind_text(stmt, 1, pattern.c_str(), -1, SQLITE_TRANSIENT);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list.push_back(extractContact(stmt));
    }
    sqlite3_finalize(stmt);
    return list;
}

std::vector<ContactRecord> PhonebookDatabase::getFavorites() {
    std::vector<ContactRecord> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, first_name, last_name, phone_number, email, company, notes, is_favorite "
                      "FROM contacts WHERE is_favorite = 1 ORDER BY last_name, first_name;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return list;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        list.push_back(extractContact(stmt));
    }
    sqlite3_finalize(stmt);
    return list;
}

std::string PhonebookDatabase::exportToVCard(int id) {
    auto c = getContact(id);
    if (!c) return "";

    std::ostringstream vcf;
    vcf << "BEGIN:VCARD\r\n";
    vcf << "VERSION:3.0\r\n";
    vcf << "N:" << c->lastName << ";" << c->firstName << ";;;\r\n";
    vcf << "FN:" << c->firstName << " " << c->lastName << "\r\n";
    if (!c->company.empty()) vcf << "ORG:" << c->company << "\r\n";
    vcf << "TEL;TYPE=CELL:" << c->phoneNumber << "\r\n";
    if (!c->email.empty()) vcf << "EMAIL;TYPE=INTERNET:" << c->email << "\r\n";
    if (!c->notes.empty()) vcf << "NOTE:" << c->notes << "\r\n";
    vcf << "END:VCARD\r\n";
    return vcf.str();
}

bool PhonebookDatabase::importFromVCard(const std::string& vcardText) {
    std::istringstream stream(vcardText);
    std::string line;
    ContactRecord c;

    while (std::getline(stream, line)) {
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) line.pop_back();

        if (line.rfind("N:", 0) == 0) {
            std::string names = line.substr(2);
            size_t semi = names.find(';');
            if (semi != std::string::npos) {
                c.lastName = names.substr(0, semi);
                size_t semi2 = names.find(';', semi + 1);
                c.firstName = names.substr(semi + 1, semi2 == std::string::npos ? std::string::npos : (semi2 - semi - 1));
            }
        } else if (line.rfind("FN:", 0) == 0 && c.firstName.empty()) {
            c.firstName = line.substr(3);
        } else if (line.rfind("TEL", 0) == 0) {
            size_t colon = line.find(':');
            if (colon != std::string::npos) c.phoneNumber = line.substr(colon + 1);
        } else if (line.rfind("EMAIL", 0) == 0) {
            size_t colon = line.find(':');
            if (colon != std::string::npos) c.email = line.substr(colon + 1);
        } else if (line.rfind("ORG:", 0) == 0) {
            c.company = line.substr(4);
        } else if (line.rfind("NOTE:", 0) == 0) {
            c.notes = line.substr(5);
        }
    }

    if (!c.firstName.empty() && !c.phoneNumber.empty()) {
        return addContact(c) > 0;
    }
    return false;
}

} // namespace phonebook
