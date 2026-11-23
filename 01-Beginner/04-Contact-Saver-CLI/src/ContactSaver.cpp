#include "ContactSaver.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace contact_saver {

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return s;
}

bool ContactBook::isValidPhone(const std::string& phone) {
    if (phone.empty()) return false;
    size_t digitCount = 0;
    for (char c : phone) {
        if (std::isdigit(static_cast<unsigned char>(c))) {
            digitCount++;
        } else if (c != '+' && c != '-' && c != '(' && c != ')' && c != ' ') {
            return false;
        }
    }
    return digitCount >= 7; // Minimal telephone number length
}

bool ContactBook::isValidEmail(const std::string& email) {
    auto atPos = email.find('@');
    if (atPos == std::string::npos || atPos == 0 || atPos == email.length() - 1) {
        return false;
    }
    auto dotPos = email.find('.', atPos);
    if (dotPos == std::string::npos || dotPos == atPos + 1 || dotPos == email.length() - 1) {
        return false;
    }
    return true;
}

bool ContactBook::addContact(const Contact& contact) {
    if (contact.name.empty() || !isValidPhone(contact.phone)) {
        return false;
    }

    Contact newContact = contact;
    if (newContact.id.empty()) {
        newContact.id = "c_" + std::to_string(m_contacts.size() + 1);
    }

    // Check for duplicate ID
    for (const auto& c : m_contacts) {
        if (c.id == newContact.id) {
            return false;
        }
    }

    m_contacts.push_back(newContact);
    // Keep contacts sorted alphabetically by name
    std::sort(m_contacts.begin(), m_contacts.end(), [](const Contact& a, const Contact& b) {
        return a.name < b.name;
    });

    return true;
}

bool ContactBook::removeContact(const std::string& idOrName) {
    std::string lowerQuery = toLower(idOrName);
    auto it = std::remove_if(m_contacts.begin(), m_contacts.end(), [&](const Contact& c) {
        return toLower(c.id) == lowerQuery || toLower(c.name) == lowerQuery;
    });
    if (it != m_contacts.end()) {
        m_contacts.erase(it, m_contacts.end());
        return true;
    }
    return false;
}

std::optional<Contact> ContactBook::getContact(const std::string& id) const {
    for (const auto& c : m_contacts) {
        if (c.id == id) {
            return c;
        }
    }
    return std::nullopt;
}

std::vector<Contact> ContactBook::getAllContacts() const {
    return m_contacts;
}

std::vector<Contact> ContactBook::search(const std::string& query) const {
    std::vector<Contact> results;
    std::string lowerQuery = toLower(query);
    for (const auto& c : m_contacts) {
        if (toLower(c.name).find(lowerQuery) != std::string::npos ||
            toLower(c.phone).find(lowerQuery) != std::string::npos ||
            toLower(c.email).find(lowerQuery) != std::string::npos ||
            toLower(c.notes).find(lowerQuery) != std::string::npos) {
            results.push_back(c);
        }
    }
    return results;
}

std::string ContactBook::serializeContact(const Contact& contact) {
    std::ostringstream oss;
    oss << contact.id << ","
        << contact.name << ","
        << contact.phone << ","
        << contact.email << ","
        << contact.notes;
    return oss.str();
}

std::optional<Contact> ContactBook::deserializeContact(const std::string& line) {
    std::istringstream iss(line);
    std::string token;
    Contact c;

    if (!std::getline(iss, c.id, ',')) return std::nullopt;
    if (!std::getline(iss, c.name, ',')) return std::nullopt;
    if (!std::getline(iss, c.phone, ',')) return std::nullopt;
    if (!std::getline(iss, c.email, ',')) return std::nullopt;
    std::getline(iss, c.notes); // Notes may be empty

    if (c.name.empty() || c.phone.empty()) return std::nullopt;
    return c;
}

bool ContactBook::saveToCsv(const std::string& filepath) const {
    std::ofstream outFile(filepath);
    if (!outFile.is_open()) {
        return false;
    }
    outFile << "id,name,phone,email,notes\n";
    for (const auto& c : m_contacts) {
        outFile << serializeContact(c) << "\n";
    }
    return true;
}

bool ContactBook::loadFromCsv(const std::string& filepath) {
    std::ifstream inFile(filepath);
    if (!inFile.is_open()) {
        return false;
    }

    std::string line;
    // Skip header line if present
    if (std::getline(inFile, line)) {
        if (line.find("id,name") == std::string::npos) {
            // Not a header line, process it as a record
            auto cOpt = deserializeContact(line);
            if (cOpt) addContact(*cOpt);
        }
    }

    while (std::getline(inFile, line)) {
        if (line.empty()) continue;
        auto cOpt = deserializeContact(line);
        if (cOpt) {
            addContact(*cOpt);
        }
    }
    return true;
}

} // namespace contact_saver
