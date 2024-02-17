#!/usr/bin/env bash

set -e

PLATFORM=$1
ACTION=$2

FFMPEG_VERSION=n4.3.1
OPENSSL_VERSION=be63e58732cedc0bbc39777d1cf7556e55f146ef

# We don't want package managers to mess up the build
export PATH=/usr/bin:/bin:/usr/sbin:/sbin

if [ "$ACTION" = "build" ]; then
    sh scripts/init-openssl.sh $PLATFORM $OPENSSL_VERSION
    sh scripts/init-ffmpeg.sh  $PLATFORM $FFMPEG_VERSION
    sh scripts/compile-openssl.sh $PLATFORM "build"
    sh scripts/compile-ffmpeg.sh $PLATFORM "build"
elif [ "$ACTION" = "clean" ]; then
    sh scripts/compile-openssl.sh $PLATFORM "clean"
    sh scripts/compile-ffmpeg.sh $PLATFORM "clean"
else
    echo "Usage:"
    echo "  build.sh iOS build"
    echo "  build.sh iOS clean"
    echo " ---"
    echo "  build.sh tvOS build"
    echo "  build.sh tvOS clean"
    echo " ---"
    echo "  build.sh macOS build"
    echo "  build.sh macOS clean"
    exit 1
fi
