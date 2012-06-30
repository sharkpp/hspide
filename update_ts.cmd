@echo off
setlocal

prompt $g

pushd "%~dp0\hspide"
echo on
lupdate hspide.pro
@echo off
popd

echo.
pause
