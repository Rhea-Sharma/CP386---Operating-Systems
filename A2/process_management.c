#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

// prototypes
void write_output(char *command, char *output);
void input_to_mem(char *memory_pointer, char *input_file, int length);
void exe_cmd(char *memory_pointer);

// constants
const int SHARED_MEM_SIZE = 2048;
const char *SHARED_MEM_FILE = "SHARED_MEM"; 
const char *OUTPUT_FILE = "output.txt";

int main(int argc, char *arguments[]) {
    if (argc > 1) {
        int memory_id = shm_open(SHARED_MEM_FILE, O_CREAT | O_RDWR, 0666);
        if (memory_id == -1) {
            printf("\nShared Memory Failure: %s!\n", strerror(errno));
            exit(-1);
    }
        ftruncate(memory_id, SHARED_MEM_SIZE); //Set the size of the shared memory
        char *memory_pointer = mmap(0, SHARED_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memory_id, 0); //Map the shared memory to a pointer
        if (memory_pointer == MAP_FAILED) {
            printf("\nMapping Failure: %s!\n", strerror(errno));
            exit(-1);
        }
        char *input_file = arguments[1]; //Assign the input file name to the input_file variable
        input_to_mem(memory_pointer, input_file, SHARED_MEM_SIZE); //Call the input_to_mem function
        exe_cmd(memory_pointer); //Call the exe_cmd function
    }
    else {
        printf("\nPlease Give an Input File!\n");
        exit(-1);
    }
    return 0;
}

// function that writes the output to a file
void write_output(char *command, char *output) {
     FILE *fp;
     fp = fopen("output.txt", "a");
     fprintf(fp, "The output of: %s : is\n", command);
     fprintf(fp, ">>>>>>>>>>>>>>>\n%s<<<<<<<<<<<<<<<\n", output);
     fclose(fp);
}


// function that reads the input file and writes it to the shared memory
void input_to_mem(char *memory_pointer, char *input_file, int length) {
    char *pointer = memory_pointer;
    pid_t child_pid = fork();

    if (child_pid == 0) {
        FILE *inputfile = fopen(input_file, "r");
        char line[1024];

        while (fgets(line, sizeof(line), inputfile)) {
            if (pointer - memory_pointer + strlen(line) >= length) {
                printf("\nShared memory is not big enough to store the input data.\n");
                exit(-1);
            }
            pointer += sprintf(pointer, "%s", line);
            *pointer = '\0';
        }
        fclose(inputfile);
        exit(0);
    } else if (child_pid > 0) {
        int state;
        wait(&state);
        if (WEXITSTATUS(state) == -1) {
            perror("\nError Waiting for Child Process! Please Try Again!\n");
            exit(-1);
        }
        printf("\nWaiting for Child Process Finished!\n");
    } else {
        printf("\nError Forking Child Process for Writing Input to Memory!\n");
        exit(1);
    }
}


//function that executes commands stored in memory
void exe_cmd(char *memory_pointer) {
    char *pointer = memory_pointer; //Assigns the value of memory_pointer to the pointer variable
    char *pipe_path = "/tmp/pipes";
    
    mkfifo(pipe_path, 0666); //Create a named pipe
    pid_t child_pid = fork(); //Create a child process
    
    if (child_pid == 0) {
        int pipe_descriptor = open(pipe_path, O_WRONLY); //Open the pipe for writing for the child process
        char res[SHARED_MEM_SIZE]; //Create an array to store the result
        for (int i = 0; i < 64; i += 1) {
            char data = (char)pointer[i];
            strncat(res, &data, 1); //Concatenate each character of the data to the result array
        }
        char *result_pt = strtok(res, "\r\n");
        while (result_pt) {
            char *cmd = strtok(result_pt, " ");
            char *args[64];
            int i = 0;
            while (cmd) {
                args[i] = cmd;
                cmd = strtok(NULL, " ");
                i += 1;
            }
            args[i] = NULL;
            int fd[2];
            pipe(fd);
            pid_t pid = fork();
            if (pid == 0) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
                execvp(args[0], args);
                exit(0);
            }
            else if (pid > 0) {
                int state;
                wait(&state);
                if (WEXITSTATUS(state) == -1) {
                    perror("\nError While Waiting!\n");
                    exit(-1);
                }
                close(fd[1]);
                char output[1024];
                read(fd[0], output, 1024);
                write_output(result_pt, output);
            }
            else {
                printf("\nForking Failure!\n");
                exit(1);
            }
        result_pt = strtok(NULL, "\r\n");
        write(pipe_descriptor, res, SHARED_MEM_SIZE + 1); //Write the result to the pipe
        close(pipe_descriptor); //Close the write end of the pipe
        }
    }else if (child_pid > 0) {
        int pipe_descriptor = open(pipe_path, O_RDONLY); //Open the pipe for reading for the parent process
        int state;
        wait(&state); //Wait for the child process to finish
        if (WEXITSTATUS(state) == -1) { //Check if the wait function encountered an error
            perror("\nError While Waiting!\n");
            exit(-1);
        }
        char res[SHARED_MEM_SIZE];
        read(pipe_descriptor, res, SHARED_MEM_SIZE);
        FILE *out_fn = fopen(OUTPUT_FILE, "w");
        char *pt = strtok(res, "\r\n");
        while (pt) {
            fprintf(out_fn, "%s\n", pt);
            pt = strtok(NULL, "\r\n");
        }
        fclose(out_fn);
        close(pipe_descriptor); //Close the read end of the pipe
    }
    else {
        printf("\nForking Failure!\n");
        exit(1);
    }
}