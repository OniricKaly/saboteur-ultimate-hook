@echo off
chcp 65001 >nul
title Script Git Automático

echo ==========================================
echo   Script de Git: Add, Commit y Push
echo ==========================================
echo.

:: 1. Git Add
echo [Paso 1] Agregando cambios (git add .)...
git add .
if errorlevel 1 (
    echo [ERROR] Hubo un problema al agregar los archivos.
    pause
    exit /b
)
echo.

:: 2. Git Commit (Solicitar mensaje)
echo [Paso 2] Escribe el mensaje para el commit:
set /p mensaje_commit=
if "%mensaje_commit%"=="" (
    echo [ERROR] El mensaje del commit no puede estar vacío.
    pause
    exit /b
)

echo Ejecutando commit con mensaje: "%mensaje_commit%"
git commit -m "%mensaje_commit%"
if errorlevel 1 (
    echo [ERROR] El commit falló (posiblemente no hay cambios para confirmar).
    pause
    exit /b
)
echo.

:: 3. Confirmación antes del Push
echo [Paso 3] Revisión final
set /p confirmar=¿Todo está correcto para hacer push? (S/N): 

if /i "%confirmar%"=="S" (
    echo.
    echo Enviando cambios a origin master...
    git push -u origin master
    if errorlevel 1 (
        echo [ERROR] El push falló. Verifica tu conexión o permisos.
    ) else (
        echo.
        echo ¡Éxito! Cambios subidos correctamente.
    )
) else (
    echo.
    echo Push cancelado por el usuario.
)

echo.
pause