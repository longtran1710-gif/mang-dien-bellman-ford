@echo off
setlocal
set "QT_DIR=D:\Qt\6.12.0\mingw_64"
set "MINGW_DIR=D:\Qt\Tools\mingw1310_64"
set "CMAKE_DIR=D:\Qt\Tools\CMake_64"
set "PATH=%MINGW_DIR%\bin;%CMAKE_DIR%\bin;%QT_DIR%\bin;%PATH%"
cd /d "%~dp0"

cmake -S . -B build-release -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="%QT_DIR%" -DBUILD_TESTING=OFF
if errorlevel 1 goto loi
cmake --build build-release
if errorlevel 1 goto loi

if exist Release rmdir /s /q Release
mkdir Release
copy /y build-release\MangDienGUI.exe Release\ >nul
windeployqt --release --no-translations Release\MangDienGUI.exe
if errorlevel 1 goto loi

echo.
echo Xong. Thu muc Release da san sang.
pause
exit /b 0

:loi
echo.
echo Co loi, xem thong bao phia tren.
pause
exit /b 1
