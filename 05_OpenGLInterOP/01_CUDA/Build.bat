cls
nvcc.exe -c -o SineWave.cu.obj SineWave.cu
cl.exe /c /EHsc /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.3\include" /I "C:\\Users\\Vaishnavi Bhave\\Downloads\\glew-2.1.0\\include"  OGL.cpp
rc.exe OGL.rc
link.exe OGL.obj SineWave.cu.obj OGL.res User32.lib GDI32.lib /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.3\lib\x64" /LIBPATH:"C:\\Users\\Vaishnavi Bhave\\Downloads\\glew-2.1.0\\lib\\Release\\x64" /SUBSYSTEM:WINDOWS
OGL.exe
