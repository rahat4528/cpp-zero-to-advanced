#include "BillSplitter.h"

#include <cmath>
#include <cstdint>
#include <sstream>

namespace bill_splitter {

// Helper to round dollars to whole cents safely
static int64_t toCents(double dollars) {
    return static_cast<int64_t>(std::round(dollars * 100.0));
}

static double toDollars(int64_t cents) {
    return static_cast<double>(cents) / 100.0;
}

std::optional<BillSummary> calculateEqualSplit(double subtotal,
                                              size_t peopleCount,
                                              double tipRatePct,
                                              double taxRatePct) {
    if (subtotal < 0.0 || peopleCount == 0 || tipRatePct < 0.0 || taxRatePct < 0.0) {
        return std::nullopt;
    }

    int64_t subtotalCents = toCents(subtotal);
    int64_t taxCents = toCents(subtotal * (taxRatePct / 100.0));
    int64_t tipCents = toCents(subtotal * (tipRatePct / 100.0));
    int64_t grandTotalCents = subtotalCents + taxCents + tipCents;

    int64_t baseShareCents = grandTotalCents / static_cast<int64_t>(peopleCount);
    int64_t remainderCents = grandTotalCents % static_cast<int64_t>(peopleCount);

    BillSummary summary;
    summary.subtotal = toDollars(subtotalCents);
    summary.taxRatePct = taxRatePct;
    summary.taxAmount = toDollars(taxCents);
    summary.tipRatePct = tipRatePct;
    summary.tipAmount = toDollars(tipCents);
    summary.grandTotal = toDollars(grandTotalCents);
    summary.peopleCount = peopleCount;
    summary.perPersonBase = toDollars(baseShareCents);

    summary.individualShares.resize(peopleCount, toDollars(baseShareCents));
    // Fairly distribute remaining pennies
    for (int64_t i = 0; i < remainderCents; ++i) {
        summary.individualShares[i] += 0.01;
    }

    return summary;
}

std::vector<Item> parseItemizedString(const std::string& input) {
    std::vector<Item> items;
    std::istringstream iss(input);
    std::string itemToken;

    while (std::getline(iss, itemToken, ';')) {
        if (itemToken.empty()) continue;
        std::istringstream itemStream(itemToken);
        Item item;
        std::string priceStr;
        std::string peopleStr;

        if (std::getline(itemStream, item.name, ':') &&
            std::getline(itemStream, priceStr, ':') &&
            std::getline(itemStream, peopleStr)) {
            try {
                item.price = std::stod(priceStr);
                std::istringstream peopleStream(peopleStr);
                std::string person;
                while (std::getline(peopleStream, person, ',')) {
                    if (!person.empty()) {
                        item.sharedBy.push_back(person);
                    }
                }
                if (!item.sharedBy.empty() && item.price >= 0.0) {
                    items.push_back(item);
                }
            } catch (...) {
                // Ignore malformed token
            }
        }
    }
    return items;
}

std::optional<ItemizedSummary> calculateItemizedSplit(const std::vector<Item>& items,
                                                    double tipRatePct,
                                                    double taxRatePct) {
    if (items.empty() || tipRatePct < 0.0 || taxRatePct < 0.0) {
        return std::nullopt;
    }

    double subtotal = 0.0;
    std::map<std::string, double> baseOwes;

    for (const auto& item : items) {
        if (item.sharedBy.empty()) return std::nullopt;
        subtotal += item.price;
        double splitPrice = item.price / static_cast<double>(item.sharedBy.size());
        for (const auto& p : item.sharedBy) {
            baseOwes[p] += splitPrice;
        }
    }

    double multiplier = 1.0 + (taxRatePct / 100.0) + (tipRatePct / 100.0);
    ItemizedSummary result;
    result.subtotal = subtotal;
    result.grandTotal = subtotal * multiplier;

    for (const auto& [person, amount] : baseOwes) {
        result.personOwes[person] = toDollars(toCents(amount * multiplier));
    }

    return result;
}

} // namespace bill_splitter
