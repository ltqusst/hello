#!/bin/sh -x

gcc -Wall $1 -o `basename $1 .c` $(pkg-config --cflags --libs gstreamer-1.0)

