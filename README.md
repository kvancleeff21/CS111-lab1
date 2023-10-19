## UID: 123456789

## Pipe Up

pipe.c is a program that emulates the behavior of the pipe shell operator, allowing  programs to act as the input for the output of other programs. 

## Building

You can build my program by using the make command or when I was testing my program on vscode outside of the virtual machine, I used gcc to compile my program
ex: gcc -o pipe pipe.c

## Running

```
./pipe ls cat wc would output 
5 5 44 to the terminal
```

## Cleaning up

Running make clean would clean up all binary files
