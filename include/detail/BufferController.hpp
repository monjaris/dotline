#pragma once
#include "TerminalController.hpp"


DOTL_NAMESPACE_BEGIN(dotl)

class BufferController {
    std::u32string m_buffer = {};  // internal buffer
    usize m_beg = {0};  // beginning of the stream, prompt's end position
    usize m_pos = {0};  // the position of the vcursor between `m_beg` and `m_beg+m_len`

    // Redraws the line from buffer index `from` to the end, reflecting m_buffer's
    // current content, assuming the terminal's physical cursor is currently sitting
    // at the column corresponding to `from`. Repositions the cursor back to m_pos
    // once done. This is what makes insert/delete visually correct instead of
    // just overwriting whatever was already on screen.
    void _redraw(usize from) {
        std::string tail(m_buffer.begin() + from, m_buffer.end());
        write_sv("\033[K");   // clear from cursor to end of line
        write_sv(tail);       // print the up-to-date tail

        usize move_back = tail.size() - (m_pos - from);
        if (move_back > 0) {
            char buf[32];
            char* ptr = buf;
            *ptr++ = '\033';
            *ptr++ = '[';
            auto to_chars = std::to_chars(ptr, buf+sizeof(buf)-1, move_back);
            if (to_chars.ec != std::errc{}) return;
            ptr = to_chars.ptr;
            *ptr++ = 'D';
            *ptr = '\0';
            write_sv(buf);
        }
    }

public:
    // disable copying as construction
    BufferController (const BufferController&) = delete;
    // default ctor
    BufferController () { m_buffer.reserve(128); };


    enum Dir : unsigned char {
        Up='A', Down='B', Right='C', Left='D'
    };

    usize length() { return m_buffer.size(); }
    std::string string() { return std::string(m_buffer.begin(), m_buffer.end()); }
    void clear() { m_buffer.clear(); m_pos = 0; }

    usize pos() { return m_pos; }
    void setPos(usize pos) { m_pos = pos; }
    void setPosRel(usize pos) { m_pos += pos; }

    // Write characters and increment axis index
    void insert(char c) {
        usize at = m_pos;
        m_buffer.insert(m_buffer.begin() + at, c);
        m_pos = at + 1;
        _redraw(at);
    }
    usize insert(std::string_view cstr) {
        usize at = m_pos;
        m_buffer.insert(m_buffer.begin() + at, cstr.begin(), cstr.end());
        m_pos = at + cstr.size();
        _redraw(at);
        return cstr.size();
    }
    // Prompt text
    void setPrompt(std::string_view prompt_text) {
        usize prompt_len = write_sv(prompt_text.data());
        m_beg = prompt_len;
    }

    // go to position in line
    void goTo(usize n) {
        if (n > m_buffer.size()) return;
        char buffer[64];
        char* ptr = buffer;
        *ptr++ = '\033';
        *ptr++ = '[';
        // \no-heap convert line-length to integer and go to char based on that
        auto to_chars = std::to_chars(ptr, buffer+sizeof(buffer)-1, m_beg + n + 1);
        if (to_chars.ec != std::errc{}) return;
        ptr = to_chars.ptr;
        *ptr++ = 'G';
        *ptr = '\0';
        write_sv(buffer);
        m_pos = n;
    }

    // Step cursor right n times
    int cursorStep(usize n, BufferController::Dir direction) {
        char buf[64];
        char* ptr = buf;
        *ptr++ = '\033';
        *ptr++ = '[';

        auto to_chars = std::to_chars(ptr, buf+sizeof(buf)-1, n);
        if (to_chars.ec != std::errc{}) return 0;

        ptr = to_chars.ptr;
        *ptr++ = direction;
        *ptr = '\0';

        switch (direction) {
            case Left: {
                if (m_pos < n) return 0;
                m_pos -= n; break;
            }
            case Right: {
                if (m_pos+n > m_buffer.size()) return 0;
                m_pos += n; break;
            }
            case Up: case Down: break;
        }
        write_sv(buf);
        return n;
    }

    // delete previous standing character
    void deleteBack() {
        if (m_pos == 0) return;
        usize at = m_pos - 1;
        m_buffer.erase(at, 1);
        m_pos = at;
        write_sv("\b");   // move terminal cursor back one column to `at`
        _redraw(at);
    }
    // Clear entire line
    void clearLine() {
        lineHome();
        clearLineRight();
        _redraw(m_pos);
    }
    // Clear from cursor to end of line
    void clearLineRight() {
        m_buffer = m_buffer.substr(0, m_pos);
        _redraw(m_pos);
    }
    // Clear from start of line to cursor
    void clearLineLeft() {
        m_buffer = m_buffer.substr(m_pos, std::string::npos);
        _redraw(m_pos);
    }
    // Go to line start
    void lineHome() {
        goTo(0);
    }
    // Go to line end
    void lineEnd() {
        goTo(m_buffer.size());
    }
};

// BufferController disables copying at construction, reference instead
inline BufferController& BufCtl() {
    static BufferController buffer_controller;
    return buffer_controller;
}

DOTL_NAMESPACE_END()
