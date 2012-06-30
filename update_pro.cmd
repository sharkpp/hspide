@echo off
setlocal

prompt $g

cl.exe >nul 2>&1
if errorlevel 1 goto :no_env

for /D %%I in (%~dp0*) do (
	if exist "%%~I\%%~nxI.pro" call :qmake "%%~I"
)

echo.
pause
goto :eof

:qmake
	pushd "%~1"
	echo -- qmake %~nx1.pro --
	echo on
	qmake
	@echo off
	popd
	exit /b

:no_env
	echo Visual Studio �R�}���h�v�����v�g�ȂǊJ������PATH���ʂ��Ă����ԂŎ��s���Ă�������
	pause
	goto :eof
