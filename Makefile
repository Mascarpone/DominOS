CPPFLAGS=-D_FILE_OFFSET_BITS=64 -std=c99 -g -O0
CFLAGS=-Wno-unused-label -Wno-unused-function

all: tagfs printtags perfs

tagfs: tagfs.o parser/parser.o parser/HashTable.o
	gcc $^ -o $@ -lfuse
	
printtags: printtags.o parser/parser.o parser/HashTable.o
	gcc $^ -o $@ 
	
perfs: tests/perfs_Ntags_1fichier.o
	gcc $^ -o $@

%.o: %.c
	gcc -c $< -o $@ $(CPPFLAGS) $(CFLAGS) 

clean:
	$(RM) tagfs printtags perfs *.o *.log parser/*.o tests/*.o
