working C to connect directly to metepreter handler. includes unobfuscated and obfuscated c.

Main.c created by mudge, and is available here:

https://github.com/rsmudge/metasploit-loader

Command to compile via mingw/gcc:

gcc -mwindows -L c:\mingw\lib src\main.c -o loader.exe -lws2_32