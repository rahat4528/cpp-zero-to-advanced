#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "InventoryManager.hpp"

TEST_CASE("InventoryDatabase - CRUD & Valuation Lifecycle") {
    // In-memory SQLite database
    inventory::InventoryDatabase db(":memory:");
    REQUIRE(db.open() == true);

    SUBCASE("Insert Products & Check Queries") {
        inventory::ProductItem item1{0, "SKU-A", "Resistor Kit", "Components", 10.0, 50, 15, "DigiKey"};
        inventory::ProductItem item2{0, "SKU-B", "Soldering Iron", "Tools", 35.0, 4, 10, "Weller"};

        CHECK(db.addProduct(item1) == true);
        CHECK(db.addProduct(item2) == true);

        auto fetched = db.getProductBySku("SKU-A");
        REQUIRE(fetched.has_value());
        CHECK(fetched->name == "Resistor Kit");
        CHECK(fetched->quantity == 50);

        auto all = db.getAllProducts();
        CHECK(all.size() == 2);
    }

    SUBCASE("Stock Dispatch & Threshold Alerts") {
        inventory::ProductItem item{0, "SKU-C", "3D Filament PLA", "Materials", 20.0, 12, 10, "Polymaker"};
        db.addProduct(item);

        // Dispatched 5 units -> remaining 7 (below threshold 10)
        CHECK(db.updateStock("SKU-C", -5, "DISPATCH", "Order #4812") == true);
        auto updated = db.getProductBySku("SKU-C");
        REQUIRE(updated.has_value());
        CHECK(updated->quantity == 7);

        auto alerts = db.getLowStockAlerts();
        REQUIRE(alerts.size() == 1);
        CHECK(alerts[0].sku == "SKU-C");

        // Negative stock prevention
        CHECK(db.updateStock("SKU-C", -10, "DISPATCH", "Should Fail") == false);
        CHECK(db.getProductBySku("SKU-C")->quantity == 7);
    }

    SUBCASE("Valuation Calculations") {
        inventory::ProductItem item1{0, "SKU-D", "Drill Bit Set", "Tools", 50.0, 10, 5, "Bosch"};
        inventory::ProductItem item2{0, "SKU-E", "Safety Glasses", "Safety", 5.0, 20, 10, "3M"};
        db.addProduct(item1);
        db.addProduct(item2);

        auto summary = db.getValuationSummary();
        CHECK(summary.totalDistinctProducts == 2);
        CHECK(summary.totalUnitsInStock == 30);
        // (10 * 50.0) + (20 * 5.0) = 500 + 100 = 600.0
        CHECK(summary.totalValuation == doctest::Approx(600.0));
    }
}
