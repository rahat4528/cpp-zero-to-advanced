#pragma once

#include <string>
#include <vector>
#include <optional>
#include <sqlite3.h>

namespace inventory {

struct ProductItem {
    int id{0};
    std::string sku;
    std::string name;
    std::string category;
    double unitPrice{0.0};
    int quantity{0};
    int reorderThreshold{10};
    std::string supplier;
};

struct StockTransaction {
    int id{0};
    int productId{0};
    std::string sku;
    std::string type; // "RESTOCK" or "DISPATCH"
    int quantityChanged{0};
    std::string timestamp;
    std::string remarks;
};

struct ValuationSummary {
    int totalDistinctProducts{0};
    int totalUnitsInStock{0};
    double totalValuation{0.0};
    int lowStockAlertCount{0};
};

class InventoryDatabase {
public:
    explicit InventoryDatabase(const std::string& dbPath);
    ~InventoryDatabase();

    bool open();
    void close();

    bool addProduct(const ProductItem& item);
    bool updateStock(const std::string& sku, int deltaQuantity, const std::string& type, const std::string& remarks);
    std::optional<ProductItem> getProductBySku(const std::string& sku);
    std::vector<ProductItem> getAllProducts();
    std::vector<ProductItem> getLowStockAlerts();
    ValuationSummary getValuationSummary();
    std::vector<StockTransaction> getTransactionHistory(const std::string& sku = "");

private:
    void initSchema();

    std::string m_dbPath;
    sqlite3* m_db{nullptr};
};

} // namespace inventory
