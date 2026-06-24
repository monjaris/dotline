#pragma once
#include "TerminalController.hpp"


DOTL_NAMESPACE_BEGIN(dotl)

class BufCtl {
    usize m_beg = 0;  // beginning of the stream, prompt's end position
    usize m_len = 0;  // the length of the line between last character and `m_beg`
    usize m_pos = 0;  // the position of the vcursor between `m_beg` and `m_beg+m_len`

    void bump_len() {
        if (m_pos > m_len) {
            m_len = m_pos;
        }
    }
public:
    enum Dir : char {
        Up='A', Down='B', Right='C', Left='D'
    };

    usize pos() { return m_pos; }
    void setPos(usize pos) { m_pos = pos; }
    void setPosRel(usize pos) { m_pos += pos; }
    // get length of the line
    usize length() { return m_len; }

    // Write characters and increment axis index
    void insert(char c) {
        write_ch(c);
        ++m_pos;
        bump_len();
    }
    usize insert(const char* cstr) {
        usize ins_len = write_cstr(cstr);
        m_pos += ins_len;
        bump_len();
        return ins_len;
    }
    // Prompt text
    void setPrompt(const char* prompt_text) {
        usize prompt_len = write_cstr(prompt_text);
        m_beg = prompt_len;
    }

    // go to position in line
    void goTo(usize n) {
        if (n > m_len) return;
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
        write_cstr(buffer);
        m_pos = n;
    }

    // Step cursor right n times
    int cursorStep(usize n, BufCtl::Dir direction) {
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
                if (m_pos+n > m_len) return 0;
                m_pos += n; break;
            }
            case Up: case Down: break;
        }
        write_cstr(buf);
        bump_len();
        return n;
    }

    // delete previous standing character
    void deleteBack() {
        write_cstr("\b \b");
        if (m_pos != 0) m_pos -= 1;
    }
    // Clear entire line
    void clearLine() {
        write_cstr("\033[2K");
        m_pos = 0;
    }
    // Clear from cursor to end of line
    void clearLineRight() {
        write_cstr("\033[K");
    }
    // Clear from start of line to cursor
    void clearLineLeft() {
        write_cstr("\033[1K");
        m_pos = 0;
    }
    // Go to line start
    void lineHome() {
        goTo(0);
    }
    // Go to line end
    void lineEnd() {
        goTo(m_len);
    }
};

inline BufCtl& bufCtl() {
    static BufCtl buffer_controller;
    return buffer_controller;
}

DOTL_NAMESPACE_END()

