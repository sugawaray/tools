#!/bin/sh

#  buildbb.sh
#  tools
#
#  Created by Yutaka Sugawara on 2016/09/29.
#  Copyright © 2016 Yutaka Sugawara. All rights reserved.
export ARCH="${arch}"
archmacro=""
case "${arch}" in
armv7*) archmacro="__arm__";;
arm64*) archmacro="__arm64__";;
esac
export CFLAGS="${CFLAGS} -arch ${arch} -D${archmacro}"
export LDFLAGS="-arch ${arch}"
/usr/bin/make -k V=1 "${@}"
