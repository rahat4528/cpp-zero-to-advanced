#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "PdfDocument.hpp"
#include <filesystem>

TEST_CASE("PdfDocument - Page and Content Construction") {
    pdf::PdfDocument doc;
    CHECK(doc.getPageCount() == 0);

    int p0 = doc.addPage(595.0f, 842.0f); // A4
    CHECK(p0 == 0);
    CHECK(doc.getPageCount() == 1);

    auto& page = doc.getPage(0);
    CHECK(page.getWidth() == 595.0f);
    CHECK(page.getHeight() == 842.0f);

    page.addText("Hello C++ PDF", 50.0f, 750.0f, 14.0f);
    page.addRect(50.0f, 700.0f, 200.0f, 30.0f, {0, 0, 0}, {0.8f, 0.8f, 0.8f}, true);
    page.addLine(50.0f, 690.0f, 250.0f, 690.0f, {0, 0, 0}, 2.0f);

    std::string stream = page.buildContentStream();
    CHECK(stream.find("BT") != std::string::npos);
    CHECK(stream.find("(Hello C++ PDF) Tj") != std::string::npos);
    CHECK(stream.find("ET") != std::string::npos);
    CHECK(stream.find("re B") != std::string::npos); // rectangle both fill and stroke
    CHECK(stream.find("m 250.00 690.00 l S") != std::string::npos);
}

TEST_CASE("PdfDocument - PDF 1.4 Specification Compliance") {
    pdf::PdfDocument doc;
    int p = doc.addPage(612.0f, 792.0f);
    doc.getPage(p).addText("Specification Test", 100, 700);

    std::string rawPdf = doc.serializeToString();
    REQUIRE(!rawPdf.empty());

    // Check Header
    CHECK(rawPdf.rfind("%PDF-1.4", 0) == 0);

    // Check Catalog and Pages tree
    CHECK(rawPdf.find("/Type /Catalog") != std::string::npos);
    CHECK(rawPdf.find("/Type /Pages") != std::string::npos);
    CHECK(rawPdf.find("/Type /Page") != std::string::npos);
    CHECK(rawPdf.find("/Type /Font") != std::string::npos);

    // Check Cross-Reference table and trailer
    CHECK(rawPdf.find("xref\n0 ") != std::string::npos);
    CHECK(rawPdf.find("trailer\n<< /Size ") != std::string::npos);
    CHECK(rawPdf.find("startxref") != std::string::npos);
    CHECK(rawPdf.find("%%EOF") != std::string::npos);
}

TEST_CASE("PdfDocument - File Save and Cleanup") {
    pdf::PdfDocument doc;
    doc.addPage();
    doc.getPage(0).addText("File Output Test", 100, 700);

    std::string path = "temp_test_output.pdf";
    CHECK(doc.saveToFile(path) == true);
    CHECK(std::filesystem::exists(path));
    CHECK(std::filesystem::file_size(path) > 100);

    std::filesystem::remove(path);
}
