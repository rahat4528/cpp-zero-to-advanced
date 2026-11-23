#include "InventoryManager.hpp"
#include <iostream>
#include <iomanip>
#include <string>

void printTable(const std::vector<inventory::ProductItem>& items) {
    std::cout << "====================================================================================================\n";
    std::cout << std::left << std::setw(12) << "SKU"
              << std::setw(25) << "PRODUCT NAME"
              << std::setw(18) << "CATEGORY"
              << std::setw(12) << "PRICE ($)"
              << std::setw(10) << "STOCK"
              << std::setw(10) << "REORDER"
              << std::setw(15) << "SUPPLIER" << "\n";
    std::cout << "----------------------------------------------------------------------------------------------------\n";
    for (const auto& item : items) {
        std::cout << std::left << std::setw(12) << item.sku
                  << std::setw(25) << item.name.substr(0, 24)
                  << std::setw(18) << item.category.substr(0, 17)
                  << std::fixed << std::setprecision(2) << std::setw(12) << item.unitPrice
                  << std::setw(10) << item.quantity
                  << std::setw(10) << item.reorderThreshold
                  << std::setw(15) << item.supplier << "\n";
    }
    std::cout << "====================================================================================================\n";
}

int main(int argc, char* argv[]) {
    std::string dbFile = "inventory.db";

    bool testRun = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--test-run") {
            testRun = true;
            dbFile = ":memory:";
        } else if (std::string(argv[i]) == "--db" && i + 1 < argc) {
            dbFile = argv[++i];
        }
    }

    inventory::InventoryDatabase db(dbFile);
    if (!db.open()) {
        std::cerr << "Failed to open inventory database.\n";
        return 1;
    }

    if (testRun) {
        std::cout << "[Test-Run Mode] Populating test inventory and checking metrics...\n";
        db.addProduct({0, "SKU-1001", "Precision Caliper", "Tools", 45.50, 15, 5, "Mitutoyo"});
        db.addProduct({0, "SKU-1002", "Microcontroller Board", "Electronics", 12.00, 3, 10, "Adafruit"});
        db.addProduct({0, "SKU-1003", "USB-C PD Cable 2m", "Accessories", 8.50, 2, 8, "Anker"});

        auto summary = db.getValuationSummary();
        std::cout << "Distinct Products: " << summary.totalDistinctProducts << "\n";
        std::cout << "Total Units      : " << summary.totalUnitsInStock << "\n";
        std::cout << "Total Valuation  : $" << summary.totalValuation << "\n";
        std::cout << "Low Stock Alerts : " << summary.lowStockAlertCount << "\n";

        std::cout << "\nLow Stock Alert Items:\n";
        printTable(db.getLowStockAlerts());
        return 0;
    }

    std::cout << "=================================================================\n";
    std::cout << "              WAREHOUSE INVENTORY MANAGER (SQLite)               \n";
    std::cout << "=================================================================\n";

    auto summary = db.getValuationSummary();
    std::cout << "Database: " << dbFile << "\n";
    std::cout << "Products: " << summary.totalDistinctProducts
              << " | Total Units: " << summary.totalUnitsInStock
              << " | Valuation: $" << std::fixed << std::setprecision(2) << summary.totalValuation
              << " | Low Stock Alerts: " << summary.lowStockAlertCount << "\n\n";

    auto items = db.getAllProducts();
    if (items.empty()) {
        std::cout << "Inventory is empty. Use command line or automated test-run to populate.\n";
    } else {
        printTable(items);
    }

    return 0;
}
