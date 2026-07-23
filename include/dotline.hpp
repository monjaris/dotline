// *dotline* This is dotty's header-only module for reading lines without noise
// Unlike readline.h, it is not required to link it to any library
// It is POSIX-compliant, meaning it works on Linux, BSDs and MacOS
// You can modify, distribute, and sell it as you wish.

#pragma once
//
#include "include/BufferController.hpp"
#include "include/PString.hpp"

DOTL_NAMESPACE_BEGIN(dotl)

// forward decl
PString read_string();


struct Prompter {
    Prompter ()  = default;
    Prompter (Prompter&&)  = delete;
    Prompter (const Prompter&)  = delete;

    [[nodiscard]]
    PString read_string() { return ::dotl::read_string(); }
};


// prompt string
static inline Prompter _prompt(void* _msg) {
    BufCtl().setPrompt((char*)_msg);
    return Prompter{};
}
inline Prompter prompt(const char* prompt_message) {
    return _prompt((char*)prompt_message);
}
inline Prompter prompt(const std::string& prompt_message) {
    return _prompt((char*)prompt_message.data());
}


// reads input and writes it to string
// no ansi escape character noise!
inline PString read_string() {
    auto& tctl = TermCtl();
    tctl.setMode(TerminalController::TermMode::RAW);

    auto& bufctl = BufCtl();
    bufctl.clear();

    char c0;
    while (read_ch(c0))
    {
        // new line
        if (c0 == '\n') break;
        // carriage return
        else if (c0 == '\r') {
            break;
        }
        else if (c0==127 || c0=='\b') {
            bufctl.deleteBack();
        }
        // ansi characters
        else if (c0 == '\033') {
            tctl.setMode(TerminalController::TermMode::ESC);
            char e;
            // read for after-escapes
            if (read_ch(e)) {
                // Both CSI (ESC '[' X) and SS3 (ESC 'O' X) variants use the
                // same trailing letter for arrow keys (A/B/C/D), depending
                // on whether the terminal is in application cursor-key mode.
                if (e=='[' || e=='O') {
                    char lead;
                    if (read_ch(lead)) {
                        if (lead == 'D' && bufctl.pos()>0) {
                            bufctl.cursorStep(1, BufferController::Left);
                        } else if (lead == 'C' && bufctl.pos() < bufctl.length()) {
                            bufctl.cursorStep(1, BufferController::Right);
                        } else if (lead == 'A') {
                            bufctl.lineHome();
                        } else if (lead == 'B') {
                            bufctl.lineEnd();
                        }
                    }
                }
            }

            tctl.setMode(TerminalController::TermMode::RAW);
        }
        // printable characters
        else if (is_printable(c0)) {
            bufctl.insert(c0);
        }
    }
    write_ch('\n');

    tctl.restore();
    return PString{bufctl.string()};
}


DOTL_NAMESPACE_END()
