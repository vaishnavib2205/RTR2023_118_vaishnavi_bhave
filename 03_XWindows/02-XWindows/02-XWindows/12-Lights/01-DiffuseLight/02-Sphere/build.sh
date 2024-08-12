g++ -c -o OGL.o OGL.cpp
g++ -o OGL -L /usr/lib/x86_64-linux-gnu  -L /usr/lib64/  OGL.o -lX11 -lGL -lGLEW -lSOIL libSphere.so
