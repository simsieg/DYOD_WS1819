#!/bin/bash

TARGET=debug
BUILDDIR=cmake-build-${TARGET}

if [[ -z $1 ]]; then
    echo "Usage: $0 configure|build|lint|format|test|coverage|sanitize|debug|git-update|shell"
    echo "Otherwise refer to the Makefile"
else
    if [[ "$1" == "configure" ]]; then
        if [ -d ${BUILDDIR} ]; then
            rm -rf ${BUILDDIR}
        fi

        mkdir ${BUILDDIR}
        cd ${BUILDDIR}

        if echo ${TARGET} | grep -i "release" >/dev/null; then
            CMAKEARGS="${CMAKEARGS} -DCMAKE_BUILD_TYPE=Release"
        fi

        cmake ${CMAKEARGS} ..
        echo "configured cmake for '${TARGET}'"
    elif [[ "$1" == "build" ]]; then
        if [[ -z ${THREADCOUNT} ]]; then
            THREADCOUNT=8
        fi

        cd ${BUILDDIR}
        make -j${THREADCOUNT}
    elif [[ "$1" == "lint" ]]; then
        ./scripts/lint.sh
    elif [[ "$1" == "format" ]]; then
        ./scripts/format.sh
    elif [[ "$1" == "test" ]]; then
        cd ${BUILDDIR}
        make hyriseTest
        cd ..
        ./${BUILDDIR}/hyriseTest
    elif [[ "$1" == "coverage" ]]; then
        ./scripts/coverage.sh ${BUILDDIR}
    elif [[ "$1" == "sanitize" ]]; then
        cd ${BUILDDIR}
        make hyriseAsan
    elif [[ "$1" == "debug" ]]; then
        if [[ -z $2 ]]; then
            echo tell me the filename inside ${BUILDDIR}/
        else
            echo "firing up gdbserver"
            GDBPORT=7777
            gdbserver 127.0.0.1:${GDBPORT} ${BUILDDIR}/$2 &
            echo "started gdbserver, listening on ${GDBPORT}"
            wait $!
            echo "gdbserver terminated"
        fi
    elif [[ "$1" == "git-update" ]]; then
        git submodule update --jobs 5 --init --recursive
    elif [[ "$1" == "shell" ]]; then
        /bin/bash
    else
        echo "unknown command"
    fi
fi

exit 0