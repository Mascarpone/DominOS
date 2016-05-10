CPPFLAGS=-D_FILE_OFFSET_BITS=64 -std=c99 -g -O0
CFLAGS=-Wno-unused-label -Wno-unused-function

all: tagfs printtags

tagfs: tagfs.o parser/parser.o parser/HashTable.o
	gcc $^ -o $@ -lfuse
	
printtags: printtags.o parser/parser.o parser/HashTable.o
	gcc $^ -o $@ 

%.o: %.c
	gcc -c $< -o $@ $(CPPFLAGS) $(CFLAGS) 

parse_test: parser/parser.o parser/HashTable.o tests/parse_test.o
		gcc $^ -o $@

clean:
	$(RM) tagfs *.o *.log parser/*.o printtags tests/*.o
