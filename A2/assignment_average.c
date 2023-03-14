/*
CP386 A2 Q1 (assignment_average.c)
Author: Rhea Sharma & Mobina Tooranisama
ID: 200576620 & 200296720
Email: shar7662@mylaurier.ca & toor6720@mylaurier.ca
Date: Feb 9th, 2023
*/

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <errno.h>



int main(int num_args, char *arg_strings[]){
    //declare variables
    int i, j = 0;;
    int TA, GradTA;
    int GradTA_connect[2], TA_connect[2];
    float AGrades_avg[2];

    //create pipe for GradTAs for each process
    pipe(GradTA_connect);

    //run thru 3 chapters
    for (i = 0; i < 3; i++){
        
       //run pipe to create new process and run thru each step to either continue the operation or complete it 
        GradTA= fork();
        if (GradTA == 0){
            break;
        }

        else if (GradTA > 0){
            wait(NULL);
        }

        //error if all fail
        else{
            printf("ERROR (GradTA)");
        }
    }

    // GradTA process runs thru child aka TA process
    if (GradTA == 0){
        for (j = 0; j < 2; j++){

            //create a pipe b/w GradTA(parent) and TA(child)
            if (GradTA == 0){
                pipe(TA_connect);
                TA = fork();
            }

            //working inside the TA process and wait for completion of GradTA process
            if (TA > 0){
                wait(NULL);
                close(TA_connect[1]);
            
                //declaring variables for output 
                int array[100];
                int n = read(TA_connect[0], array, sizeof(array));
                float sum = 0;
                int k = 0;
                int grades = n/6;


                //process for GradTA
                for (k = 0; k < grades; k++){
                    sum = sum + array[k];
                    fflush(stdout);
                    
                }

                //calauting avergage and assignment
                int a_num = 2 * i + j + 1;
                AGrades_avg[j] = sum / grades;
                
                
                printf("Assignment %d - Average = %.6f\n", a_num, AGrades_avg[j]);
                close(TA_connect[0]);

                
            }

            //TA process for reading the text file
            else if (TA == 0){
                int x = 0, temp[100], array[100];
                FILE* f_grade;

                if (num_args == 1){
                    f_grade = fopen("sample_in_grades.txt", "r");   
                }
                else{
                    f_grade = fopen(arg_strings[1], "r");
                }

                //if the text file is not found empty
                if (NULL != f_grade){
                    char line_buffer[100];
                    int c = 0;
                    char space[] = " ";
                    char *string = strtok(line_buffer, space);

                    //collect all the values in column to form into arrays
                    while (NULL != fgets(line_buffer, sizeof(line_buffer), f_grade)){
                        while (string != NULL){
                            temp[c] = atoi(string);
                            c++;
                            string = strtok(NULL, space);
                        }

                        array[x] = temp[2 * i + j];
                        x++;
                    }

                    fclose(f_grade);
                }

                //close and write into the process
                close(TA_connect[0]);
                write(TA_connect[1], &array, x*sizeof(x));
                close(TA_connect[1]);

                return 0;
            }

            else{
                printf("ERROR (TA)");
            }
        }   

    }

    return 0;

}