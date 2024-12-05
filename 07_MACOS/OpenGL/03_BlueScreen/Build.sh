#!/bin/sh

#  Build.sh
#
#
#  Created by Prasad Bhalkikar on 23/08/24.
#

clang -Wno-deprecated-declarations -c -o OGL.o OGL.m
mkdir -p OGL.app/Contents/MacOS
clang -o OGL.app/Contents/MacOS/OGL OGL.o -framework cocoa -framework QuartzCore -framework OpenGL



