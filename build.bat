@echo off
pip install pyinstaller requests
pyinstaller --onefile --noconsole ^
    --name "NitroGen_by_Rubilacxe" ^
    --icon icon.ico ^
    main.py
echo [+] Done -> dist\NitroGen_by_Rubilacxe.exe
pause
