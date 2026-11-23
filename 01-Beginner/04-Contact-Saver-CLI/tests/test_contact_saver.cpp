#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "ContactSaver.h"

#include <cstdio>

using namespace contact_saver;

TEST_CASE("Validation rules") {
    CHECK(ContactBook::isValidPhone("+1-555-123-4567") == true);
    CHECK(ContactBook::isValidPhone("5551234567") == true);
    CHECK(ContactBook::isValidPhone("(800) 555-0199") == true);
    CHECK(ContactBook::isValidPhone("123") == false); // Too short
    CHECK(ContactBook::isValidPhone("abc-defg") == false);

    CHECK(ContactBook::isValidEmail("john.doe@example.com") == true);
    CHECK(ContactBook::isValidEmail("alice@domain.org") == true);
    CHECK(ContactBook::isValidEmail("invalid-email") == false);
    CHECK(ContactBook::isValidEmail("@nodomain.com") == false);
}

TEST_CASE("Add, sort, and remove contacts") {
    ContactBook book;
    Contact c1{"1", "Zara Smith", "555-111-2222", "zara@test.com", "Friend"};
    Contact c2{"2", "Alex Adams", "555-333-4444", "alex@test.com", "Colleague"};

    CHECK(book.addContact(c1) == true);
    CHECK(book.addContact(c2) == true);
    CHECK(book.size() == 2);

    // Sorted alphabetically
    auto all = book.getAllContacts();
    CHECK(all[0].name == "Alex Adams");
    CHECK(all[1].name == "Zara Smith");

    // Remove by name
    CHECK(book.removeContact("Alex Adams") == true);
    CHECK(book.size() == 1);
    CHECK(book.removeContact("NonExistent") == false);
}

TEST_CASE("Search contacts") {
    ContactBook book;
    book.addContact({"c1", "Bob Johnson", "555-999-8888", "bob@work.com", "DevOps"});
    book.addContact({"c2", "Charlie Brown", "555-444-3333", "charlie@home.net", "Manager"});

    auto res1 = book.search("johnson");
    CHECK(res1.size() == 1);
    CHECK(res1[0].name == "Bob Johnson");

    auto res2 = book.search("DevOps");
    CHECK(res2.size() == 1);
    CHECK(res2[0].name == "Bob Johnson");

    auto res3 = book.search("555");
    CHECK(res3.size() == 2);

    auto res4 = book.search("xyznotfound");
    CHECK(res4.empty());
}

TEST_CASE("CSV File Serialization and Deserialization") {
    const std::string tempFile = "temp_test_contacts.csv";

    ContactBook bookWrite;
    bookWrite.addContact({"1", "Grace Hopper", "555-010-0101", "grace@navy.mil", "Pioneer"});
    bookWrite.addContact({"2", "Alan Turing", "555-020-0202", "alan@bletchley.uk", "Cryptanalyst"});

    CHECK(bookWrite.saveToCsv(tempFile) == true);

    ContactBook bookRead;
    CHECK(bookRead.loadFromCsv(tempFile) == true);
    CHECK(bookRead.size() == 2);

    auto grace = bookRead.getContact("1");
    REQUIRE(grace.has_value());
    CHECK(grace->name == "Grace Hopper");
    CHECK(grace->email == "grace@navy.mil");

    std::remove(tempFile.c_str());
}
