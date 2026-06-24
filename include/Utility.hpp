#pragma once
#include "Core.hpp"


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
bool is_printable(char c) {
    return (c >= ' ') && (c <= '~');
}

// returns true if `c` is ansi character
bool is_ansi(char c) {
    return c == '\033';
}

// return true if `c` is standart after-ansi character
bool is_after_ansi(char c) {
    return
        c=='[' || c=='O'
    ;
}

// return true if `c` is standart after-ansi lead character
bool is_after_ansi_lead(char c) {
    return (c>63 && c<127);  // 64='@', 126='~'
}


inline isize read_cstr(char& cstr, isize n) {
    return ::read(STDIN_FILENO, &cstr, n);
}

inline bool read_ch(char& c) {
    return read_cstr(c, 1) == 1;
}

inline usize write_cstr(const char* cstr, usize leave_last_n_bytes=0) {
    usize cstr_len = std::strlen(cstr);
    ::write(STDOUT_FILENO, cstr, cstr_len - leave_last_n_bytes);
    return cstr_len;
}

inline void write_ch(char c) {
    ::write(STDOUT_FILENO, &c, 1);
}




DOTL_NAMESPACE_END()
