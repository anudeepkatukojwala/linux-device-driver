/**************************************************************
* Class:  CSC-415-02 Fall 2022
* Name: Anudeep Katukojwala
* Student ID: 922404701
* GitHub UserID: anudeepkatukojwala
* Project: Assignment 6 – Device Driver
*
* File: katukojwala_anudeep_HW6_main.c
*
* Description: This program is to build a loadable device driver 
*              which can be loaded and unloaded into and from 
*              the kernel with some functionalities such as 
*              open, release, read, write, and IOCTL. To test
*              the device driver functionality, we are required
*              to write a test program to test every functionality
*              of our device driver.
*
**************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<sys/ioctl.h>
 
#define BUFFER_SIZE 500    
 
int main(){

   int encryptionKey = 5; //Encryption key initially set to 5
   char readString[BUFFER_SIZE]; //Stored into this when a string is read from device file
   char currentString[BUFFER_SIZE]; //User entered string, that is written to device file is stored
   int isEncrypted = 0; //Used to check if a string is already encrypted or not
   int keyChange = 0; //Used to check if the encryption key change is allowed or not
   int isStringEntered =0; //Used to check if the string is entered atleast once
   int returnOfRead; //To store return values from read function
   int returnOfWrite; //To store return values from write function
   int returnOfClose; //To store return of close function


   //Open the device file of encryptor device driver 
   //with read and write permissions
   int fileDescriptor = open("/dev/encryptor", O_RDWR);   
   if (fileDescriptor < 0){
      printf("Error while opening device file /dev/encryptor\n");
      return -1;
   }
   //Loop controller
   int lc = 0; //used to decide whether to continue or exit the loop

   //Input mode of operation to perform various functions including IOCTL functions
   while(lc != 4) {

      //Take input for user and perform required operation
      printf("\n*************\n");
      printf("If this is the first time, please select 2 to enter a string\n\n");
      printf("Please enter the mode of operation desired:\n");
      printf("0 to Encrypt the string\n1 to Decrypt the string\n");
      printf("2 to enter new string\n3 to set encryption key\n4 to end the program\n\n");
      printf("*************\n");
      scanf("%d", &lc);

      //verify if user entered valid value for required operation
      if(!(lc==0 || lc==1 || lc==2 || lc==3 || lc==4)){
        printf("Incorrect input for performing operations\nExiting the program...\n");
        returnOfClose = close(fileDescriptor); //close device file before exit
        if(returnOfClose < 0){
        printf("Error while closing device file /dev/encryptor\n");
        return -1;
        }
        exit(0);
      }

      //perform required operation based on user input
      switch(lc){
        case 0:
            //Encrypts only if string is entered atleast once in this session
            if(isStringEntered==0){
                printf("\nPlease enter the string atleast once\n");
            }
            else{
                isEncrypted++; //Keep track of the level of encryption

                //call device file ioctl function to encrypt string
                ioctl(fileDescriptor, lc, encryptionKey);
            
                //Filling readString with some constant value
                memset(readString, 0, BUFFER_SIZE);

                //read from device file encrypted string
                returnOfRead = read(fileDescriptor, readString, BUFFER_SIZE);

                if(returnOfRead < 0){
                perror("Could not read the encrypted string from encryptor device file\n");
                return errno;
                }
                printf("\nRead from device file");
                printf("; String after %d level encryption: %s\n", isEncrypted, readString);

                keyChange=1; //sets to 1 to avoid changing key in the middle of the session
            }            

            break;
        case 1:
            //Decrypts only if string has been entered and that string is encrypted atleast once
            if(isStringEntered==0){
                printf("\nPlease enter the string first\n");
            }
            else{
                if(isEncrypted<=0){
                    printf("\nString is already in decrypted form\n\n");
                }
                else{
                    //call device file ioctl function to decrypt string
                    ioctl(fileDescriptor, lc, encryptionKey);
            
                    //Filling readString with some constant value
                    memset(readString, 0, BUFFER_SIZE);

                    //read from device file decrypted string
                    returnOfRead = read(fileDescriptor, readString, BUFFER_SIZE);

                    if(returnOfRead < 0){
                    perror("Could not read the decrypted string from encryptor device file\n");
                    return errno;
                    }

                    isEncrypted--; //decrement level of encryption to have right encryption level
                    printf("\nRead from device file; String after decryption: %s,", readString);
                    printf(" can be decrypted %d more times\n", isEncrypted);
                
                }
                keyChange=1; //To avoid changing key in the middle of the session
            
            }
            
            break;
        case 2:
            //Take new string as input
            printf("\nEnter new string for encryption\n");
            scanf("%s", currentString);
            printf("\nOriginal String before encryption: %s\n", currentString);

            keyChange=0; //Since this marks has new session, we allow the key to be changed

            //Write the newly entered string to device file
            returnOfWrite = write(fileDescriptor, currentString, strlen(currentString));
            if (returnOfWrite < 0){
               perror("Could not write to encryptor device file\n");
               return errno;
            }
            printf("\nString written to device file is: %s\n", currentString);

            isStringEntered=1; //This will unlock case 0 and case 1, if string is entered first time
            break;
        case 3:
            //Set the encryption key to device driver
            //only when new string is just entered
            if(keyChange==0 && isStringEntered==1){
                printf("\nEnter any value between 5 and 20 for setting encryption key\n");
                scanf("%d", &encryptionKey);

                //Verify if the entered encryption is in valid range
                if(encryptionKey<5 || encryptionKey>20){
                printf("Incorrect encryption key\nExiting the program..\n");
                returnOfClose = close(fileDescriptor); // close device file before exit
                if(returnOfClose < 0){
                    printf("Error while closing device file /dev/encryptor\n");
                    return -1;
                }
                exit(0);
                }
                break;
            }
            else{
                printf("\nYou are either in middle of the session");
                printf(" or have not entered the string atleast once yet\n");
                break;
            }

            
        case 4:
            //User chose to end the program
            printf("You chose to end the program\nExiting...\n");            
            break;
    }

    if(lc == 4){
        break;
    }

    

  }
   //closing the opened device file
   returnOfClose = close(fileDescriptor);
   if(returnOfClose < 0){
    printf("Error while closing device file /dev/encryptor\n");
    return -1;
   }

   return 0;

 }