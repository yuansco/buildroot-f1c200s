#!/bin/bash
set -e

SELFDIR=`dirname \`realpath ${0}\``

[ $# -eq 2 ] || {
    echo "SYNTAX: $0 <output dir> <u-boot-with-spl image>"
    echo "Given: $@"
    exit 1
}

support/scripts/genimage.sh ${1} -c board/yuan/scripts/genimage-sdcard-zimage.cfg
