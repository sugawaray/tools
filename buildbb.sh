#!/bin/sh

#  buildbb.sh
#  tools
#
#  Created by Yutaka Sugawara on 2016/09/29.
#  Copyright © 2016 Yutaka Sugawara. All rights reserved.
export ARCH="${arch}"
/usr/bin/make -k V=1 "${@}"
