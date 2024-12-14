cls
cl.exe /c /EHsc D3D.cpp
rc.exe D3D.rc
link.exe D3D.obj D3D.res User32.lib GDI32.lib /SUBSYSTEM:WINDOWS
D3D.exe


