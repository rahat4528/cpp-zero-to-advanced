#include "InventoryManager.hpp"
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace inventory {

InventoryDatabase::InventoryDatabase(const std::string& dbPath) : m_dbPath(dbPath) {}

InventoryDatabase::~InventoryDatabase() {
    close();
}

bool InventoryDatabase::open() {
    if (m_db) return true;
    if (sqlite3_open(m_dbPath.c_str(), &m_db) != SQLITE_OK) {
        std::cerr << "SQLite open error: " << sqlite3_errmsg(m_db) << "\n";
        m_db = nullptr;
        return false;
    }
    initSchema();
    return true;
}

void InventoryDatabase::close() {
    if (m_db) {
        sqlite3_close(m_db);
        m_db = nullptr;
    }
}

void InventoryDatabase::initSchema() {
    const char* schema = R"(
        CREATE TABLE IF NOT EXISTS products (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            sku TEXT UNIQUE NOT NULL,
            name TEXT NOT NULL,
            category TEXT NOT NULL,
            unit_price REAL NOT NULL,
            quantity INTEGER NOT NULL DEFAULT 0,
            reorder_threshold INTEGER NOT NULL DEFAULT 10,
            supplier TEXT NOT NULL
        );
        CREATE TABLE IF NOT EXISTS transactions (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            product_id INTEGER NOT NULL,
            sku TEXT NOT NULL,
            trans_type TEXT NOT NULL,
            quantity_changed INTEGER NOT NULL,
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            remarks TEXT,
            FOREIGN KEY(product_id) REFERENCES products(id)
        );
        CREATE INDEX IF NOT EXISTS idx_products_sku ON products(sku);
    )";

    char* err = nullptr;
    if (sqlite3_exec(m_db, schema, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string errStr = err ? err : "Unknown schema error";
        sqlite3_free(err);
        throw std::runtime_error("Failed to initialize inventory schema: " + errStr);
    }
}

bool InventoryDatabase::addProduct(const ProductItem& item) {
    if (!m_db) return false;

    const char* sql = "INSERT INTO products (sku, name, category, unit_price, quantity, reorder_threshold, supplier) "
                      "VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    sqlite3_bind_text(stmt, 1, item.sku.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, item.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, item.category.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, item.unitPrice);
    sqlite3_bind_int(stmt, 5, item.quantity);
    sqlite3_bind_int(stmt, 6, item.reorderThreshold);
    sqlite3_bind_text(stmt, 7, item.supplier.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    if (ok && item.quantity > 0) {
        auto prod = getProductBySku(item.sku);
        if (prod) {
            const char* logSql = "INSERT INTO transactions (product_id, sku, trans_type, quantity_changed, remarks) "
                                 "VALUES (?, ?, 'INITIAL_STOCK', ?, 'Opening inventory');";
            sqlite3_stmt* logStmt = nullptr;
            if (sqlite3_prepare_v2(m_db, logSql, -1, &logStmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(logStmt, 1, prod->id);
                sqlite3_bind_text(logStmt, 2, item.sku.c_str(), -1, SQLITE_TRANSIENT);
                sqlite3_bind_int(logStmt, 3, item.quantity);
                sqlite3_step(logStmt);
                sqlite3_finalize(logStmt);
            }
        }
    }
    return ok;
}

bool InventoryDatabase::updateStock(const std::string& sku, int deltaQuantity, const std::string& type, const std::string& remarks) {
    if (!m_db) return false;

    auto prod = getProductBySku(sku);
    if (!prod) return false;

    int newQty = prod->quantity + deltaQuantity;
    if (newQty < 0) {
        return false; // Cannot dispatch more than available
    }

    // Begin transaction
    sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr);

    const char* updateSql = "UPDATE products SET quantity = ? WHERE sku = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, updateSql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_int(stmt, 1, newQty);
    sqlite3_bind_text(stmt, 2, sku.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }
    sqlite3_finalize(stmt);

    const char* logSql = "INSERT INTO transactions (product_id, sku, trans_type, quantity_changed, remarks) "
                         "VALUES (?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(m_db, logSql, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }

    sqlite3_bind_int(stmt, 1, prod->id);
    sqlite3_bind_text(stmt, 2, sku.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, deltaQuantity);
    sqlite3_bind_text(stmt, 5, remarks.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }
    sqlite3_finalize(stmt);

    sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, nullptr);
    return true;
}

std::optional<ProductItem> InventoryDatabase::getProductBySku(const std::string& sku) {
    if (!m_db) return std::nullopt;

    const char* sql = "SELECT id, sku, name, category, unit_price, quantity, reorder_threshold, supplier "
                      "FROM products WHERE sku = ?;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return std::nullopt;
    }

    sqlite3_bind_text(stmt, 1, sku.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        ProductItem item;
        item.id = sqlite3_column_int(stmt, 0);
        item.sku = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        item.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        item.category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        item.unitPrice = sqlite3_column_double(stmt, 4);
        item.quantity = sqlite3_column_int(stmt, 5);
        item.reorderThreshold = sqlite3_column_int(stmt, 6);
        item.supplier = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));

        sqlite3_finalize(stmt);
        return item;
    }

    sqlite3_finalize(stmt);
    return std::nullopt;
}

std::vector<ProductItem> InventoryDatabase::getAllProducts() {
    std::vector<ProductItem> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, sku, name, category, unit_price, quantity, reorder_threshold, supplier FROM products ORDER BY sku;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return list;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ProductItem item;
        item.id = sqlite3_column_int(stmt, 0);
        item.sku = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        item.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        item.category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        item.unitPrice = sqlite3_column_double(stmt, 4);
        item.quantity = sqlite3_column_int(stmt, 5);
        item.reorderThreshold = sqlite3_column_int(stmt, 6);
        item.supplier = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        list.push_back(item);
    }
    sqlite3_finalize(stmt);
    return list;
}

std::vector<ProductItem> InventoryDatabase::getLowStockAlerts() {
    std::vector<ProductItem> list;
    if (!m_db) return list;

    const char* sql = "SELECT id, sku, name, category, unit_price, quantity, reorder_threshold, supplier "
                      "FROM products WHERE quantity <= reorder_threshold ORDER BY quantity ASC;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        return list;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        ProductItem item;
        item.id = sqlite3_column_int(stmt, 0);
        item.sku = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        item.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        item.category = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        item.unitPrice = sqlite3_column_double(stmt, 4);
        item.quantity = sqlite3_column_int(stmt, 5);
        item.reorderThreshold = sqlite3_column_int(stmt, 6);
        item.supplier = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        list.push_back(item);
    }
    sqlite3_finalize(stmt);
    return list;
}

ValuationSummary InventoryDatabase::getValuationSummary() {
    ValuationSummary summary;
    if (!m_db) return summary;

    const char* sql = "SELECT COUNT(*), COALESCE(SUM(quantity), 0), COALESCE(SUM(quantity * unit_price), 0.0) FROM products;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            summary.totalDistinctProducts = sqlite3_column_int(stmt, 0);
            summary.totalUnitsInStock = sqlite3_column_int(stmt, 1);
            summary.totalValuation = sqlite3_column_double(stmt, 2);
        }
        sqlite3_finalize(stmt);
    }

    summary.lowStockAlertCount = static_cast<int>(getLowStockAlerts().size());
    return summary;
}

std::vector<StockTransaction> InventoryDatabase::getTransactionHistory(const std::string& sku) {
    std::vector<StockTransaction> history;
    if (!m_db) return history;

    std::string sql = "SELECT id, product_id, sku, trans_type, quantity_changed, timestamp, remarks FROM transactions ";
    if (!sku.empty()) {
        sql += "WHERE sku = ? ";
    }
    sql += "ORDER BY id DESC;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return history;
    }

    if (!sku.empty()) {
        sqlite3_bind_text(stmt, 1, sku.c_str(), -1, SQLITE_TRANSIENT);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        StockTransaction tx;
        tx.id = sqlite3_column_int(stmt, 0);
        tx.productId = sqlite3_column_int(stmt, 1);
        tx.sku = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        tx.type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        tx.quantityChanged = sqlite3_column_int(stmt, 4);
        tx.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        const auto* remarks = sqlite3_column_text(stmt, 6);
        tx.remarks = remarks ? reinterpret_cast<const char*>(remarks) : "";
        history.push_back(tx);
    }
    sqlite3_finalize(stmt);
    return history;
}

} // namespace inventory
