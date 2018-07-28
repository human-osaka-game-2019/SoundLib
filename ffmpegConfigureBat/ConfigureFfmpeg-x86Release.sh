#!/bin/sh

../ffmpeg/configure \
--toolchain=msvc \
--prefix=./x86Release \
--cpu=i686 \
--target-os=mingw32 \
--arch=x86 \
--enable-static \
--disable-shared \
--enable-cross-compile \
--disable-doc \
--disable-htmlpages \
--disable-manpages \
--disable-podpages \
--disable-txtpages \
--disable-swscale \
--disable-avfilter \
--disable-avdevice \
--disable-postproc \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-network \
--optflags="-O2" \
--extra-cflags="-I/mingw32/include -MT" \
--extra-ldflags="-LIBPATH:/mingw32/lib" \
--extra-libs="user32.lib" \
--pkg-config-flags="--static" \
--enable-small 