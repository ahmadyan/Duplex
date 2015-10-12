# Duplex Makefile version 0.1
CC=c++
CFLAGS=-O2 -w -Wno-write-strings -std=c++11 -stdlib=libc++
BOOST=/usr/local/include/
SRC=main.cpp duplex.cpp state.cpp configuration.cpp system.cpp graph.cpp search.cpp
OUT=-o ../bin/duplex
all: main

main: $(SRC)
	$(CC)  $(OUT) $(CFLAGS) $(SRC)  -I$(BOOST)

test: 
	../bin/duplex
	
bison: parser.yy
#	bison -d parser.yy

flex: tokenizer.l
#	flex tokenizer.l

sim:
	$(CC) $(CFLAGS) ScriptGenerator.cpp -o ScriptGenerator.exe -I$(BOOST)

analysis:
	$(CC) $(CFLAGS) ResultAnalysis.cpp -o ResultAnalysis.exe -I$(BOOST)