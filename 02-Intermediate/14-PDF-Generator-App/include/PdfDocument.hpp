#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace pdf {

struct ColorRGB {
    float r{0.0f}; // [0.0 - 1.0]
    float g{0.0f};
    float b{0.0f};
};

struct TextElement {
    std::string text;
    float x{0.0f};
    float y{0.0f};
    float fontSize{12.0f};
    std::string fontName{"Helvetica"};
    ColorRGB color{0.0f, 0.0f, 0.0f};
};

struct RectElement {
    float x{0.0f};
    float y{0.0f};
    float width{0.0f};
    float height{0.0f};
    ColorRGB strokeColor{0.0f, 0.0f, 0.0f};
    ColorRGB fillColor{1.0f, 1.0f, 1.0f};
    bool fill{false};
    bool stroke{true};
    float lineWidth{1.0f};
};

struct LineElement {
    float x1{0.0f};
    float y1{0.0f};
    float x2{0.0f};
    float y2{0.0f};
    ColorRGB color{0.0f, 0.0f, 0.0f};
    float lineWidth{1.0f};
};

class PdfPage {
public:
    PdfPage(float width = 612.0f, float height = 792.0f); // Default US Letter

    float getWidth() const { return m_width; }
    float getHeight() const { return m_height; }

    void addText(const std::string& text, float x, float y, float fontSize = 12.0f,
                 const std::string& fontName = "Helvetica", ColorRGB color = {0, 0, 0});
    void addRect(float x, float y, float w, float h, ColorRGB stroke, ColorRGB fill, bool doFill = true, float lineWidth = 1.0f);
    void addLine(float x1, float y1, float x2, float y2, ColorRGB color, float lineWidth = 1.0f);

    [[nodiscard]] std::string buildContentStream() const;

private:
    float m_width;
    float m_height;
    std::vector<TextElement> m_texts;
    std::vector<RectElement> m_rects;
    std::vector<LineElement> m_lines;
};

class PdfDocument {
public:
    PdfDocument();

    int addPage(float width = 612.0f, float height = 792.0f);
    PdfPage& getPage(int index);
    [[nodiscard]] size_t getPageCount() const;

    bool saveToFile(const std::string& outputPath) const;
    [[nodiscard]] std::string serializeToString() const;

private:
    std::vector<PdfPage> m_pages;
};

} // namespace pdf
