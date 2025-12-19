#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAXLEN 1024


//Check if the character is in the string

bool charInString(char *string, char c){
    for(int i = 0; string[i]; i++){
        if(string[i] == c){
            return true;
        }
    }
    return false;
}

//Random suffle of the characters in the given string
void randomShuffle(char *string){
    int length = strlen(string);
    for(int i = 0; i < length - 1; i++){
        int j = i + rand() / (RAND_MAX / (length - i) + 1);
        char temp = string[j];
        string[j] = string[i];
        string[i] = temp;
    }
}

int main (int argc, char *argv[]) {
    if(argc < 2){
        printf("%d \n", argc);
        fprintf(stderr, "Error not enough arguments.Did not enter a word. Usage: %s \n", argv[0]);
        return 1;
    }

    srand(time(NULL));
    //Getting input from the command line.
    char input[MAXLEN] = "";
    for (int i = 1; i < argc; i++) {
        strncat(input, argv[i], MAXLEN - strlen(input) - 2);
        if (i < argc - 1){
            strncat(input, " ", MAXLEN - strlen(input) - 2);
        }
    }

    input[MAXLEN - 1] = '\0';
    int i;
    int stringLength = strlen(input);
    //Putting everything in the string in lower case
    for(i = 0; i < stringLength; i++){
        input[i] = tolower(input[i]);
    }
    //Creating an input dictionary and making sure no duplicate character's either in the input dictionary
    char inputDict[MAXLEN] = "";
    for(i = 0; i < stringLength; i++){
        if(isalpha(input[i]) && !charInString(inputDict, input[i])){
            inputDict[strlen(inputDict)] = input[i];
            inputDict[strlen(inputDict) + 1] = '\0';
        }
    }
    //Encryption of the string, but first random shuffling the input dictionary
    char encryption[MAXLEN];
    strcpy(encryption, inputDict);
    randomShuffle(encryption);

    char encryptedString[MAXLEN] = "";
    //Then encrypting it.
    for(i = 0; i < stringLength; i++){
        if(isalpha(input[i])){
            char *position = strchr(inputDict, input[i]);
            if(position){
                int newPosition = position - inputDict;
                encryptedString[i] = encryption[newPosition];
            }
        }else{
            encryptedString[i] = input[i];
        }
    }
    encryption[strlen(inputDict)] = '\0';
    //Write to ciphertext.txt file and overwrite the content
    FILE *encryptFile = fopen("ciphertext.txt", "w");
    if(!encryptFile){
        perror("Failed to open ciphertext.txt");
        return 1;
    }
    fprintf(encryptFile, "%s", encryptedString);
    fclose(encryptFile);

    //Outputs and exit

    printf("Input: %s\n", input);
    printf("Input dictionary:      %s\n", inputDict);
    printf("Encryption dictionary: %s\n", encryption);
    printf("Encrypted text:        %s\n", encryptedString);
    printf("Written to ciphertext.txt\n");

    return 0;
}