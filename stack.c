/**
CP386 A4 Q1 (stack.c)
Author: Rhea Sharma & Mobina Tooranisama
ID: 200576620 & 200296720
Email: shar7662@mylaurier.ca & toor6720@mylaurier.ca
Date: March , 2023
**/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10

//StackNode struct
typedef struct Node {
    int data;
    struct Node *next;
} StackNode;

StackNode *top;

//function prototypes
pthread_mutex_t mutex_lock;
void push(int v, StackNode **top);
int pop(StackNode **top);
int is_empty(StackNode *top);


//push 
void push(int v, StackNode **top){
    StackNode *new_node;
    printf("Thread is running Push() operation fro value: %d\n", v);

    new_node = (StackNode *)malloc(sizeof(StackNode));
    new_node->data = v;
    new_node->next = *top;
    *top = new_node;
}

//pop function
int pop(StackNode **top){
    StackNode *temp;

    if(is_empty(*top)){
        printf("Stack empty \n");
        return 0;
    }
    else{
        int data = (*top)->data;
        printf("Thread is runnning Pop() operation and value is: %d\n", data);
        temp = *top;
        *top = (*top)->next;
        free(temp);

        return data;
    }
}

//is_empty function to Check if top is NULL
int is_empty(StackNode *top){
    if (top == NULL){
        return 1;
    }
    else{
        return 0;
    }
}

//Thread's push function
void* thread_push(void *args){
    pthread_mutex_trylock(&mutex_lock);

    //int i;
    int *threadId = (int *)args;
    
    push(*threadId + 1, &top);

    pthread_mutex_unlock(&mutex_lock);

    return NULL;
}

//Thread's pop function
void* thread_pop(){
    pthread_mutex_trylock(&mutex_lock);
    pop(&top);
    pthread_mutex_unlock(&mutex_lock);

    return NULL;
}

//main function
int main(void){
    int thread_arg[NUM_THREADS];
    int i, j;
    //StackNode *top = NULL;
    pthread_t threads_push[NUM_THREADS];
    pthread_t threads_pop[NUM_THREADS];

    //creating pop threads
    for (i = 0; i < NUM_THREADS; i++){
        thread_arg[i] = i;
        pthread_create(&threads_push[i], NULL, thread_push, (void *)&thread_arg[i]);
    }

    //create pop threads
    for (i = 0; i < NUM_THREADS; i++){
        thread_arg[i] = i;
        pthread_create(&threads_pop[i], NULL, thread_pop, NULL);
    }

    //join push threads
    for (j = 0; j < NUM_THREADS; j++){
        pthread_join(threads_push[j], NULL);
    }

    //join pop threads
    for (j = 0; j < NUM_THREADS; j++){
        pthread_join(threads_pop[j], NULL);
    }

    pthread_exit(NULL);
    return 0;

   
}




