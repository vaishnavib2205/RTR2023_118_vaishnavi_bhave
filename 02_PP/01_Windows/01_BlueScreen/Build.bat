cls
cl.exe /c /EHsc /I "C:\\Users\\Vaishnavi Bhave\\Downloads\\glew-2.1.0\\include" OGL.c
rc.exe OGL.rc
link.exe OGL.obj OGL.res User32.lib GDI32.lib /LIBPATH:"C:\\Users\\Vaishnavi Bhave\\Downloads\\glew-2.1.0\\lib\\Release\\x64" /SUBSYSTEM:WINDOWS
OGL.exe



