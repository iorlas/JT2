@echo off
msbuild jt2.vcxproj /p:Configuration=Debug
copy Debug\jt2.dll jt2.dll
pause