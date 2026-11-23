#pragma once

#include <string>
#include <vector>
#include <stack>

namespace editor {

struct CursorPosition {
    int line{0};
    int column{0};
};

struct BufferSnapshot {
    std::vector<std::string> lines;
    CursorPosition cursor;
};

class TextBuffer {
public:
    TextBuffer();
    explicit TextBuffer(const std::string& initialContent);

    void insertText(const std::string& text);
    void insertLine(int lineIndex, const std::string& content);
    void deleteLine(int lineIndex);
    void setContent(const std::string& content);

    [[nodiscard]] std::string getContent() const;
    [[nodiscard]] const std::vector<std::string>& getLines() const;
    [[nodiscard]] size_t getLineCount() const;
    [[nodiscard]] size_t getCharacterCount() const;

    void setCursor(int line, int col);
    [[nodiscard]] CursorPosition getCursor() const;

    void undo();
    void redo();

private:
    void recordHistory();

    std::vector<std::string> m_lines;
    CursorPosition m_cursor{0, 0};
    std::stack<BufferSnapshot> m_undoStack;
    std::stack<BufferSnapshot> m_redoStack;
};

} // namespace editor
