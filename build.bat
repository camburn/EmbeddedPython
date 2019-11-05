CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\Tools\VsDevCmd.bat"
cl .\src\embed.c -I./env/include/ D:/python36/libs/python36.lib /Fe.\build\embed.exe /Fo.\build\embed.obj