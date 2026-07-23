#pragma once
#include "core.hpp"


DOTL_NAMESPACE_BEGIN(dotl)


namespace util {
    constexpr uint32 digit_c(usize value) {
        uint32 count = 0;
        if (value == 0) return 1;
        while (value > 0) {
            count++;
            value /= 10;
        }
        return count;
    }

    constexpr auto UINT_DIGITS = digit_c(std::numeric_limits<usize>::max());
}



// returns true if `c` is printable in terminal
inline bool is_printable(char c) {
    return (c >= ' ') && (c <= '~');
}

// returns true if `c` is ansi character
inline bool is_ansi(char c) {
    return c == '\033';
}

// return true if `c` is standart after-ansi character
inline bool is_after_ansi(char c) {
    return
        c=='[' || c=='O'
    ;
}

inline char ctrl_bind(char key) {
    if (::islower(key)) return key-'a'+1;
    else if (::isupper(key)) return key-'A'+1;
    return '\0';
}

// return true if `c` is standart after-ansi lead character
inline bool is_after_ansi_lead(char c) {
    return (c>63 && c<127);  // 64='@', 126='~'
}

inline bool read_ch(char& c) {
    return ::read(STDIN_FILENO, &c, 1) == 1;
}

inline isize read_cstr(const char* cstr, isize n) {
    return ::read(STDIN_FILENO, (void*)cstr, n);
}

inline void write_ch(char c) {
    ::write(STDOUT_FILENO, &c, 1);
}

inline usize write_sv(const std::string_view sv, usize leave_last_n_bytes=0) {
    ::write(STDOUT_FILENO, sv.data(), sv.size() - leave_last_n_bytes);
    return sv.size() - leave_last_n_bytes;
}


DOTL_NAMESPACE_END()
