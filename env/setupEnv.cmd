@echo off

pushd %~dp0..
set GARNET_ROOT=%cd%
popd

REM ==========
REM setup path
REM ==========
if "AMD64" == "%PROCESSOR_ARCHITECTURE%" (
    set mypath=%GARNET_ROOT%\env\bin\x64;%GARNET_ROOT%\env\bin\win32;%GARNET_ROOT%\env\bin\common
) else (
    set mypath=%GARNET_ROOT%\env\bin\win32;%GARNET_ROOT%\env\bin\common
)
set PATH=%mypath%;%PATH%
set mypath=


REM ===========
REM setup scons
REM ===========
echo SCons Directory : %GARNET_ROOT%\env\scons
set PATH=%GARNET_ROOT%\env\scons\Scripts;%PATH%
set SCONS_LIB_DIR=%GARNET_ROOT%\env\scons\Lib

REM ===========
REM setup alias
REM ===========

alias root "cd /d %GARNET_ROOT%\$*"
alias src "cd /d %GARNET_ROOT%\src\$*"
alias priv "cd /d %GARNET_ROOT%\src\priv\$*"
alias base "cd /d %GARNET_ROOT%\src\priv\base\$*"
alias win "cd /d %GARNET_ROOT%\src\priv\win\$*"
alias core "cd /d %GARNET_ROOT%\src\priv\core\$*"
alias gfx "cd /d %GARNET_ROOT%\src\priv\gfx\$*"
alias input "cd /d %GARNET_ROOT%\src\priv\input\$*"
alias d3d "cd /d %GARNET_ROOT%\src\priv\d3d\$*"
alias ogl "cd /d %GARNET_ROOT%\src\priv\ogl\$*"
alias test "cd /d %GARNET_ROOT%\src\priv\test\$*"
alias doc "cd /d %GARNET_ROOT%\src\priv\doc$*"
alias man "cd /d %GARNET_ROOT%\src\priv\manual$*"
alias misc "cd /d %GARNET_ROOT%\src\priv\misc$*"
alias inc "cd /d %GARNET_ROOT%\src\priv\inc\garnet\$*"
alias extern "cd /d %GARNET_ROOT%\src\extern\$*"
alias bin "cd /d %GARNET_ROOT%\bin"
alias msvc "cd /d %GARNET_ROOT%\msvc"
alias env "cd /d %GARNET_ROOT%\env\$*"

REM =====
REM Misc.
REM =====

set INCLUDE=
set LIBS=

REM ============
REM End of setup
REM ============

echo Garnet root : %GARNET_ROOT%
cd /d %GARNET_ROOT%
