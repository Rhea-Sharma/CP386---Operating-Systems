//CP386 A1 Q2 (filecopy.c)
//Author: Rhea Sharma (shar7662)
//Email: shar7662@mylaurier.ca
//Date: Jan 26th, 2023

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int num_args, char *arg_strings[]){
    /*declaring all variables*/
    char buffer[1024];
    int file1 = open(arg_strings[1], O_RDONLY);
    int file2 = open(arg_strings[2], O_WRONLY | O_CREAT | O_TRUNC, 0777);
    int read_file = read(file1, buffer, sizeof(buffer));
    int write_file = write(file2, buffer, read_file);

    /*checking to ensure that they are the required amount of parameters*/
    if (num_args != 3){
        /*if not the right amount of parameters, error message will display*/
        printf("Insufficient parameters passed.\n");
        return -1;
    }

    /*checking to see if source file exists and can be read from*/
    if (file1 == -1){
        printf("open");
        return -1;
    }

    /*checking to see if the file destination is created*/
    if (file2 == -1){
        printf("open");
        return -1;
    }

    
        /*checking to see if the new files created match each other or not*/
    if (write_file != read_file){
        printf("write");
        return -1;
    } else{
        /*displays once the copying and writing is done*/
        fprintf(stdout, "The contents of file '%s' has been successfully copied into '%s' file", arg_strings[1], arg_strings[2]);
        }
    

    if (read_file == -1){
        printf("read");
        return -1;
    }

    /*closing the source and destination files directory*/
    if (close(file1) == -1){
        printf("close");
        return -1;
    }

    if (close(file2) == -1){
        printf("close");
        return -1;
    }

    return 0;

}