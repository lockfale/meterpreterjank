# Meterpreter Payload Stage 1 with Obsfuscation and Evasion

main-timeobfs.c contains the code from [mudge's main.c](https://github.com/rsmudge/metasploit-loader) with a number of obsfuscation and evasion techniques applied. 

## Usage
Change the IP address in main-timeobfs.c near line 107 to whatever you would set your LHOST to when setting up your payload. Currently this only works with windows/meterpreter/reverse_tcp payloads. We also recommend that you use "set EnableStageEncoding true" to enable shikata_ga_nai encoding of the stage 2 payload.

## Compiling
### Windows 
gcc -mwindows -L c:\mingw\lib main-timeobfs.c -o loader.exe -lws2_32
### Linux
apt-get install mingw32
i586-mingw32msvc-cc -mwindows main-timeobfs.c -o loader.exe -lws2_32


