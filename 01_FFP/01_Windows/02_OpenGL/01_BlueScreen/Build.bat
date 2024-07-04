cls
cl.exe /c /EHsc OGL.c
rc.exe Window.rc
link.exe OGL.obj OGL.res User32.lib GDI32.lib /SUBSYSTEM:WINDOWS
OGL.exe


