#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLEN 1024
#define MAXWORDS 256
#define MAXDICTWORDS 200000

//A structure to hold the words in the dictionary
typedef struct {
    char **words;
    int count;
} Dictionary;
//Global variable to check if a word was found
int wordsFound = 0;

//Prototypes
void decrypt(char *cipher, char *cipherDict, char *decryptDict, char *result);
int validateDecryption(char *decrypted, Dictionary *dict);
void permute(char *str, int start, int end, char *cipher, char *cipherDict, Dictionary *dict);
void swap(char *a, char *b);
//Helper to compare strings
int compareStrings(const void *a, const void *b){
    return strcmp(*(const char **)a, *(const char **)b);
}
// Loading the dictionary into memory, and then qsorting as we do binary search checks against the dictionary.
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
            /* strdup failed: free previously allocated words and structures */
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

//Function to check if the word is in the dictionary, through binary search
int isWordInDictionary(Dictionary *dictionary, char *word){
    int left = 0;
    int right = dictionary->count - 1;
    //Iterate through the structure too see if the word is in the dictionary
    while(left <= right){
        int mid = left + (right - left) / 2;
        int cmp = strcmp(word, dictionary->words[mid]);
        
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
//Helper function to swap characters.
void swap(char *a, char *b){
    char temp = *a;
    *a = *b;
    *b = temp;
}
//Permutation function
void permute(char *string, int start, int end, char *cipher, char *cipherDict, Dictionary *dictionary){
    if(start == end){
        //Test current permutation
        char decrypted[MAXLEN];
        decrypt(cipher, cipherDict, string, decrypted);
        
        if(validateDecryption(decrypted, dictionary)){
            printf("rank 0: %s\n", decrypted);
            wordsFound++;
            fflush(stdout);
        }
    } else {
        //Else swap characters and move onto next permutation
        int i;
        for(i = start; i <= end; i++){
            swap(&string[start], &string[i]);
            permute(string, start + 1, end, cipher, cipherDict, dictionary);
            swap(&string[start], &string[i]); // backtrack
        }
    }
}
//Attempt to decrypt the cipher string with current permutation of the decryption dictionary and comparing it the cipher dictionary
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
//Validating to make sure it a valid decryption and checking if in the dictionary
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
//Main
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s ciphertext.txt dictionary.txt\n", argv[0]);
        return 1;
    }

    // Read ciphertext
    FILE *cipherFile = fopen(argv[1], "r");
    if (!cipherFile) {
        perror("Failed to open ciphertext file");
        return 1;
    }
    //Getting cipher from the given ciphertext file.
    char cipher[MAXLEN];
    fgets(cipher, MAXLEN, cipherFile);
    cipher[strcspn(cipher, "\r\n")] = 0;
    fclose(cipherFile);

    // Extract unique letters from ciphertext to basically get the cipher dict.
    char cipherDict[27] = "";
    for (int i = 0; cipher[i]; i++) {
        if (isalpha(cipher[i]) && !strchr(cipherDict, cipher[i])) {
            int len = strlen(cipherDict);
            cipherDict[len] = cipher[i];
            cipherDict[len + 1] = '\0';
        }
    }

    // Load system dictionary into memory
    Dictionary *dict = loadDictionary(argv[2]);

    int dictLength = strlen(cipherDict);
    // Brute-force  by trying all different permutations of cipherDict this is serial so only one process (rank 0) does it
    for (int firstIndex = 0; firstIndex < dictLength; firstIndex++) {
        char perm[27];
        strcpy(perm, cipherDict);
        char firstLetter = perm[firstIndex];
        for (int i = firstIndex; i > 0; i--) {
            perm[i] = perm[i-1];
        }
        perm[0] = firstLetter;

        
        permute(perm, 1, dictLength - 1, cipher, cipherDict, dict);
    }
    //If no words were found for the cipher text
    if(wordsFound == 0){
        printf("No valid words were found from the given dictionary after the decryption(s) \n");
    }

    //Clean-up
    for (int i = 0; i < dict->count; i++) {
        free(dict->words[i]);   
    }
    free(dict->words);           
    free(dict);

    return 0;
}
