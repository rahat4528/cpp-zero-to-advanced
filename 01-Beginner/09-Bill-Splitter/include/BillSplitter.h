#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

namespace bill_splitter {

struct BillSummary {
    double subtotal{0.0};
    double taxRatePct{0.0};
    double taxAmount{0.0};
    double tipRatePct{0.0};
    double tipAmount{0.0};
    double grandTotal{0.0};
    size_t peopleCount{1};
    double perPersonBase{0.0};
    // If penny remainder exists, individual exact amounts in cents
    std::vector<double> individualShares;
};

struct Item {
    std::string name;
    double price{0.0};
    std::vector<std::string> sharedBy; // Names of people sharing this item
};

struct ItemizedSummary {
    double subtotal{0.0};
    double grandTotal{0.0};
    std::map<std::string, double> personOwes; // Person name -> amount owed
};

// Calculates an equal split with penny-exact remainder distribution
std::optional<BillSummary> calculateEqualSplit(double subtotal,
                                              size_t peopleCount,
                                              double tipRatePct = 15.0,
                                              double taxRatePct = 8.5);

// Calculates an itemized split across designated participants
std::optional<ItemizedSummary> calculateItemizedSplit(const std::vector<Item>& items,
                                                    double tipRatePct = 15.0,
                                                    double taxRatePct = 8.5);

// Parses itemized string: "Pizza:24.0:Alice,Bob;Salad:12.0:Bob;Drink:6.0:Alice"
std::vector<Item> parseItemizedString(const std::string& input);

} // namespace bill_splitter
