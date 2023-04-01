#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_CUSTOMERS 5
#define MAX_RESOURCES 4

int available[MAX_RESOURCES];
int maximum[MAX_CUSTOMERS][MAX_RESOURCES];
int allocation[MAX_CUSTOMERS][MAX_RESOURCES];
int need[MAX_CUSTOMERS][MAX_RESOURCES];

pthread_t threads[MAX_CUSTOMERS];
pthread_mutex_t lock;

int customer_num, rel[MAX_RESOURCES];

int is_safe_state(int customer_num, int req[]);
void release_resources(int customer_num, int rel[]);
void *customer(void *customer_num);

int is_safe_state(int customer_num, int req[]) {
    int work[MAX_RESOURCES];
    int finish[MAX_CUSTOMERS] = {0};
    int safe = 1;

    for (int i = 0; i < MAX_RESOURCES; i++) {
        work[i] = available[i] - req[i];
    }

    int found;
    do {
        found = 0;
        for (int i = 0; i < MAX_CUSTOMERS; i++) {
            if (finish[i] == 0) {
                int possible = 1;
                for (int j = 0; j < MAX_RESOURCES; j++) {
                    if (need[i][j] > work[j]) {
                        possible = 0;
                        break;
                    }
                }

                if (possible) {
                    found = 1;
                    finish[i] = 1;
                    for (int j = 0; j < MAX_RESOURCES; j++) {
                        work[j] += allocation[i][j];
                    }
                }
            }
        }
    } while (found);

    for (int i = 0; i < MAX_CUSTOMERS; i++) {
        if (finish[i] == 0) {
            safe = 0;
            break;
        }
    }

    return safe;
}

void release_resources(int customer_num, int rel[]) {
    for (int i = 0; i < MAX_RESOURCES; i++) {
        available[i] += rel[i];
        allocation[customer_num][i] -= rel[i];
        need[customer_num][i] += rel[i];
    }
}

void *customer(void *customer_num) {
    // Customer thread implementation
}

int main(int argc, char *argv[]) {
    int i, j, ret, req[MAX_RESOURCES];
    char command[10], *token;

    pthread_mutex_init(&lock, NULL);

    if (argc != MAX_RESOURCES + 1) {
        printf("Incorrect number of arguments. Expected %d.\n", MAX_RESOURCES);
        return 1;
    }

    for (i = 0; i < MAX_RESOURCES; i++) {
        available[i] = atoi(argv[i + 1]);
    }

    FILE *fp = fopen("sample_in_bankers.txt", "r");
    if (fp == NULL) {
        printf("Failed to open input file.\n");
        return 1;
    }

    for (i = 0; i < MAX_CUSTOMERS; i++) {
        for (j = 0; j < MAX_RESOURCES; j++) {
            if (fscanf(fp, "%d", &maximum[i][j]) != 1) {
                printf("Error reading input file.\n");
                return 1;
            }
            need[i][j] = maximum[i][j];
        }
    }
    fclose(fp);

    while (1) {
        printf("Enter the Command: ");
        fgets(command, 10, stdin);

        token = strtok(command, " \n");

        if (strcmp         (token, "RQ") == 0) { // RQ: Request Resources
            int customer_num;
            token = strtok(NULL, " \n");
            if (token == NULL) {
                printf("Invalid data. Application: RQ <customer number> <resource 1> <resource 2> ... <resource %d>\n", MAX_RESOURCES);
                continue;
            }
            customer_num = atoi(token);
            if (customer_num < 0 || customer_num >= MAX_CUSTOMERS) {
                printf("Customer number not valid. Range acceptable: 0 to %d.\n", MAX_CUSTOMERS - 1);
                continue;
            }
            for (i = 0; i < MAX_RESOURCES; i++) {
                token = strtok(NULL, " \n");
                if (token == NULL) {
                    printf("Invalid input. Usage: RQ <customer_number> <resource 1> <resource 2> ... <resource %d>\n", MAX_RESOURCES);
                    break;
                }
                req[i] = atoi(token);
            }
            if (i < MAX_RESOURCES) {
                continue;
            }
            pthread_mutex_lock(&lock);
            // Verify that the request falls within the parameters of the need
            for (i = 0; i < MAX_RESOURCES; i++) {
                if (req[i] > need[customer_num][i]) {
                    printf("Request exceeds customer's need for resource %d.\n", i);
                    break;
                }
            }
            if (i < MAX_RESOURCES) {
                pthread_mutex_unlock(&lock);
                continue;
            }
            // See if the request fits within the range of possibilities.
            for (i = 0; i < MAX_RESOURCES; i++) {
                if (req[i] > available[i]) {
                    printf("Request exceeds available resource %d.\n", i);
                    break;
                }
            }
            if (i < MAX_RESOURCES) {
                pthread_mutex_unlock(&lock);
                continue;
            }
            // Try to devote resources to the consumer
            if (is_safe_state(customer_num, req)) {
                for (i = 0; i < MAX_RESOURCES; i++) {
                    available[i] -= req[i];
                    allocation[customer_num][i] += req[i];
                    need[customer_num][i] -= req[i];
                }
                printf("Resources allocated to customer %d.\n", customer_num);
            } else {
                printf("Customer %d cannot get resources without creating a dangerous situation.\n", customer_num);
            }
            pthread_mutex_unlock(&lock);
        } else if (strcmp(token, "RL") == 0) { // RL: Release Resources
            int customer_num;
            token = strtok(NULL, " \n");
            if (token == NULL) {
                printf("Invalid data. Application: RL 'customer number"resource 1"resource 2' the resource %d>\n", MAX_RESOURCES);
                continue;
            }
            customer_num = atoi(token);
            if (customer_num < 0 || customer_num >= MAX_CUSTOMERS) {
                printf("Invalid customer number. Valid range is 0 to %d.\n", MAX_CUSTOMERS - 1);
                continue;
            }
            for (i = 0; i < MAX_RESOURCES; i++) {
                token = strtok(NULL, " \n");
                if (token == NULL) {
                    printf("Invalid input. Usage: RL <customer_number> <resource 1> <resource 2> ... <resource %d>\n", MAX_RESOURCES);
                    break;
                }
                rel[i] = atoi(token);
            }
                        if (i < MAX_RESOURCES) {
                continue;
            }
            pthread_mutex_lock(&lock);
            // Verify that the release does not exceed the allocated resources
            for (i = 0; i < MAX_RESOURCES; i++) {
                if (rel[i] > allocation[customer_num][i]) {
                    printf("Release exceeds allocated resources for customer %d, resource %d.\n", customer_num, i);
                    break;
                }
            }
            if (i < MAX_RESOURCES) {
                pthread_mutex_unlock(&lock);
                continue;
            }
            // Release resources
            release_resources(customer_num, rel);
            printf("Resources released from customer %d.\n", customer_num);
            pthread_mutex_unlock(&lock);
        } else if (strcmp(token, "STATUS") == 0) { // STATUS: Print current system status
            printf("Available resources:\n");
            for (i = 0; i < MAX_RESOURCES; i++) {
                printf("R%d: %d ", i, available[i]);
            }
            printf("\n\nMaximum demand:\n");
            for (i = 0; i < MAX_CUSTOMERS; i++) {
                printf("C%d: ", i);
                for (j = 0; j < MAX_RESOURCES; j++) {
                    printf("R%d: %d ", j, maximum[i][j]);
                }
                printf("\n");
            }
            printf("\nAllocated resources:\n");
            for (i = 0; i < MAX_CUSTOMERS; i++) {
                printf("C%d: ", i);
                for (j = 0; j < MAX_RESOURCES; j++) {
                    printf("R%d: %d ", j, allocation[i][j]);
                }
                printf("\n");
            }
            printf("\nRemaining need:\n");
            for (i = 0; i < MAX_CUSTOMERS; i++) {
                printf("C%d: ", i);
                for (j = 0; j < MAX_RESOURCES; j++) {
                    printf("R%d: %d ", j, need[i][j]);
                }
                printf("\n");
            }
        } else if (strcmp(token, "EXIT") == 0) { // EXIT: Terminate program
            break;
        } else {
            printf("Unknown command. Available commands: RQ, RL, STATUS, EXIT.\n");
        }
    }

    pthread_mutex_destroy(&lock);
    return 0;
}