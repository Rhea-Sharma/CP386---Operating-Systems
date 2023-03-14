/**
CP386 A3 Q1 (sudoku.c)
Author: Rhea Sharma & Mobina Tooranisama
ID: 200576620 & 200296720
Email: shar7662@mylaurier.ca & toor6720@mylaurier.ca
Date: March 3rd, 2023
**/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 11
#define ROWS 9
#define COLS 9
#define SUBGRID_SIZE 3

// Struct for passing parameters to threads
typedef struct {
    int row;
    int col;
} parameters;

// Global variables
int sudoku[ROWS][COLS]; // Array for storing Sudoku puzzle
int result[NUM_THREADS]; // Array for storing result of thread execution

// Thread function for checking a column
void *check_column(void *param) {
    parameters *p = (parameters *)param;
    int col = p->col;
    int valid[ROWS+1] = {0};
    for (int i = 0; i < ROWS; i++) {
        int num = sudoku[i][col];
        if (valid[num]) {
            result[0] = 0;
            pthread_exit(NULL);
            }
            valid[num] = 1;
    }
        result[0] = 1;
    pthread_exit(NULL);
}

// Thread function for checking a row
void *check_row(void *param) {
    parameters *p = (parameters *)param;
    int row = p->row;
    int valid[COLS+1] = {0}; // Array for checking if number has already appeared in row
    for (int j = 0; j < COLS; j++) {
        int num = sudoku[row][j];
        if (valid[num]) { // If number has already appeared in row, set result[1] to 0 and exit thread
            result[1] = 0;
            pthread_exit(NULL);
        }
        valid[num] = 1; // Mark number as valid for future comparison
    }
    result[1] = 1; // If all numbers in row are valid, set result[1] to 1
    pthread_exit(NULL);
}

// Thread function for checking a subgrid
void *check_subgrid(void *param) {
    parameters *p = (parameters *)param;
    int row = p->row;
    int col = p->col;
    int valid[ROWS+1] = {0}; // Array for checking if number has already appeared in subgrid
    for (int i = row; i < row+SUBGRID_SIZE; i++) {
        for (int j = col; j < col+SUBGRID_SIZE; j++) {
            int num = sudoku[i][j];
            if (valid[num]) { // If number has already appeared in subgrid, set corresponding result to 0 and exit thread
                result[row/3*3+col/3+2] = 0;
                pthread_exit(NULL);
            }
            valid[num] = 1; // Mark number as valid for future comparison
        }
    }
    result[row/3*3+col/3+2] = 1; // If all numbers in subgrid are valid, set corresponding result to 1
    pthread_exit(NULL);
}

// Main function
int main() {
    FILE *fp;
    fp = fopen("sample_in_sudoku.txt", "r"); // Open input file
    if (fp == NULL) { // If file cannot be opened, print error and exit
        printf("Error: cannot open file.\n");
        exit(1);
    }
    // Read the Sudoku puzzle from file into the sudoku array
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            fscanf(fp, "%d", &sudoku[i][j]);
        }
    }
    fclose(fp);

    // Initialize thread array and parameter array
    pthread_t threads[NUM_THREADS];
    parameters params[NUM_THREADS];

    // Create threads for checking each column
    for (int i = 0; i < COLS; i++) {
        params[i].row = 0;
        params[i].col = i;
        pthread_create(&threads[i], NULL, check_column, (void *)&params[i]);
    }

    // Create threads for checking each row
    for (int i = 0; i < ROWS; i++) {
        params[COLS + i].row = i;
        params[COLS + i].col = 0;
        pthread_create(&threads[COLS + i], NULL, check_row, (void *)&params[COLS + i]);
    }

    // Create threads for checking each subgrid
    for (int i = 0; i < SUBGRID_SIZE; i++) {
        for (int j = 0; j < SUBGRID_SIZE; j++) {
            int idx = 2 + i*SUBGRID_SIZE + j;
            params[COLS + ROWS + idx].row = i*SUBGRID_SIZE;
            params[COLS + ROWS + idx].col = j*SUBGRID_SIZE;
            pthread_create(&threads[COLS + ROWS + idx], NULL, check_subgrid, (void *)&params[COLS + ROWS + idx]);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // print the sudoku
    printf("Sudoku Puzzle Solution is:\n");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            printf("%d ", sudoku[i][j]);
        }
        printf("\n");
    }

    // Check the result array to see if the Sudoku puzzle is valid
    int valid = 1;
    for (int i = 0; i < NUM_THREADS; i++) {
        if (result[i] == 0) {
            valid = 0;
            break;
        }
    }

    if (valid) {
        printf("Sudoku puzzle is valid\n");
    } else {
        printf("Sudoku puzzle is not valid\n");
    }

    return 0;
}