@echo off
setlocal
if "%~1"=="" goto usage
if not "%~2"=="" goto usage
for /f "delims=0123456789" %%p in ("%~1") do goto usage
set "pid=%~1"
if "%pid:~0,1%"=="0" goto usage
if "%~1"=="1" goto usage
for /f "tokens=1 delims=," %%p in ('tasklist /fi "PID eq %~1" /fo csv /nh') do (
    if /i "%%~p"=="app.exe" goto stop
    if /i "%%~p"=="app_d.exe" goto stop
)
echo PID %~1 is not an app.exe or app_d.exe process; refusing to stop it. 1>&2
exit /b 1
:stop
taskkill /f /pid %~1
exit /b %errorlevel%
:usage
echo Usage: %~nx0 ^<application-pid^> 1>&2
exit /b 2
