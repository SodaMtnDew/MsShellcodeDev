@call "C:\Program Files\Microsoft Visual Studio\18\Professional\VC\Auxiliary\Build\vcvars32.bat"

msbuild ShellcodePrototype.vcxproj -p:Configuration=Release;Platform=Win32
msbuild ShellcodePrototype.vcxproj -p:Configuration=Release;Platform=x64
