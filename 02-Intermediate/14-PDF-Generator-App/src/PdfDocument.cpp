#include "PdfDocument.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace pdf {

PdfPage::PdfPage(float width, float height) : m_width(width), m_height(height) {}

void PdfPage::addText(const std::string& text, float x, float y, float fontSize,
                     const std::string& fontName, ColorRGB color) {
    m_texts.push_back({text, x, y, fontSize, fontName, color});
}

void PdfPage::addRect(float x, float y, float w, float h, ColorRGB stroke, ColorRGB fill, bool doFill, float lineWidth) {
    m_rects.push_back({x, y, w, h, stroke, fill, doFill, true, lineWidth});
}

void PdfPage::addLine(float x1, float y1, float x2, float y2, ColorRGB color, float lineWidth) {
    m_lines.push_back({x1, y1, x2, y2, color, lineWidth});
}

std::string PdfPage::buildContentStream() const {
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(2);

    // Draw rectangles
    for (const auto& r : m_rects) {
        ss << r.lineWidth << " w\n";
        ss << r.strokeColor.r << " " << r.strokeColor.g << " " << r.strokeColor.b << " RG\n";
        if (r.fill) {
            ss << r.fillColor.r << " " << r.fillColor.g << " " << r.fillColor.b << " rg\n";
            ss << r.x << " " << r.y << " " << r.width << " " << r.height << " re B\n";
        } else {
            ss << r.x << " " << r.y << " " << r.width << " " << r.height << " re S\n";
        }
    }

    // Draw lines
    for (const auto& l : m_lines) {
        ss << l.lineWidth << " w\n";
        ss << l.color.r << " " << l.color.g << " " << l.color.b << " RG\n";
        ss << l.x1 << " " << l.y1 << " m " << l.x2 << " " << l.y2 << " l S\n";
    }

    // Draw text elements
    for (const auto& t : m_texts) {
        ss << "BT\n";
        ss << "/F1 " << t.fontSize << " Tf\n";
        ss << t.color.r << " " << t.color.g << " " << t.color.b << " rg\n";
        ss << t.x << " " << t.y << " Td\n";
        // Escape parentheses in PDF string literal
        std::string escaped;
        for (char c : t.text) {
            if (c == '(' || c == ')' || c == '\\') escaped += '\\';
            escaped += c;
        }
        ss << "(" << escaped << ") Tj\n";
        ss << "ET\n";
    }

    return ss.str();
}

PdfDocument::PdfDocument() = default;

int PdfDocument::addPage(float width, float height) {
    m_pages.emplace_back(width, height);
    return static_cast<int>(m_pages.size() - 1);
}

PdfPage& PdfDocument::getPage(int index) {
    if (index < 0 || index >= static_cast<int>(m_pages.size())) {
        throw std::out_of_range("Invalid page index");
    }
    return m_pages[index];
}

size_t PdfDocument::getPageCount() const {
    return m_pages.size();
}

std::string PdfDocument::serializeToString() const {
    if (m_pages.empty()) return "";

    std::ostringstream pdf;
    std::vector<size_t> offsets;
    offsets.push_back(0); // 0-th object in xref is dummy

    pdf << "%PDF-1.4\n";
    pdf << "%\xE2\xE3\xCF\xD3\n"; // Binary header comment

    // Object 1: Catalog
    offsets.push_back(pdf.tellp());
    pdf << "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n";

    // Object 2: Pages tree
    offsets.push_back(pdf.tellp());
    pdf << "2 0 obj\n<< /Type /Pages /Kids [";
    // Each page will be object (3 + i * 2)
    for (size_t i = 0; i < m_pages.size(); ++i) {
        pdf << " " << (3 + i * 2) << " 0 R";
    }
    pdf << " ] /Count " << m_pages.size() << " >>\nendobj\n";

    // Object for Helvetica font (shared by pages)
    // We'll put font as the last object: fontObjNum = 3 + pages.size() * 2
    int fontObjNum = static_cast<int>(3 + m_pages.size() * 2);

    for (size_t i = 0; i < m_pages.size(); ++i) {
        int pageObjNum = static_cast<int>(3 + i * 2);
        int contentObjNum = pageObjNum + 1;

        std::string stream = m_pages[i].buildContentStream();

        // Page Object
        offsets.push_back(pdf.tellp());
        pdf << pageObjNum << " 0 obj\n<< /Type /Page /Parent 2 0 R /MediaBox [0 0 "
            << m_pages[i].getWidth() << " " << m_pages[i].getHeight() << "]"
            << " /Contents " << contentObjNum << " 0 R"
            << " /Resources << /Font << /F1 " << fontObjNum << " 0 R >> >> >>\nendobj\n";

        // Contents Stream Object
        offsets.push_back(pdf.tellp());
        pdf << contentObjNum << " 0 obj\n<< /Length " << stream.length() << " >>\nstream\n"
            << stream << "endstream\nendobj\n";
    }

    // Font Object
    offsets.push_back(pdf.tellp());
    pdf << fontObjNum << " 0 obj\n<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>\nendobj\n";

    // Cross-Reference Table
    size_t startXref = pdf.tellp();
    size_t totalObjs = offsets.size();

    pdf << "xref\n0 " << totalObjs << "\n";
    pdf << "0000000000 65535 f \n";
    for (size_t i = 1; i < totalObjs; ++i) {
        pdf << std::setw(10) << std::setfill('0') << offsets[i] << " 00000 n \n";
    }

    // Trailer
    pdf << "trailer\n<< /Size " << totalObjs << " /Root 1 0 R >>\n";
    pdf << "startxref\n" << startXref << "\n%%EOF\n";

    return pdf.str();
}

bool PdfDocument::saveToFile(const std::string& outputPath) const {
    std::string data = serializeToString();
    if (data.empty()) return false;

    std::ofstream out(outputPath, std::ios::binary);
    if (!out.is_open()) return false;

    out.write(data.data(), data.length());
    return out.good();
}

} // namespace pdf
