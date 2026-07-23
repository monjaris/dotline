#pragma once
#include "Utility.hpp"

// *dotline* This is dotty's header-only module for reading lines without noise
// Unlike readline.h, it is not required to link it to any library
// It is POSIX-compliant, meaning it works on Linux, BSDs and MacOS
// You can modify, distribute, and sell it as you wish.


DOTL_NAMESPACE_BEGIN(dotl)


class TerminalController {
    using Ctermios = ::termios;

    Ctermios m_orig;
    Ctermios m_term;
    bool restored = true;

    // ANSI
    static void mEnableAnsi(Ctermios& term_new) {
        term_new.c_lflag |= ECHO;  // enable echo
        term_new.c_lflag |= ICANON;  // enable canonical mode
    }
    // RAW
    static void mEnableRaw(Ctermios& term_new) {
        term_new.c_lflag &= ~ECHO;  // disable echo
        term_new.c_lflag &= ~ICANON;  // disable canonical mode
        term_new.c_cc[VMIN]  = 1;  // 1 char to read() for
        term_new.c_cc[VTIME] = 0;  //  x 100ms
    }
    // Escape sequence
    static void mEnableEsc(Ctermios& term_new) {
        term_new.c_cc[VMIN]  = 0;
        term_new.c_cc[VTIME] = 1;
    }

public:

    enum class TermMode {
        ANSI, RAW, ESC
    };
    TermMode m_mode = TermMode::ANSI;


    // disable copying as construction
    TerminalController (const TerminalController&) = delete;
    // default ctor
    TerminalController () {
        tcgetattr(STDIN_FILENO, &m_orig);
    }
    // dtor
    ~TerminalController () {
        this->restore();
    }

    Ctermios getOriginal() { return m_orig; }
    Ctermios* Original() { return &m_orig; }

    Ctermios getTermIOS() { return m_term; }
    Ctermios* TermIOS() { return &m_term; }

    void setMode(TermMode mode) {
        if (mode == m_mode) return;
        else {
            m_mode = mode;
            restored = false;
        }

        ::tcgetattr(STDIN_FILENO, &m_term);
        Ctermios term_new = m_term;
        switch (mode) {
            case TermMode::ANSI: {
                mEnableAnsi(term_new); break;
            };
            case TermMode::RAW: {
                mEnableRaw(term_new); break;
            }
            case TermMode::ESC: {
                mEnableEsc(term_new); break;
            }
        };
        ::tcsetattr(STDIN_FILENO, TCSANOW, &term_new);
    }

    void restore() {
        if (!restored) {
            ::tcsetattr(STDIN_FILENO, TCSANOW, &m_orig);
            restored = true;
        }
    }

};


// `TerminalController` disables copying at construction
// Initialize with `auto&` or `TerminalController&` instead
inline TerminalController& TermCtl() {
    static TerminalController terminal_controller;
    return terminal_controller;
}


DOTL_NAMESPACE_END()

