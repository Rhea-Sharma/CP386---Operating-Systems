//CP386 A1 Q1 (directory.c)
//Author: Rhea Sharma (shar7662)
//Email: shar7662@mylaurier.ca
//Date: Jan 26th, 2023

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>



int main(){
    /*created a variable to store user input*/
    char user_input;

    /*loop the program until the user enters 'q' to quit the program*/
    while (user_input != 'q'){
        fflush(stdout);
        /*a list of options for the user to select*/
         printf("Select option(s) appropriately by entering the number:\n");
         printf("Enter 1 for creating a directory\n");
         printf("Enter 2 for removing directory\n");
         printf("Enter 3 for printing working directory\n");
         printf("Enter 4 for changing directory one level up\n");
         printf("Enter 5 for reading contents of directory\n");
         printf("Enter 6 for closing current directory\n");
         printf("Enter q to exit the program\n");

        scanf(" %c", &user_input);

        /*program for when user wants to create a directory*/
        if (user_input == '1'){
            /*allow user to enter name of directory they would like to create*/
            printf("Enter the Directory name you want to create:\n");
            char directory_name[100];
            scanf("%s", directory_name);
    
            int create_dir = mkdir(directory_name, 0777);
            /*program for determining whether the directory is created or not*/
            if (create_dir == 0){
                printf("Directory is Created Successfully\n");
                
            } else {
                printf("ERROR: Directory was Unsuccessfully Created\n");
            }
        }
        /*program for when user wants to remove a directory*/
        else if (user_input == '2'){
            /*allow user to enter name of directory they would like to remove*/
            printf("Enter the Directory name you want to remove:\n");
            char directory_name[100];
            scanf("%s", directory_name);

            int remove_dir = rmdir(directory_name);
            /*program for determining whether the directory was removed or not*/
            if(remove_dir == -1){
                printf("ERROR: Directory was Unsuccessfully removed\n");
            } else {
                printf("Directory is Removed Successfully\n");
            }
        }
        /*program for when user wants to print the location of the working directory*/
        else if (user_input == '3'){
            /*displaying the current directory*/
            char cw_directory[500];
            getcwd( cw_directory, sizeof(cw_directory));
            printf("Current Working Directory is: %s \n", cw_directory);
        }
        /*program for when user wants to print the change in directory to its location before the operation*/
        else if (user_input == '4'){
            char directory_name[100];
            char cw_directory[500];
           
           /*display the directory before the operation*/
            getcwd(cw_directory, sizeof(cw_directory));
            printf("Working Directory Before Operation: %s \n", cw_directory);

            int change_dir = chdir("..");
            /*program for determining whether the directory was changed or not*/
            if (change_dir == -1){
                printf("ERROR: Directory Unsuccessfully Changed\n");
            } else {
                printf("Directory Changed Successfully\n");
            }

            getcwd(directory_name, sizeof(directory_name));
            printf("Working Directory After Operation: %s \n", directory_name);
        }
        /*program for when user wants to read the contents located inside the directory*/
        else if (user_input == '5'){
            char directory_name[100];
            getcwd(directory_name, sizeof(directory_name));

            DIR *open_dir = opendir(directory_name);
            struct dirent *read_dir;
            /*program for if the directory does not reach its end position */
            if (open_dir != NULL){
                /*program to list the contents located inside the directory*/
                while ((read_dir = readdir(open_dir)) != NULL){
                    printf("%s\n", read_dir->d_name);
                }
            } else{
                printf("ERROR\n");
                return -1;
            }
        }
        /*program for when user wants to close the directory*/
        else if (user_input == '6'){
            char directory_name[100];

            /*reading the directory and then closing it*/
            DIR *open_directory = opendir(directory_name);
            getcwd(directory_name, sizeof(directory_name));

            closedir(open_directory);

            printf("Directory is Closed Successfully\n");
        }


    }

    return 0;

}


