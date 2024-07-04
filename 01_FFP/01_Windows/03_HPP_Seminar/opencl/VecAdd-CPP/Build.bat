cls
cl.exe /c /EHsc /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.3\include" VecAdd.cpp
link.exe VecAdd.obj /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v12.3\lib\x64" OpenCL.lib /MACHINE:X64 /SUBSYSTEM:CONSOLE