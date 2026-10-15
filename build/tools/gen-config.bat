@echo off
setlocal
cd /d "%~dp0"
if not defined PYTHON set "PYTHON=python"
if not defined CLANG_FORMAT set "CLANG_FORMAT=clang-format"

"%PYTHON%" config_tool\config_tool.py -p ..\resource
if errorlevel 1 exit /b 1
"%CLANG_FORMAT%" -i --style=file:..\..\.clang-format ..\resource\code\meta_define.hpp
if errorlevel 1 exit /b 1
copy ..\resource\code\meta_define.hpp ..\..\include\ark\meta_define.hpp /Y
if errorlevel 1 exit /b 1
