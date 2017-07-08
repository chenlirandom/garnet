#!/bin/bash
# ======================================
# detect garnet root directory
# ======================================
GARNET_ROOT=$(cd $(dirname $(pwd)/${BASH_SOURCE[0]})/..; pwd)

# ===========================
# setup default build variants
# ===========================
if [ ${OSTYPE} = "cygwin" ] ; then
    export GN_BUILD_TARGET_OS=cygwin
    export GN_BUILD_TARGET_CPU=x86
else
    export GN_BUILD_TARGET_OS=posix
    export GN_BUILD_TARGET_CPU=x64
fi
export GN_BUILD_COMPILER=gcc
export GN_BUILD_VARIANT=retail
export GN_BUILD_STATIC_LINK=1

# ======================================
# parse command line arguments
# ======================================
while [[ $# -gt 0 ]]
do
    case "$1" in
    debug)
        GN_BUILD_VARIANT=debug
        shift
        ;;
    retail)
        GN_BUILD_VARIANT=retail
        shift
        ;;
    x86)
        GN_BUILD_TARGET_OS=x86
        shift
        ;;
    x64)
        GN_BUILD_TARGET_OS=x64
        shift
        ;;
    *) # default
        ;;
    esac
done

# ===========
# setup scons
# ===========
SCONS_ROOT=${GARNET_ROOT}/env/scons/2.4.1
SCONS_LIB_DIR=${SCONS_ROOT}/engine
export SCONS_ROOT
export PATH SCONS_LIB_DIR
export SCONSFLAGS=-U
echo SCons Directory: ${SCONS_ROOT}

# ===========
# setup alias
# ===========
while read -r line
do
    line="${line/\$\*/}"  # remove $*
    line="${line//\\/\/}"  # replace "\" with "/"
    line="${line/cd \/d/cd}" # replace "cd /d" with "cd"

    # replace all %..% with ${..}
    line2="${line/\%/\$\{}" && line3="${line2/\%/\}}"
    while [ "${line}" != "${line3}" ]; do
        line="${line3}"
        line2="${line/\%/\$\{}" && line3="${line2/\%/\}}" #replace one pair of %...%
    done

    eval x=($line) # split line into array of words. x[0] is alias name, x[1] is alias value
    alias ${x[0]}="${x[1]}"
done < ${GARNET_ROOT}/env/alias.txt

# =====
# Misc.
# =====

unset INCLUDE
unset LIBS
PATH=${GARNET_ROOT}/env/bin/bash:${PATH}


# ============
# End of setup
# ============

cd ${GARNET_ROOT}

# =========================
# Call user specific script
# =========================

# if exist env/user/${USER}.sh env/user/${USER}.sh

# ================================================
# Garnet build environment setup done successfully
# ================================================
echo USERNAME             = $USER
echo GARNET_ROOT          = $GARNET_ROOT
echo GN_BUILD_COMPILER    = $GN_BUILD_COMPILER
echo GN_BUILD_VARIANT     = $GN_BUILD_VARIANT
echo GN_BUILD_TARGET_OS   = $GN_BUILD_TARGET_OS
echo GN_BUILD_TARGET_CPU  = $GN_BUILD_TARGET_CPU
echo GN_BUILD_STATIC_LINK = $GN_BUILD_STATIC_LINK
