#pragma once

#include <switch.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

namespace Verifier {
    const char* hardware_model[] = {
        "Icosa",
        "Copper",
        "Hoag",
        "Iowa",
        "Calcio",
        "Aula",
        "Unknown"
    };

    void Init(void);
    int Main(int argc, char* argv[]);

    int xdigit(char c) {
        c = tolower(c);

        if (!isxdigit(c))
            return -1;

        if (c >= 'a')
            return c - 'a' + 10;
        return c - '0';
    }

    SwkbdTextCheckResult validate_text(char* tmp_string, size_t tmp_string_size) {
        for (int i = 0; i < 6; i++) {
            tmp_string[i] = tolower(tmp_string[i]);
            if (!isxdigit(tmp_string[i])) {
                strncpy(tmp_string, "Invalid hash.", tmp_string_size);
                return SwkbdTextCheckResult_Bad;
            }
        }

        tmp_string[6] = tolower(tmp_string[6]);
        tmp_string[7] = tolower(tmp_string[7]);
        if (xdigit(tmp_string[6]) != (xdigit(tmp_string[0]) + xdigit(tmp_string[2]) + xdigit(tmp_string[4])) % 16
            || xdigit(tmp_string[7]) != (xdigit(tmp_string[1]) + xdigit(tmp_string[3]) + xdigit(tmp_string[5])) % 16) {
            strncpy(tmp_string, "Invalid hash.", tmp_string_size);
            return SwkbdTextCheckResult_Bad;
        }

        return SwkbdTextCheckResult_OK;
    }   
}