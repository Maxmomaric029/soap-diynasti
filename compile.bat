@echo off
echo Compilando Soap Dynasty Structured (.EXE)...

set RAYLIB_PATH=C:\raylib\raylib

:: Compilamos todos los archivos .cpp de la carpeta src
g++ src\main.cpp src\Game.cpp src\World.cpp -o SoapDynasty.exe ^
    -Isrc ^
    -I%RAYLIB_PATH%\src ^
    -L%RAYLIB_PATH%\src ^
    -lraylib -lopengl32 -lgdi32 -lwinmm ^
    -static

if %errorlevel% neq 0 (
    echo.
    echo ERROR: No se pudo compilar.
    pause
    exit /b %errorlevel%
)

echo.
echo EXITO: SoapDynasty.exe creado!
pause
start SoapDynasty.exe
