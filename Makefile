CPPFLAGS=-D_FILE_OFFSET_BITS=64
CFLAGS=-Wno-unused-label -Wno-unused-function

all: tagfs

tagfs: tagfs.o parser.o HashTable.o
	gcc $^ -o $@ -lfuse

%.o: %.c
	gcc -c $< -o $@ $(CPPFLAGS) $(CFLAGS)

clean:
	$(RM) tagfs *.o *.log
