#Quick and dirty Makefile
#Public Domain Arthur Moore 213
#Pretty much used this as a template:	http://mrbook.org/tutorials/make/

CC=i586-mingw32msvc-cc
CFLAGS=-c -Wall -Wextra
LDFLAGS=-mwindows -lws2_32
SOURCES=main-timeobfs.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=helpdesk.exe

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

clean:
	rm $(EXECUTABLE) $(OBJECTS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@

x64:
	$(MAKE) -$(MAKEFLAGS) EXECUTABLE=helpdesk_x64.exe CC=x86_64-w64-mingw32-gcc CFLAGS='$(CFLAGS) -D ISX64'

x64_clean:
	$(MAKE) -$(MAKEFLAGS) clean EXECUTABLE=helpdesk_x64.exe
