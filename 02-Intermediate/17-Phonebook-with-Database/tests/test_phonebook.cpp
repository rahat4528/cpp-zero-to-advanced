#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "PhonebookDatabase.hpp"

TEST_CASE("PhonebookDatabase - CRUD and Queries") {
    phonebook::PhonebookDatabase db(":memory:");
    REQUIRE(db.open() == true);

    SUBCASE("Insert and Retrieve Contacts") {
        phonebook::ContactRecord c1{0, "Nikola", "Tesla", "+1-555-1856", "tesla@wardenclyffe.org", "Tesla Electric", "AC motor", true};
        int id1 = db.addContact(c1);
        REQUIRE(id1 > 0);

        auto fetched = db.getContact(id1);
        REQUIRE(fetched.has_value());
        CHECK(fetched->firstName == "Nikola");
        CHECK(fetched->lastName == "Tesla");
        CHECK(fetched->isFavorite == true);
    }

    SUBCASE("Update and Delete Contacts") {
        phonebook::ContactRecord c{0, "Thomas", "Edison", "+1-555-1847", "edison@ge.com", "General Electric", "DC power", false};
        int id = db.addContact(c);
        c.id = id;
        c.notes = "Phonograph and incandescent bulb";
        c.isFavorite = true;
        CHECK(db.updateContact(c) == true);

        auto updated = db.getContact(id);
        REQUIRE(updated.has_value());
        CHECK(updated->notes == "Phonograph and incandescent bulb");
        CHECK(updated->isFavorite == true);

        CHECK(db.deleteContact(id) == true);
        CHECK(db.getContact(id).has_value() == false);
    }

    SUBCASE("Search and Favorites Filter") {
        db.addContact({0, "Grace", "Hopper", "+1-555-1906", "grace@navy.mil", "US Navy", "COBOL", true});
        db.addContact({0, "Margaret", "Hamilton", "+1-555-1936", "margaret@mit.edu", "NASA MIT", "Apollo guidance", true});
        db.addContact({0, "Dennis", "Ritchie", "+1-555-1941", "dmr@bell-labs.com", "Bell Labs", "C creator", false});

        auto byName = db.searchByName("Hopper");
        CHECK(byName.size() == 1);
        CHECK(byName[0].firstName == "Grace");

        auto byPhone = db.searchByPhone("1936");
        CHECK(byPhone.size() == 1);
        CHECK(byPhone[0].firstName == "Margaret");

        auto favs = db.getFavorites();
        CHECK(favs.size() == 2);
    }

    SUBCASE("vCard Export and Import") {
        phonebook::ContactRecord original{0, "Linus", "Torvalds", "+1-555-1969", "torvalds@kernel.org", "Linux Foundation", "Git creator", true};
        int id = db.addContact(original);

        std::string vcf = db.exportToVCard(id);
        CHECK(vcf.find("BEGIN:VCARD") != std::string::npos);
        CHECK(vcf.find("FN:Linus Torvalds") != std::string::npos);
        CHECK(vcf.find("+1-555-1969") != std::string::npos);

        // Import into clean database
        phonebook::PhonebookDatabase db2(":memory:");
        db2.open();
        CHECK(db2.importFromVCard(vcf) == true);
        auto importedList = db2.searchByName("Torvalds");
        REQUIRE(importedList.size() == 1);
        CHECK(importedList[0].phoneNumber == "+1-555-1969");
        CHECK(importedList[0].email == "torvalds@kernel.org");
    }
}
