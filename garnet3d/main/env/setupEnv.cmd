@echo off
goto :start

REM ============
REM sub routines
REM ============

:warn
echo.
echo ========
echo = WARN = %*
echo ========
echo.
goto :EOF

:usage
echo.
echo Usage: setupEnv.cmd [/h^|/?] [vc80^|vc71^|icl^|mingw] [x86^|x64^|xenon] [debug^|profile^|retail^|stdbg^|stret^|stprof]
echo.
goto :EOF

REM ====================
REM begin of main script
REM ====================
:start
pushd %~dp0..
set GARNET_ROOT=%cd%
popd

REM ============================
REM setup garnet build parameers
REM ============================
:parse_cmdline
if not "" == "%1" (
		if "/?" == "%1" ( goto :usage
		) else if /I "/h" == "%1" ( goto :usage
        ) else if /I "vc80" == "%1" ( set GN_BUILD_COMPILER=vc80
        ) else if /I "vc71" == "%1" ( set GN_BUILD_COMPILER=vc71
        ) else if /I "icl" == "%1" ( set GN_BUILD_COMPILER=icl
        ) else if /I "mingw" == "%1" ( set GN_BUILD_COMPILER=mingw
        ) else if /I "x86" == "%1" ( set GN_BUILD_TARGET_CPU=x86
        ) else if /I "x64" == "%1" ( set GN_BUILD_TARGET_CPU=x64
        ) else if /I "debug" == "%1" ( set GN_BUILD_VARIANT=debug
        ) else if /I "profile" == "%1" ( set GN_BUILD_VARIANT=profile
        ) else if /I "retail" == "%1" ( set GN_BUILD_VARIANT=retail
        ) else if /I "stdbg" == "%1" ( set GN_BUILD_VARIANT=stdbg
        ) else if /I "stprof" == "%1" ( set GN_BUILD_VARIANT=stprof
        ) else if /I "stret" == "%1" ( set GN_BUILD_VARIANT=stret
        ) else if /I "xenon" == "%1" (
                set GN_BUILD_COMPILER=xenon
                set GN_BUILD_TARGET_OS=xenon
                set GN_BUILD_TARGET_CPU=ppc
                set GN_BUILD_VARIANT=stdbg
        ) else (
                call :warn Unknown parameter "%1".
        )
        shift
        goto parse_cmdline
)

REM =======================
REM detect current CPU type
REM =======================
       if /I "amd64" == "%PROCESSOR_ARCHITECTURE%" ( set GN_CURRENT_CPU=x64
) else if /I "amd64" == "%PROCESSOR_ARCHITEWOW64%" ( set GN_CURRENT_CPU=x64
) else if /I "ia64" == "%PROCESSOR_ARCHITECTURE%" ( set GN_CURRENT_CPU=ia64
) else if /I "x86" == "%PROCESSOR_ARCHITECTURE%" ( set GN_CURRENT_CPU=x86
) else (
    call :error Unknown CPU type!
    goto :EOF
)

REM ============================
REM setup garnet build parameers
REM ============================

if "" == "%GN_BUILD_COMPILER%" set GN_BUILD_COMPILER=vc80
if "" == "%GN_BUILD_TARGET_OS%" set GN_BUILD_TARGET_OS=mswin
if "AMD64" == "%PROCESSOR_ARCHITECTURE%" (
    if "" == "%GN_BUILD_TARGET_CPU%" set GN_BUILD_TARGET_CPU=x64
) else (
    if "" == "%GN_BUILD_TARGET_CPU%" set GN_BUILD_TARGET_CPU=x86
)
if "" == "%GN_BUILD_VARIANT%" set GN_BUILD_VARIANT=debug

echo GN_BUILD_COMPILER = %GN_BUILD_COMPILER%
echo GN_BUILD_TARGET_OS = %GN_BUILD_TARGET_OS%
echo GN_BUILD_TARGET_CPU = %GN_BUILD_TARGET_CPU%
echo GN_BUILD_VARIANT = %GN_BUILD_VARIANT%

REM =====================
REM setup VS8 environment
REM =====================
if /I "vc80" == "%GN_BUILD_COMPILER%" (
    REM
    REM TODO: detect vs8 install path!
    REM
    set VS8_ROOT="C:\Program Files\Microsoft Visual Studio 8"
)

if /I "vc80" == "%GN_BUILD_COMPILER%" (
    if /I "x86" == "%GN_BUILD_TARGET_CPU%" (
        call %VS8_ROOT%\VC\vcvarsall.bat x86
    ) else if /I "x64" == "%GN_BUILD_TARGET_CPU%" (
        if /I "x64" == "%GN_CURRENT_CPU%" (
            call %VS8_ROOT%\VC\vcvarsall.bat amd64
        ) else (
            call %VS8_ROOT%\VC\vcvarsall.bat x86_amd64
        )
    ) else if /I "ia64" == "%GN_BUILD_TARGET_CPU%" (
        if /I "ia64" == "%GN_CURRENT_CPU%" (
            call %VS8_ROOT%\VC\vcvarsall.bat ia64
        ) else (
            call %VS8_ROOT%\VC\vcvarsall.bat x86_ia64
        )
    ) else (
        call :error Unsupport target CPU type: %GN_BUILD_TARGET_CPU%.
    )
)


REM =======================
REM setup xenon environment
REM =======================
if "xenon" == "%GN_BUILD_COMPILER%" (
    echo.
    if "" == "%XEDK%" (
        call :warn Environment variable XEDK not found.
    ) else (
        echo Using XDK at "%XEDK%"
        pushd .
        call "%XEDK%\bin\win32\xdkvars.bat"
        popd
    )
)

REM =================
REM setup custom path
REM =================
if "AMD64" == "%PROCESSOR_ARCHITECTURE%" (
    set mypath=%GARNET_ROOT%\env\bin\mswin\x64;%GARNET_ROOT%\env\bin\mswin\x86
) else (
    set mypath=%GARNET_ROOT%\env\bin\mswin\x86
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

alias root      "cd /d %GARNET_ROOT%\$*"
alias bin       "cd /d %GARNET_ROOT%\bin\$*"
alias bld       "cd /d %GARNET_ROOT%\build.tmp\scons\%GN_BUILD_TARGET_OS%\%GN_BUILD_TARGET_CPU%\%GN_BUILD_COMPILER%\%GN_BUILD_VARIANT%\bin\$*"
alias env       "cd /d %GARNET_ROOT%\env\$*"
alias msvc      "cd /d %GARNET_ROOT%\msvc"
alias src       "cd /d %GARNET_ROOT%\src\$*"
alias extern    "cd /d %GARNET_ROOT%\src\extern\$*"
alias media     "cd /d %GARNET_ROOT%\src\media\$*"
alias priv      "cd /d %GARNET_ROOT%\src\priv\$*"
alias base      "cd /d %GARNET_ROOT%\src\priv\base\$*"
alias core      "cd /d %GARNET_ROOT%\src\priv\core\$*"
alias doc       "cd /d %GARNET_ROOT%\src\priv\doc$*"
alias inc       "cd /d %GARNET_ROOT%\src\priv\inc\garnet\$*"
alias gfx       "cd /d %GARNET_ROOT%\src\priv\gfx\$*"
alias rndr      "cd /d %GARNET_ROOT%\src\priv\gfx\rndr\$*"
alias misc      "cd /d %GARNET_ROOT%\src\priv\misc$*"
alias sample    "cd /d %GARNET_ROOT%\src\priv\sample\$*"
alias test      "cd /d %GARNET_ROOT%\src\priv\test\$*"
alias tool      "cd /d %GARNET_ROOT%\src\priv\tool$*"
alias util      "cd /d %GARNET_ROOT%\src\priv\util$*"
alias cb        "mkdir %GARNET_ROOT%\build.tmp\cmake&pushd %GARNET_ROOT%\build.tmp\cmake&cmake ..\..\src&popd"

REM ====================
REM Update console title
REM ====================
title garnet3d ( %GARNET_ROOT% %GN_BUILD_COMPILER%-%GN_BUILD_TARGET_OS%-%GN_BUILD_TARGET_CPU% %GN_BUILD_VARIANT% )

REM ===================
REM End of common setup
REM ===================

echo Garnet root : %GARNET_ROOT%
cd /d %GARNET_ROOT%


REM =========================
REM Call user specific script
REM =========================

echo User name: %USERNAME%
if exist env\user\%USERNAME%.cmd call env\user\%USERNAME%.cmd
