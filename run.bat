@echo off
if not exist mini_sql.exe (
    call build.bat
)
if not exist mini_sql.exe exit /b 1
python app.py
