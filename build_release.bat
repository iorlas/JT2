@echo off
msbuild jt2.vcxproj /p:Configuration=Release
copy Release\jt2.dll jt2.dll
pause