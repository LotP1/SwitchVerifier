// Include the most common headers from the C standard library
#include "common.hpp"

// Main program entrypoint
int main(int argc, char* argv[]) {
    Verifier::Init();
    Verifier::Main(argc, argv);
    return 0;
}

namespace Verifier {
    void Init() {
        Result ret = 0;

        if (R_FAILED(ret = setsysInitialize())) {
            printf("setsysInitialize() failed: 0x%x.\n\n", ret);
        }

        if (R_FAILED(ret = splInitialize())) {
            printf("splInitialize() failed: 0x%x.\n\n", ret);
        }

        if (R_FAILED(ret = nsInitialize())) {
            printf("nsInitialize() failed: 0x%x.\n\n", ret);
        }
    }

    Result InitKBD(SwkbdConfig * kbd) {
        Result rc = 0;

        rc = swkbdCreate(kbd, 0);

        if (R_SUCCEEDED(rc)) {
            swkbdConfigMakePresetDefault(kbd);
            swkbdConfigSetOkButtonText(kbd, "Submit");
            swkbdConfigSetSubText(kbd, "User Hash: please input your User Hash from the discord bot");
            swkbdConfigSetGuideText(kbd, "xxxxxxxx");
            swkbdConfigSetStringLenMax(kbd, 8u);
            swkbdConfigSetTextCheckCallback(kbd, validate_text);
        }

        return rc;
    }

    char* GenerateToken() {
        Result ret = 0;

        SwkbdConfig kbd;
        char h[16] = { 0 };
        ret = InitKBD(&kbd);

        if (R_SUCCEEDED(ret)) {
            ret = swkbdShow(&kbd, h, sizeof(h));
            swkbdClose(&kbd);

            if (R_SUCCEEDED(ret)) {
                printf("user hash: %s\n", h);
            }
            else {
                printf("Invalid user hash, please relaunch the SwitchVerifier.nro and enter a valid User Hash.\n\n");
                return nullptr;
            }

        }
        else {
            printf("Invalid User Hash, please relaunch the SwitchVerifier.nro and enter a valid User Hash.\n\n");
            return nullptr;
        }

        SetSysSerialNumber s;

        if (R_FAILED(ret = setsysGetSerialNumber(&s))) {
            printf("A value wasn't fetched correctly, please reboot your homebrew menu and relaunch the SwitchVerifier.nro.\n\n");
            return nullptr;
        }

        bool isAllZero = true;
        for (int i = 3; i < 14; i++) {
            if (s.number[i] != '0') {
                isAllZero = false;
                break;
            }
        }

        if (isAllZero || s.number[0] != 88) {
            printf("A value wasn't fetched correctly, please make sure your serial isn't spoofing hardware values and then relaunch the SwitchVerifier.nro.\n\n");
            return nullptr;
        }

        printf("Serial number: %s\n", s.number);

        long t = 0;
        long f = 0;
        u64 in = 3;
        long a = 0;

        Service srv = { 0 }, * srv_ptr = &srv;
        Result rc = 0;
        if (hosversionAtLeast(3, 0, 0))
            rc = nsGetContentManagementInterface(&srv);
        else
            srv_ptr = nsGetServiceSession_GetterInterface();

        if (R_SUCCEEDED(rc)) rc = serviceDispatchInOut(srv_ptr, 47, in, t);
        if (R_SUCCEEDED(rc)) rc = serviceDispatchInOut(srv_ptr, 48, in, f);

        serviceClose(&srv);

        if (R_SUCCEEDED(rc) && hosversionAtLeast(3, 0, 0))
            srv_ptr = appletGetServiceSession_CommonStateGetter();
        else {
            printf("A value wasn't fetched correctly, please reboot your homebrew menu and relaunch the SwitchVerifier.nro.\n\n");
            return nullptr;
        }
            
        serviceAssumeDomain(srv_ptr);
        rc = serviceDispatchOut(srv_ptr, 60, a);

        if (R_FAILED(rc) || t == 0 || f == 0) {
            printf("A value wasn't fetched correctly, please make sure your device isn't spoofing hardware values isn't spoofed and then relaunch the SwitchVerifier.nro.\n\n");
            return nullptr;
        }

        time_t ti = time(NULL);
        struct tm* tm = gmtime((const time_t*)&ti);
        int mi = tm->tm_min;
        int ho = tm->tm_hour;
        int da = tm->tm_mday;
        int mo = tm->tm_mon + 1;
        int ye = tm->tm_year;
        char a1 = (a >> 4) % 10;
        char a2 = (a >> 8) % 10;
        char a3 = (a >> 36) % 10;
        char a4 = (a >> 40) % 10;
        char t1 = t % 10;
        char t2 = (t / 10) % 10;
        char t3 = (t / 100) % 10;
        char t4 = (t / 1000) % 10;
        char f1 = f % 10;
        char f2 = (f / 10) % 10;
        char f3 = (f / 100) % 10;
        char f4 = (f / 1000) % 10;
        char u1 = xdigit(h[0]) % 10;
        char u2 = xdigit(h[1]) % 10;
        char u3 = xdigit(h[2]) % 10;
        char u4 = xdigit(h[3]) % 10;
        char u5 = xdigit(h[4]) % 10;
        char u6 = xdigit(h[5]) % 10;

        char* token = new char[27]();
        token[0] = 'A' + ((s.number[1] - 'A' + f1 + t1 + u1) % 25);
        token[1] = 'A' + ((s.number[2] - 'A' + f2 + t2 + u2) % 25);
        token[2] = '0' + (f1 + a1) % 10;
        token[3] = '0' + (ye % 10);
        token[4] = 'A' + ((s.number[3] - '0' + f3 + t3 + u3) % 10);
        token[5] = 'A' + (mo / 10 + (ye % 10));
        token[6] = '0' + ((s.number[4] - '0' + f4 + t4 + u4) % 10);
        token[7] = '0' + (f2 + a2) % 10;
        token[8] = 'A' + ((s.number[5] - '0' + f1 + t1 + u5) % 10);
        token[9] = 'A' + (mo % 10);
        token[10] = '0' + (da / 10 + (ye % 10));
        token[11] = '0' + ((s.number[7] - '0' + f2 + t2 + u6) % 10);
        int sum1 = token[0] + token[1] + token[2] + token[3] + token[4] + token[5]
            + token[6] + token[7] + token[8] + token[9] + token[10] + token[11];
        token[12] = 'A' + ((sum1 + u5) % 10);
        token[13] = 'A' + (f3 + a3) % 10;
        token[14] = '0' + (da % 10);
        token[15] = '0' + ((s.number[9] - '0' + f3 + t3 + u1) % 10);
        token[16] = 'A' + (ho / 10 + (mo % 10));
        token[17] = 'A' + (f4 + a4) % 10;
        token[18] = '0' + ((s.number[11] - '0' + f4 + t4 + u2) % 10);
        token[19] = '0' + (ho % 10);
        token[20] = 'A' + (((s.number[6] + s.number[8] + s.number[10] + s.number[12] - 4 * '0') * ('!' / '\t') + f1 + t1 + u3) % 10);
        token[21] = 'A' + (mi / 10 + (ho % 10));
        token[22] = '0' + ((s.number[13] - '0' + f2 + t2) % 10);
        token[23] = '0' + (mi % 10);
        int sum2 = token[0] + token[1] + token[2] + token[3] + token[4]
            + token[5] + token[6] + token[7] + token[8] + token[9]
            + token[10] + token[11] + token[12] + token[13] + token[14]
            + token[15] + token[16] + token[17] + token[18] + token[19]
            + token[20] + token[21] + token[22] + token[23];
        token[24] = 'A' + ((sum2 + u6) % 10);
        token[25] = 'L';
        token[26] = '\0'; 

        printf("token: %s\n", token);

        FILE* fptr;
        fptr = fopen("sdmc:/switch/SwitchVerification.txt", "w");
        fputs(token, fptr);
        fclose(fptr);

        printf("\ntoken has also been written to sdmc:/switch/SwitchVerification.txt\n");
        
        return token;
    }

    int Main(int argc, char* argv[])
    {
        // This example uses a text console, as a simple way to output text to the screen.
        // If you want to write a software-rendered graphics application,
        //   take a look at the graphics/simplegfx example, which uses the libnx Framebuffer API instead.
        // If on the other hand you want to write an OpenGL based application,
        //   take a look at the graphics/opengl set of examples, which uses EGL instead.
        consoleInit(NULL);

        // Configure our supported input layout: a single player with standard controller styles
        padConfigureInput(1, HidNpadStyleSet_NpadStandard);

        // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
        PadState pad;
        padInitializeDefault(&pad);

        printf("\tPress PLUS to exit.\n\n");
        
        GenerateToken();
        
        // Main loop
        while (appletMainLoop())
        {
            // Scan the gamepad. This should be done once for each frame
            padUpdate(&pad);

            // padGetButtonsDown returns the set of buttons that have been
            // newly pressed in this frame compared to the previous one
            u64 kDown = padGetButtonsDown(&pad);

            if (kDown & HidNpadButton_Plus)
                break; // break in order to return to hbmenu

            // Your code goes here

            // Update the console, sending a new frame to the display
            consoleUpdate(NULL);
        }

        // Deinitialize and clean up resources used by the console (important!)
        consoleExit(NULL);
        return 0;
    }
}
