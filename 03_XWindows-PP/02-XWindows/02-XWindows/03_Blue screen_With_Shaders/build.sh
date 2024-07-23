gcc -c -o OGL.o -I /usr/include OGL.cpp
gcc -o OGL -L/usr/lib/x86_64-linux-gnu OGL.o -lX11 -lGL -lGLEW
