/**
CP386 A3 Q2 (fcfs.c)
Author: Rhea Sharma & Mobina Tooranisama
ID: 200576620 & 200296720
Email: shar7662@mylaurier.ca & toor6720@mylaurier.ca
Date: March 3rd, 2023
**/

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>

#define MAX_THREADS 100
 
/*Thread Info structure*/
struct threadInfo{
    int p_id;
    int arr_time;
    int burst_time;
    int completion_time;
    int waiting_time;
    int turn_around_time;
};

int main(){
    //initialize variables 
    FILE *fd;
    int n = 0;
    float avg_wt = 0, avg_tat = 0;
    int completion = 0;
    struct threadInfo threads[MAX_THREADS];

    //open file to read values
    fd = fopen("sample_in_schedule.txt", "r");

    if (fd == NULL){
        printf("Error: File unable to open!\n");
        return 1;
    }

    //output headers for values that are being displayed
    printf("Thread ID\tArrival Time\tBurst Time\tCompletion Time\t\tTurn-Around Time\tWaiting Time\n");

    //while loop for reading contents in the file until all lines are read/scanned
    while(fscanf(fd, "%d,%d,%d", &threads[n].p_id, &threads[n].arr_time, &threads[n].burst_time) != EOF){

        //calculation for thread's turn-around time and waiting time
        threads[n].completion_time = completion + threads[n].burst_time;
        threads[n].turn_around_time = threads[n].completion_time - threads[n].arr_time;
        threads[n].waiting_time = threads[n].turn_around_time - threads[n].burst_time;

        completion = threads[n].completion_time;

        //output values
        printf("%d\t\t%d\t\t%d\t\t%d\t\t\t%d\t\t\t%d\n", threads[n].p_id, threads[n].arr_time, threads[n].burst_time, threads[n].completion_time, threads[n].turn_around_time, threads[n].waiting_time);
    
        //calculating sum for average turn-around and average waiting time
        avg_wt += threads[n].waiting_time;
        avg_tat += threads[n].turn_around_time;

        //add till each line of done is read
        n++;
    
    }

    //calculate average time
    avg_wt /= n;
    avg_tat /= n;

    //output average time
    printf("\nThe average waiting time: %0.2f\n", avg_wt);
    printf("The average turn-around time: %0.2f\n", avg_tat);

    //close file
    fclose(fd);
    return 0;

}

