#pragma once

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include "switch/runtime/hosversion.h"
#include "switch/sf/service.h"
#include "switch/services/async.h"
#include "switch/services/nifm.h"
#include "switch/services/ns.h"
#include <time.h>

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
    Result InitSerialKBD(SwkbdConfig * kbd);
    SwkbdTextCheckResult validate_serial(char* tmp_string, size_t tmp_string_size);
    bool GetManualSerial(SetSysSerialNumber* s);

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

    SwkbdTextCheckResult validate_serial(char* tmp_string, size_t tmp_string_size) {
        if (strlen(tmp_string) != 14) {
            strncpy(tmp_string, "Serial must be 14 characters.", tmp_string_size);
            return SwkbdTextCheckResult_Bad;
        }

        // Verify format: XAW + 11 digits
        if (tmp_string[0] != 'X' || !isalpha(tmp_string[1]) || !isalpha(tmp_string[2])) {
            strncpy(tmp_string, "Invalid format. Should start with XAW.", tmp_string_size);
            return SwkbdTextCheckResult_Bad;
        }

        for (int i = 3; i < 14; i++) {
            if (!isdigit(tmp_string[i])) {
                strncpy(tmp_string, "Last 11 chars must be digits.", tmp_string_size);
                return SwkbdTextCheckResult_Bad;
            }
        }

        return SwkbdTextCheckResult_OK;
    }   
}