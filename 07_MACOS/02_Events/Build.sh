#!/bin/sh

#  Build.sh
#
#
#  Created by Prasad Bhalkikar on 23/08/24.
#

clang -c -o Window.o Window.m
mkdir -p Window.app/Contents/MacOS
clang -o Window.app/Contents/MacOS/Window Window.o -framework cocoa


