#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "LibraryManager.h"

#include <cstdio>

using namespace library_manager;

TEST_CASE("Add and catalog query") {
    LibraryManager lib;
    Book b1{"B101", "The C++ Programming Language", "Bjarne Stroustrup", "978-0321563842", "Programming", false, ""};
    Book b2{"B102", "Effective Modern C++", "Scott Meyers", "978-1491903995", "Programming", false, ""};

    CHECK(lib.addBook(b1) == true);
    CHECK(lib.addBook(b2) == true);
    // Duplicate ID rejected
    CHECK(lib.addBook(b1) == false);

    CHECK(lib.size() == 2);
    auto found = lib.findBookById("B101");
    REQUIRE(found.has_value());
    CHECK(found->title == "The C++ Programming Language");
}

TEST_CASE("Checkout and return state transitions") {
    LibraryManager lib;
    lib.addBook({"B1", "Clean Code", "Robert C. Martin", "1234567890", "Software Engineering", false, ""});

    // Valid checkout
    CHECK(lib.checkoutBook("B1", "Alice") == true);
    auto bAfterCheckout = lib.findBookById("B1");
    REQUIRE(bAfterCheckout.has_value());
    CHECK(bAfterCheckout->isCheckedOut == true);
    CHECK(bAfterCheckout->borrowerName == "Alice");

    // Double checkout prevention
    CHECK(lib.checkoutBook("B1", "Bob") == false);

    // Return book
    CHECK(lib.returnBook("B1") == true);
    auto bAfterReturn = lib.findBookById("B1");
    REQUIRE(bAfterReturn.has_value());
    CHECK(bAfterReturn->isCheckedOut == false);
    CHECK(bAfterReturn->borrowerName.empty());

    // Double return prevention
    CHECK(lib.returnBook("B1") == false);
}

TEST_CASE("Search filtering") {
    LibraryManager lib;
    lib.addBook({"B1", "Design Patterns", "Gang of Four", "1111", "CS", false, ""});
    lib.addBook({"B2", "Refactoring", "Martin Fowler", "2222", "CS", false, ""});

    auto res = lib.search("Martin");
    REQUIRE(res.size() == 1);
    CHECK(res[0].title == "Refactoring");

    auto resCat = lib.search("CS");
    CHECK(resCat.size() == 2);
}

TEST_CASE("File persistence round-trip") {
    const std::string testFile = "temp_library_test.txt";

    LibraryManager libOut;
    libOut.addBook({"B10", "Structure and Interpretation of Computer Programs", "Abelson & Sussman", "3333", "CS", true, "Linus"});
    libOut.addBook({"B20", "Introduction to Algorithms", "CLRS", "4444", "CS", false, ""});

    CHECK(libOut.saveToFile(testFile) == true);

    LibraryManager libIn;
    CHECK(libIn.loadFromFile(testFile) == true);
    CHECK(libIn.size() == 2);

    auto b10 = libIn.findBookById("B10");
    REQUIRE(b10.has_value());
    CHECK(b10->isCheckedOut == true);
    CHECK(b10->borrowerName == "Linus");

    std::remove(testFile.c_str());
}
