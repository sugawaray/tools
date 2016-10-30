#!/bin/sh

#  buildbb.sh
#  tools
#
#  Created by Yutaka Sugawara on 2016/09/29.
#  Copyright © 2016 Yutaka Sugawara. All rights reserved.

cplibs() {
    for d in coreutils libbb editors archival archival/libarchive coreutils/libcoreutils findutils; do
	d2=$(echo $d | sed -e 's#/#_#g')
	cp "${objdest}/${d}/lib.a" "${SRCROOT}/libbusybox_${d2}_${arch}.a"
    done
}

cflags="${CFLAGS}"

for a in ${ARCHS}; do
	arch=$a
	export arch;
	export ARCH="${arch}"
	archmacro=""

	case "${arch}" in
	armv7*) archmacro="__arm__";;
	arm64*) archmacro="__arm64__";;
	i386*) archmacro="__i386__";;
	x86_64*) archmacro="__x86_64__";;
	esac

	export CFLAGS="${cflags} -arch ${arch} -D${archmacro} -fno-common"
	export LDFLAGS="-arch ${arch}"
	export objdest="${OBJECT_FILE_DIR}/${arch}"

	/usr/bin/make -k V=1 "${@}" && cplibs
done

