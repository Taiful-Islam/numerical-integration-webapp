@echo off
echo.
echo ==========================================
echo        MINI SQL ENGINE - BUILD
echo ==========================================
echo.

where win_bison >nul 2>nul
if errorlevel 1 (
    echo ERROR: win_bison was not found.
    pause
    exit /b 1
)

where win_flex >nul 2>nul
if errorlevel 1 (
    echo ERROR: win_flex was not found.
    pause
    exit /b 1
)

where gcc >nul 2>nul
if errorlevel 1 (
    echo ERROR: gcc was not found.
    pause
    exit /b 1
)

echo [1/3] Generating parser...
win_bison -d compiler.y
if errorlevel 1 goto failed

echo [2/3] Generating lexer...
win_flex compiler.l
if errorlevel 1 goto failed

echo [3/3] Compiling C files...
gcc compiler.tab.c lex.yy.c engine.c main.c -o mini_sql.exe
if errorlevel 1 goto failed

if not exist data mkdir data

echo.
echo ==========================================
echo BUILD SUCCESSFUL
echo ==========================================
echo Run the GUI with:
echo python app.py
echo.
pause
exit /b 0

:failed
echo.
echo BUILD FAILED.
echo Check the error messages above.
pause
exit /b 1
