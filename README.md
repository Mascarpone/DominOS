# How to use the tagfs filesystem (by team DominOS)

## Prerequisites
You must have fuse installed.
The folder from which you will build the daemon must have a `.tags` file with the good syntaxe.

For example:

    # comment
    sharedtag1
    sharedtag2
    
    [file1]
    tag1
    tag2
    
Note that lines should not go over 150 characters.



## Compiling everything
Launch a terminal and enter the following command:

    make
  
  
  
## Playing with tagfs

### Starting the fuse daemon
Launch the tagfs filesystem in the `mnt` folder based on the content of the `images` folder:

    ./tagfs images/ mnt/


### Adding a tag
Tags are automatically added.
Please use *ln* to add new tags.
You can add multiple tags at the same time.
  
    ln mnt/ipb.jpeg mnt/test/ipb.jpeg
    

### Deleting a tag
If you want to delete a tag from all the files, use *rmdir*.
  
    rmdir mnt/test/

If you want to remove a tag from a specific file, use *rm*.
You can delete multiple tags at the same time.

    rm mnt/test/ipb.jpeg

Tags with no remaining files refering to them will be deleted.


### Renaming a tag
Use *mv*.
You can delete and add multiple tags at the same time.

    mv mnt/test/ipb.jpeg mnt/test2/test3/


### Printing the tags of a file
    ./printtags mnt/ipb.jpeg


### Stopping the fuse daemon
In the root folder of the project:

    fusermount -u mnt



## Testing tagfs
You can launch all the bash scripts from the `tests/` folder in the root folder of the project.
You can launch **testparser** in the root folder of the project.
You can launch **testperfs** in the `mnt` folder after starting the fuse daemon in it.



## Cleaning everything
    make clean

