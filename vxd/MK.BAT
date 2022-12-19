@echo off
if "%1"=="all" goto com
if "%1"=="com" goto com
if "%1"=="vxd" goto vxd
echo Usage: "mk com [...]" or "mk vxd [...]" or "mk all" (eg, "mk vxd depend")
echo.
goto end
REM
REM This builds SPY.COM, a stand-alone DOS debugger
REM
:com
nmake %2 %3 %4
if not "%1"=="all" goto end
REM
REM This builds SPY.386, a stand-alone VxD debugger
REM
:vxd
nmake -f spy.mk %2 %3 %4
if errorlevel 1 goto end
if exist c:\chicago\system\spy.386 if exist spy.386 copy spy.386 c:\chicago\system
if exist c:\chicago\system\spy.sym if exist spy.sym copy spy.sym c:\chicago\system
:end
