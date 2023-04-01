/**
--------------------------------------
CP386 A5 Q1 (banker.c)
Authors: Mobina Tooranisama & Rhea Sharma
IDs: 200296720 & 200576620
Emails: toor6720@mylaurier.ca & shar@mylaurier.ca
Date: March 31st, 2023
--------------------------------------
**/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>


#define MAXCHARACTER 21


// Global Variables
int THREADLEN = 0;
int RESOURCENUM = 0;
int **MAX;
int **ALLOC_CONST;
int *AVAILABLE;

// Function Prototypes
int *get_max_resource_line(char *line);
void read_file_to_max_arr(FILE** file, int **max);
void determine_thread_arr_len(FILE **file);
void request_resources(char* buf);
void release_resources(char* buf);
void run();

// Function to get the maxmimum number of resources from the file
int *get_max_resource_line(char *line) { 
    int i = 0;
    char *token = strtok(line, ",");
    int *thread_arr = malloc(sizeof(int) * RESOURCENUM);
    thread_arr[i] = atoi(token);

    while ((token = strtok(NULL, ","))) {    
        i += 1;
        thread_arr[i] = atoi(token);
    }

    return thread_arr;
}

// function to read the file and store the maximum resources into a 2D array
void read_file_to_max_arr(FILE **file, int **max) { 
    ssize_t r;
    size_t len = 0;
    char *line;
    int *thread_arr; 
    int i = 0; 

    while ((r = getline(&line, &len, *file)) != -1) {
        *(MAX + i) = malloc(sizeof(int) * RESOURCENUM);
        thread_arr = get_max_resource_line(line);

        for (int j = 0; j < RESOURCENUM; j++) { 
            MAX[i][j] = thread_arr[j];
        }

        i += 1;
    }

    return;
}


// function to determine the length of the thread array
void determine_thread_arr_len(FILE **file) { 
    ssize_t r;
    size_t len = 0;
    char *line;
     
    while ((r = getline(&line, &len, *file)) != -1) { 
        THREADLEN += 1;
    }

    rewind(*file);

    return;
}



// function to request resources 

void request_resources(char *buf) {
    int i;
    bool deny = false;
    
    char* token = strtok(buf, " ");
    token = strtok(NULL, " ");
    int customer_number = atoi(token);
    int customer_resources[RESOURCENUM];
    i = 0;

    while ((token = strtok(NULL, " "))) {   
        customer_resources[i] = atoi(token);
        i++;
    }

    for (i = 0; i < RESOURCENUM; i++) {
        if (customer_resources[i] > MAX[customer_number][i]) {
            deny = true;
        }
    }

    if (deny == false) {
        for (i = 0; i < RESOURCENUM; i++) {
            if (customer_resources[i] > AVAILABLE[i]) {
                deny = true;
            }
        }
    }

    if (deny == false) {
        for (i = 0; i < RESOURCENUM; i++) {
            AVAILABLE[i] -= customer_resources[i];
            ALLOC_CONST[customer_number][i] += customer_resources[i];
        }
    }

    if (deny == true) {
        printf("State is not safe, and request is not satisfied\n");
    } else {
        printf("State is safe, and request is satisfied\n");
    }

    return;
}


//Function to release resources 

void release_resources(char *buf) {
    char* token = strtok(buf, " ");
    int temp[RESOURCENUM];
    token = strtok(NULL, " "); 
    int num_thread = atoi(token);
    int i = 0;

    while ((token = strtok(NULL, " "))) {   
        temp[i] = atoi(token);
        i++;
    }

    printf("    To release: ");
    for (int j = 0; j < RESOURCENUM; j++) { 
        AVAILABLE[j] += temp[j];
        ALLOC_CONST[num_thread][j] -= temp[j];
        printf("%d ", temp[j]);
    }

    printf("\n");

    printf("    Now Available Resources: ");
    for (int j = 0; j < RESOURCENUM; j++) { 
        printf("%d ", AVAILABLE[j]);
    }

    printf("\n");

    printf("    Resources still held by thread: ");
    for (int j = 0; j < RESOURCENUM; j++) { 
        printf("%d ", ALLOC_CONST[num_thread][j]);
    }

    printf("\n");
}



//function when user enter the run command
void run(){
    int thread_valid;
    int remaining_threads[THREADLEN];
    int remaining_thread_count = THREADLEN;
    int num_thread;
    int counter = 0;

    for (int i = 0; i < THREADLEN; i++) { 
        remaining_threads[i] = i;
    }

    printf("Safe Sequence: ");
    for (int i = 0; i < THREADLEN; i++) { 
        printf("%d ", remaining_threads[i]);
    }
                
    printf(" \n");
                
    while (remaining_thread_count > 0 && counter < THREADLEN) { 
        for (int i = 0; i < remaining_thread_count; i++) { 
            thread_valid = 1;
            num_thread = remaining_threads[i];

            for (int j = 0; j < RESOURCENUM; j++) { 
                if (AVAILABLE[j] < (MAX[num_thread][j] - ALLOC_CONST[num_thread][j])) {
                    thread_valid = 0;
                }
            }

            if (thread_valid == 1) {
                for (; i < remaining_thread_count - 1; i++) { 
                    remaining_threads[i] = remaining_threads[i + 1];
                }

                i++;
                remaining_thread_count -= 1;
                printf("--> Customer/Thread %d \n", num_thread);
                printf("    Allocated resources: ");
                
                for (int i = 0; i < RESOURCENUM; i++) { 
                    printf("%d ", ALLOC_CONST[num_thread][i]);
                }           
                            
                printf(" \n");

                printf("    Needed: ");
                for (int i = 0; i < RESOURCENUM; i++) {  
                    printf("%d ", MAX[num_thread][i] -  ALLOC_CONST[num_thread][i]);
                }

                printf(" \n");

                printf("    Available: ");
                for (int i = 0; i < RESOURCENUM; i++) { 
                    printf("%d ", AVAILABLE[i]);
                }

                printf(" \n");

                printf("    Thread has started \n");
                printf("    Thread has finished \n");
                printf("    Thread is releasing resources \n");
                for (int i = 0; i < RESOURCENUM; i++) { 
                    AVAILABLE[i] += ALLOC_CONST[num_thread][i];
                }

                printf("    New available: ");
                for (int i = 0; i < RESOURCENUM; i++) { 
                    printf("%d ", AVAILABLE[i]);
                }

                printf(" \n");
            }
        }
        counter++;
    }
}


//Main function 

int main(int argc, char **argv) {
    FILE* file;
    file = fopen("sample_in_banker.txt" , "r");

    AVAILABLE = malloc(sizeof(int) * RESOURCENUM);

    if (argc > 1) {
        for (int i = 1; i < argc; i++) { 
            AVAILABLE[i-1] = atoi(argv[i]);
            RESOURCENUM += 1; 
        }
    } else { 
        exit(0);
    }

    determine_thread_arr_len(&file);
    MAX = malloc(sizeof(int*) * THREADLEN);
    read_file_to_max_arr(&file, MAX);
    ALLOC_CONST = malloc(sizeof(int*) * THREADLEN);

    printf("Number of Customers: %d \n", THREADLEN);
    printf("Currently Available Resources: ");
    for (int i = 0; i < RESOURCENUM; i++) { 
        printf("%d ", AVAILABLE[i]);
    }

    printf(" \n");

    printf("Maximum resources from file: \n");
    for (int i = 0; i < THREADLEN; i++) { 
        ALLOC_CONST[i] = calloc(RESOURCENUM, sizeof(int));
        for (int j = 0; j < RESOURCENUM; j++) { 
            printf("%d ", MAX[i][j]);
        }
        printf("\n");
    }

    while (1) {
        char buf[MAXCHARACTER];                  
        fputs ("Enter Command: ", stdout);   
        while (fgets(buf, MAXCHARACTER, stdin)) {  
            char *buff = malloc(sizeof(char) * MAXCHARACTER);
            strcpy(buff, buf);
            char *prefix = strtok(buf, " \n");
            if (strcmp(prefix, "Exit") == 0) {
                exit(0);
            } else if (strcmp(prefix, "Run") == 0) {
                run(); 

            } else if (strcmp(prefix, "Status") == 0) { 
                printf("Available Resources: \n");
                int i, j; 
                for (i = 0; i < RESOURCENUM; i++) { 
                    printf("%d ", AVAILABLE[i]);
                }
                printf("\n");

                printf("Max Resources: \n");
                for (i = 0; i < THREADLEN; i++) { 
                    for (j = 0; j < RESOURCENUM; j++) { 
                        printf("%d ", MAX[i][j]);
                    }
                    printf("\n");
                }

                printf("Allocated Resources: \n");
                for (i = 0; i < THREADLEN; i++) {
                    for (j = 0; j < RESOURCENUM; j++) { 
                        printf("%d ", ALLOC_CONST[i][j]);
                    }
                    printf("\n");
                }

                printf("Need Resources: \n");
                for (i = 0; i < THREADLEN; i++) { 
                    for (j = 0; j < RESOURCENUM; j++) { 
                        printf("%d ", MAX[i][j] - ALLOC_CONST[i][j]);
                    }
                    printf("\n");
                }
            } else if (strcmp(prefix, "RQ") == 0) { 
                request_resources(buff);
            } else if (strcmp(prefix, "RL") == 0) { 
                release_resources(buff);
            } else { 
                printf("Invalid input, use one of RQ, RL, Status, Run, Exit \n");
            }
            fputs ("Enter Command: ", stdout);
        }
    }
}