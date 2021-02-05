# Terminal based File Explorer

## Overview

File Explorer works in 2 modes - 
* Normal mode - this is default mode that the explorer will run.
* Command mode - shell commands can be executed here.

Last line of the terminal is used as a status bar. In normal mode, the current directory is displayed in status bar and in command mode, the user input is shown.

ncurses is used to maintain windows.

## OS used
MAC OS

## How to run ?

`make`

`./a.out`

Makefile is included which has the command to run the file along with the dependent header file.

Press **q** to **q**uit the program.

## Working

### Normal mode

1. Files and directories in the current folder will be displayed along with their permissions, file size and last modified time.
2. An extra entry (**../**) is displayed at the top to go to parent directory.
3. User can navigate up and down the directories uptill root folder of his file system i.e., till /
4. If there are more files in a directory than the full screen size of terminal then user can scroll down to see other files and folders.
5. **Up and down arrows** can be used to move up and down in the terminal respectively.
6. To open a file/directory, move the cursor to that file (using up/down arrows) and press **ENTER**.
   * Files will be opened using default file editors in system that supports the file type.

### Command mode

To enter into command mode, press **e**.
To execute a command, press **ENTER**

Supported commands are - 

1. copy
    * syntax -> copy <source file(s)> <destination directory>
2. move
    * syntax -> move <source file(s)> <destination directory>
3. rename
    * syntax -> rename <old filename> <new filename>
4. create file
    * syntax -> create_file <file name> <destination path>
5. create directory
    * syntax -> create_dir <directory name> <destination path>
6. delete file
    * syntax -> delete_file <file path>
7. delete directory
    * syntax -> delete_dir <file path>
8. goto
    * syntax -> goto <directory path>
9. search
    * syntax -> search <file name> (or)
                search <directory name>

If changes are made with respect to directory that is currently being displayed using commands, then they'll be reflected. goto command will change to the user specified directory and lists files and folders.

To exit from command mode and go into normal mode, press **ESCAPE**.
