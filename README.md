# MsShellcodeDev
The Solution to help Developing &amp; Debugging as C &amp; Deploying in Shellcode.

# Requirement - To Build the Solution
Visual Studio 2026 is the only IDE could be used to compile the solution (as slnx not compatible with older version), hardware & software requirements same as Visual Studio 2026.
For allowing ShellcodeTestLoader to be executed on Windows XP x86/x64 version, v140xp or v141xp toolsets (though deprecated) should be installed in addtion to the C Desktop Development Core Components.

# Requirement - To Build the Shellcode
To execute "Vcxproj to Shellcode Compiler" to convert & compile the vcxproj to shellcode, Windows 7 & Visual Studio 2015 (or higher version OSes/IDEs) are required, so the Software/Hardwre requirements just same as Win 7 & VS 2015. The converted project should be developed using pure C & win32/win64 APIs without class & global variable definitions.

# Usage
Both "ShellcodePrototype" & "ShellcodeSample" are programs could be compiled & executed, the outcome are "run calc.exe & pop MessageBox" and "get system info & pop MessageBox;" "Hash of Func & LibName Calculator" could be used to pick existing dll file & export hash values DLL Name (case-insensitive) & API Names (case-sensitive) and save into a csv file; "ShellcodeTestLoader" is the tool to test extracted binary shellcode files using VirtualAlloc API, just be careful that x86/x64 build of loader could only test corresponding architecture shellcode; "Vcxproj to Shellcode Compiler" is GUI application, since the IDE version & "vcvars32.bat" path could be auto detected, just pick the vcxproj & assign the output name (or not, in this case the name of this converted project will be used) & clik on the "Compile" button, the outcome will be there, 2 binary files with no error message or many error messages.

# Additional Info
To test the compiled shellcode on windows 2000, be noticed that Visual Studio 2008 was "officially" the last version of MS IDE supporting Windows 2000, please find the Express version of it to install, then migrate "ShellcodeTestLoader" to VS2008 with "careful" modification, then you could get a Windows 2000 executable Shellcode Test Loader.
