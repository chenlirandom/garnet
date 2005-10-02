@echo off

pushd %~dp0..
set GARNET_ROOT=%cd%
popd

REM ==========
REM setup path
REM ==========
if "AMD64" == "%PROCESSOR_ARCHITECTURE%" (
    set PATH=%GARNET_ROOT%\env\bin\x64;%GARNET_ROOT%\env\bin\win32;%GARNET_ROOT%\env\bin\common;%PATH%
) else (
    set PATH=%GARNET_ROOT%\env\bin\win32;%GARNET_ROOT%\env\bin\common;%PATH%
)


REM ===========
REM setup scons
REM ===========

REM set PATH=%GARNET_ROOT%\env\scons\Scripts;%PATH%
REM set PYTHONPATH=%GARNET_ROOT%\env\scons\Lib;%PYTHONPATH%

REM ===========
REM setup alias
REM ===========

alias root "cd /d %GARNET_ROOT%\$*"
alias priv "cd /d %GARNET_ROOT%\core\priv\$*"
alias base "cd /d %GARNET_ROOT%\core\priv\base\$*"
alias core "cd /d %GARNET_ROOT%\core\priv\core\$*"
alias gfx "cd /d %GARNET_ROOT%\core\priv\gfx\$*"
alias d3d "cd /d %GARNET_ROOT%\core\priv\d3d\$*"
alias ogl "cd /d %GARNET_ROOT%\core\priv\ogl\$*"
alias test "cd /d %GARNET_ROOT%\core\priv\test\$*"
alias doc "cd /d %GARNET_ROOT%\core\priv\doc$*"
alias man "cd /d %GARNET_ROOT%\core\priv\manual$*"
alias misc "cd /d %GARNET_ROOT%\core\priv\misc$*"
alias inc "cd /d %GARNET_ROOT%\core\priv\inc\garnet\$*"
alias extern "cd /d %GARNET_ROOT%\core\extern\$*"
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
