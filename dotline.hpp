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
    std::string str;
    str.clear();

    auto& tctl = TermCtl();
    tctl.setMode(TerminalController::TermMode::RAW);

    auto& bufctl = BufCtl();

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
            if (!str.empty() && bufctl.pos()>0) {
                str.erase(bufctl.pos()-1, 1);
                bufctl.deleteBack();
            }
        }
        // ansi characters
        else if (c0 == '\033') {
            tctl.setMode(TerminalController::TermMode::ESC);
            char e;
            // read for after-escapes
            if (read_ch(e)) {
                char lead;
                if (e=='[') {
                    // read for after-escape-leads
                    while (read_ch(lead)) {
                        if (lead == 'D' && bufctl.pos()>0) {
                            bufctl.cursorStep(1, BufferController::Left);
                        } else if (lead == 'C' && bufctl.pos()<str.size()) {
                            bufctl.cursorStep(1, BufferController::Right);
                        } else if (lead == 'A') {
                            bufctl.lineHome();
                        } else if (lead == 'B') {
                            bufctl.lineEnd();
                        }
                    }
                }
                else if (e=='O') {
                    char e_lead;
                    // read for after-escape-leads
                    while (read_ch(e_lead)) {
                        if (is_after_ansi_lead(e_lead)) break;
                        switch (e_lead) {
                            ;
                        };
                    }
                }
            }

            tctl.setMode(TerminalController::TermMode::RAW);
        }
        // printable characters
        else if (is_printable(c0)) {
            str.insert(str.begin()+bufctl.pos(), c0);
            bufctl.insert(c0);
        }
    }
    write_ch('\n');

    tctl.restore();
    return PString{str};
}



DOTL_NAMESPACE_END()
