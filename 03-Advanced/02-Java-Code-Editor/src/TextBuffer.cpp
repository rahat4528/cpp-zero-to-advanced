#include "TextBuffer.hpp"
#include <sstream>
#include <algorithm>

namespace editor {

TextBuffer::TextBuffer() {
    m_lines.push_back("");
}

TextBuffer::TextBuffer(const std::string& initialContent) {
    setContent(initialContent);
}

void TextBuffer::recordHistory() {
    m_undoStack.push({m_lines, m_cursor});
    while (!m_redoStack.empty()) m_redoStack.pop();
}

void TextBuffer::setContent(const std::string& content) {
    recordHistory();
    m_lines.clear();
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        m_lines.push_back(line);
    }
    if (m_lines.empty()) m_lines.push_back("");
    m_cursor = {0, 0};
}

void TextBuffer::insertLine(int lineIndex, const std::string& content) {
    recordHistory();
    if (lineIndex < 0 || lineIndex > static_cast<int>(m_lines.size())) {
        m_lines.push_back(content);
    } else {
        m_lines.insert(m_lines.begin() + lineIndex, content);
    }
}

void TextBuffer::deleteLine(int lineIndex) {
    if (lineIndex >= 0 && lineIndex < static_cast<int>(m_lines.size())) {
        recordHistory();
        m_lines.erase(m_lines.begin() + lineIndex);
        if (m_lines.empty()) m_lines.push_back("");
    }
}

void TextBuffer::insertText(const std::string& text) {
    recordHistory();
    if (m_lines.empty()) m_lines.push_back("");

    int l = std::clamp(m_cursor.line, 0, static_cast<int>(m_lines.size() - 1));
    int c = std::clamp(m_cursor.column, 0, static_cast<int>(m_lines[l].length()));

    m_lines[l].insert(c, text);
    m_cursor.column += static_cast<int>(text.length());
}

std::string TextBuffer::getContent() const {
    std::ostringstream oss;
    for (size_t i = 0; i < m_lines.size(); ++i) {
        oss << m_lines[i];
        if (i + 1 < m_lines.size()) oss << "\n";
    }
    return oss.str();
}

const std::vector<std::string>& TextBuffer::getLines() const {
    return m_lines;
}

size_t TextBuffer::getLineCount() const {
    return m_lines.size();
}

size_t TextBuffer::getCharacterCount() const {
    size_t total = 0;
    for (const auto& l : m_lines) total += l.length();
    return total;
}

void TextBuffer::setCursor(int line, int col) {
    m_cursor.line = std::clamp(line, 0, static_cast<int>(m_lines.size() - 1));
    m_cursor.column = std::clamp(col, 0, static_cast<int>(m_lines[m_cursor.line].length()));
}

CursorPosition TextBuffer::getCursor() const {
    return m_cursor;
}

void TextBuffer::undo() {
    if (m_undoStack.empty()) return;
    m_redoStack.push({m_lines, m_cursor});
    auto snap = m_undoStack.top();
    m_undoStack.pop();
    m_lines = snap.lines;
    m_cursor = snap.cursor;
}

void TextBuffer::redo() {
    if (m_redoStack.empty()) return;
    m_undoStack.push({m_lines, m_cursor});
    auto snap = m_redoStack.top();
    m_redoStack.pop();
    m_lines = snap.lines;
    m_cursor = snap.cursor;
}

} // namespace editor
