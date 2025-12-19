#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <mpi.h>

#define MAXDICTWORDS 200000
#define MAXLEN 1020

//Global Variable/flag for checking if a valid word is found
int wordsFound = 0;

typedef struct {
    char **words;
    int count;
} Dictionary;

// typedef struct {
//     char decrypted[MAXLEN];
//     int rank;
// } Result;

//Removing the char from the newly converted cipher dictionary, and then adding it to the front. Getting the right char
//According to the process rank/number (MPI Reasons)
void removeCharAndPrepend(char *string, char c){
    //Removing
    int i;
    int j = 0;
    int length;

    for(i = 0; string[i] != '\0'; i++){
        if(string[i] != c){
            string[j++] = string[i];
        }
    }

    string[j] = '\0';
    //Prepend
    length = strlen(string);
    for(i = length; i >= 0; i--){
        string[i + 1] = string[i];
    }

    string[0] = c;

}
//Helper to compare strings, same in serial as it is the same brute force method
int compareStrings(const void *a, const void *b){
    return strcmp(*(const char **)a, *(const char **)b);
}
//Loading into memory and qsorting as it is the same method of brute force as serial
Dictionary *loadDictionary(char *filename){
    FILE *file = fopen(filename, "r");
    if(!file){
        perror("Failed to open dictionary file");
        return NULL;
    }
    Dictionary * dictionary = malloc(sizeof(Dictionary));
    if(!dictionary){
        fclose(file);
        return NULL;
    }

    dictionary->words = malloc(MAXDICTWORDS * sizeof(char *));
    if(!dictionary->words){
        free(dictionary);
        fclose(file);
        return NULL;
    }
    dictionary->count = 0;
    int i;  
    char line[MAXLEN];
    while (fgets(line, sizeof(line), file) && dictionary->count < MAXDICTWORDS){
        line[strcspn(line, "\r\n")] = 0;
        for(i = 0; line[i]; i++){
            line[i] = tolower((unsigned char)line[i]);    
        }
        if(line[0] == '\0'){
            continue;
        }
        char *dup = strdup(line);
        if(!dup){
            for(int j = 0; j < dictionary->count; j++){
                free(dictionary->words[j]);
            }
            free(dictionary->words);
            free(dictionary);
            fclose(file);
            return NULL;
        }
        dictionary->words[dictionary->count++] = dup;
    }

    fclose(file);

    qsort(dictionary->words, dictionary->count, sizeof(char *), compareStrings);
    return dictionary;
}
// Checking if the word is in the dictionary by doing binary search, same as in serial.
int isWordInDictionary(Dictionary *dict, char *word){
    int left = 0;
    int right = dict->count - 1;
    
    while(left <= right){
        int mid = left + (right - left) / 2;
        int cmp = strcmp(word, dict->words[mid]);
        
        if(cmp == 0){
            return 1;
        } else if(cmp < 0){
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }
    return 0;
}

// Trying a decryption on cipher text using the given decryption dictionary from the permute function
void decrypt(char *cipher, char *cipherDict, char *decryptDict, char *result){
    int i;
    for(i = 0; cipher[i]; i++){
        if(isalpha(cipher[i])){
            char *pos = strchr(cipherDict, cipher[i]);
            if(pos){
                int index = pos - cipherDict;
                result[i] = decryptDict[index];
            } else {
                result[i] = cipher[i];
            }
        } else {
            result[i] = cipher[i];
        }
    }
    result[i] = '\0';
}

// Validate if the word is in the dictionary and if it is a proper decryption
int validateDecryption(char *decrypted, Dictionary *dict){
    char temp[MAXLEN];
    strcpy(temp, decrypted);
    
    char *token = strtok(temp, " \t\n");
    while(token != NULL){
        // Remove non-alphabetic characters from token
        char cleanWord[MAXLEN];
        int j = 0;
        int k;
        for(k = 0; token[k]; k++){
            if(isalpha(token[k])){
                cleanWord[j++] = tolower(token[k]);
            }
        }
        cleanWord[j] = '\0';
        
        if(strlen(cleanWord) > 0 && !isWordInDictionary(dict, cleanWord)){
            return 0;
        }
        token = strtok(NULL, " \t\n");
    }
    return 1;
}

// Helper to swap two characters
void swap(char *a, char *b){
    char temp = *a;
    *a = *b;
    *b = temp;
}

// Function to generate permutations and test each one
void permute(char *str, int start, int end, char *cipher, char *cipherDict, 
             Dictionary *dict, int rank){
    if(start == end){
        // Test this permutation
        char decrypted[MAXLEN];
        decrypt(cipher, cipherDict, str, decrypted);
        
        if(validateDecryption(decrypted, dict)){
            printf("rank %d: %s \n", rank, decrypted);
            wordsFound++;
            fflush(stdout);

        }
    } else {
        int i;
        for(i = start; i <= end; i++){
            swap(&str[start], &str[i]);
            permute(str, start + 1, end, cipher, cipherDict, dict, rank);
            swap(&str[start], &str[i]);
        }
    }
}
//Main
int main(int argc, char * argv[]){
    if(argc < 3){
        fprintf(stderr, "Error not enough arguments. Usage: %s \n", argv[0]);
        return 1;
    }
    //Initialization for MPI
    int processes;
    int processRank;
    int i;
    int length;
    char firstLetter;
    //MPI set up
    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &processes);
    MPI_Comm_rank(MPI_COMM_WORLD, &processRank);

    //Getting cipher file and cipher from the file.
    FILE *cipherFile = fopen(argv[1], "r");
    if(!cipherFile){
        perror("Failed to open ciphertext file");
        MPI_Finalize(); 
        return 1;
    }
    char cipher[MAXLEN];
    if(!fgets(cipher, MAXLEN, cipherFile)){
        if(processRank == 0) {
            fprintf(stderr, "Ciphertext file empty\n");
        }
        fclose(cipherFile);
        MPI_Finalize();
        return 1;
    }
    
    cipher[strcspn(cipher, "\r\n")] = 0;
    fclose(cipherFile);
    //Creating the cipher dictionary by getting rid of repeat characters
    char cipherDict[27] = "";
    for(i = 0; cipher[i]; i++){
        if(isalpha(cipher[i]) && !strchr(cipherDict, cipher[i])){
            length = strlen(cipherDict);
            cipherDict[length] = cipher[i];
            cipherDict[length + 1] = '\0';
        }
    }
    //Load in the dictionary into memory
    Dictionary *loadedDictionary = loadDictionary(argv[2]);
    if (loadedDictionary == NULL) {
        if (processRank == 0) {
            fprintf(stderr, "Failed to load dictionary file: %s\n", argv[2]);
        }
        MPI_Barrier(MPI_COMM_WORLD); // give ranks a chance to synchronize then cleanup
        MPI_Finalize();
        return 1;
    } 
    int dictLen = strlen(cipherDict);

    MPI_Barrier(MPI_COMM_WORLD);
    //If we have extra processes that are not needed, it has no work so we clean it up.
    if(processRank >= dictLen){
        for(i = 0; i < loadedDictionary->count; i++){
            free(loadedDictionary->words[i]);
        }
        free(loadedDictionary->words);
        free(loadedDictionary);
        MPI_Finalize();
        return 0;
    }
    
    //create the string to create the different decryptions for each process. Basically each process is getting their assigned string to permute and test
    char myDictionary[27];
    strcpy(myDictionary, cipherDict);
    firstLetter = cipherDict[processRank];
    removeCharAndPrepend(myDictionary, firstLetter);
    permute(myDictionary, 1, dictLen - 1, cipher, cipherDict, loadedDictionary, processRank);

    int globalWordsFound = 0;
    //Doing reduction across all processes
    MPI_Reduce(&wordsFound, &globalWordsFound, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    //If no words were found, then process zero or main will output the error message
    if(globalWordsFound == 0 && processRank == 0){
        printf("No valid words were found from the given dictionary after the decryption(s). \n");
    }
    
    //Was just another way I was try to implement it
    // int *resultCounts = NULL;
    // if(processRank == 0){
    //     resultCounts = malloc(processes * sizeof(int));
    // }
    
    // MPI_Gather(&myResultCount, 1, MPI_INT, resultCounts, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    // // Rank 0 receives and prints all results
    // if(processRank == 0){
    //     int totalResults = 0;
    //     for(i = 0; i < processes; i++){
    //         totalResults += resultCounts[i];
    //     }
        
    //     if(totalResults == 0){
    //         printf("No valid decryptions found.\n");
    //     } else {
    //         // First print rank 0's own results
    //         for(i = 0; i < myResultCount; i++){
    //             printf("rank %d: %s\n", myResults[i].rank, myResults[i].decrypted);
    //         }
            
    //         // Receive and print results from other ranks
    //         for(int rank = 1; rank < processes; rank++){
    //             if(resultCounts[rank] > 0){
    //                 Result receivedResults[MAX_RESULTS];
    //                 MPI_Recv(receivedResults, resultCounts[rank] * sizeof(Result), 
    //                         MPI_BYTE, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    
    //                 for(i = 0; i < resultCounts[rank]; i++){
    //                     printf("rank %d: %s\n", receivedResults[i].rank, 
    //                            receivedResults[i].decrypted);
    //                 }
    //             }
    //         }
    //     }
    //     free(resultCounts);
    // } else {
    //     // Other ranks send their results to rank 0
    //     if(myResultCount > 0){
    //         MPI_Send(myResults, myResultCount * sizeof(Result), 
    //                 MPI_BYTE, 0, 0, MPI_COMM_WORLD);
    //     }
    // }

    //Cleanup of dictionary and MPI.
    
    for(i = 0; i < loadedDictionary->count; i++){
        free(loadedDictionary->words[i]);
    }
    free(loadedDictionary->words);
    free(loadedDictionary);

    MPI_Finalize();
    return 0;
}