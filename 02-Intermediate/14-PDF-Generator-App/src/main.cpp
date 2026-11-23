#include "PdfDocument.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc >= 2 && std::string(argv[1]) == "--test-run") {
        std::cout << "=================================================================\n";
        std::cout << "                PDF DOCUMENT GENERATOR ENGINE                    \n";
        std::cout << "=================================================================\n";
        std::cout << "[Test-Run Mode] Generating sample PDF document (Executive Invoice)...\n";

        pdf::PdfDocument doc;
        int pageIdx = doc.addPage(612.0f, 792.0f); // US Letter
        auto& page = doc.getPage(pageIdx);

        // Header Background Banner
        page.addRect(40, 720, 532, 40, {0.1f, 0.3f, 0.6f}, {0.15f, 0.35f, 0.7f}, true);

        // Header Title
        page.addText("INVOICE / RECEIPT STATEMENT", 60, 735, 18, "Helvetica", {1.0f, 1.0f, 1.0f});

        // Billed To Info
        page.addText("Billed To: ACME Corporation", 50, 680, 12, "Helvetica", {0.1f, 0.1f, 0.1f});
        page.addText("Date: 2026-11-23", 450, 680, 12, "Helvetica", {0.3f, 0.3f, 0.3f});
        page.addText("Invoice #: INV-2026-9042", 450, 665, 12, "Helvetica", {0.3f, 0.3f, 0.3f});

        // Dividing Line
        page.addLine(50, 640, 562, 640, {0.8f, 0.8f, 0.8f}, 1.5f);

        // Table Header
        page.addRect(50, 605, 512, 25, {0.9f, 0.9f, 0.9f}, {0.92f, 0.92f, 0.95f}, true);
        page.addText("ITEM DESCRIPTION", 60, 612, 10, "Helvetica", {0.2f, 0.2f, 0.2f});
        page.addText("QTY", 350, 612, 10, "Helvetica", {0.2f, 0.2f, 0.2f});
        page.addText("RATE", 420, 612, 10, "Helvetica", {0.2f, 0.2f, 0.2f});
        page.addText("AMOUNT", 490, 612, 10, "Helvetica", {0.2f, 0.2f, 0.2f});

        // Items
        page.addText("High-Performance C++ Architecture Consulting", 60, 580, 10, "Helvetica");
        page.addText("40 hrs", 350, 580, 10, "Helvetica");
        page.addText("$150.00", 420, 580, 10, "Helvetica");
        page.addText("$6,000.00", 490, 580, 10, "Helvetica");

        page.addText("Low-Latency Socket Optimization Review", 60, 560, 10, "Helvetica");
        page.addText("20 hrs", 350, 560, 10, "Helvetica");
        page.addText("$175.00", 420, 560, 10, "Helvetica");
        page.addText("$3,500.00", 490, 560, 10, "Helvetica");

        // Total
        page.addLine(340, 530, 562, 530, {0.5f, 0.5f, 0.5f}, 1.0f);
        page.addText("TOTAL DUE: $9,500.00", 420, 510, 12, "Helvetica", {0.1f, 0.5f, 0.1f});

        std::string filename = "sample_invoice.pdf";
        if (doc.saveToFile(filename)) {
            std::cout << "Generated valid PDF 1.4 document: " << filename << "\n";
            std::cout << "[Test-Run Mode] Completed successfully.\n";
            return 0;
        } else {
            std::cerr << "Failed to save PDF file.\n";
            return 1;
        }
    }

    std::cout << "Usage: PDFGeneratorApp --test-run\n";
    return 0;
}
