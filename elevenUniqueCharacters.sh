#!/bin/bash

OUTPUTFILE="outputs/elevenUniqueChars.txt" 

echo "Running eleven Unique Character Decryption Performance Analysis 30 times..."

echo -e "Eleven Unique Character Performance analysis: \n" > "$OUTPUTFILE"
for i in {1..30}
do
    echo "Run #$i" >> "$OUTPUTFILE"
    ./encrypt I tested subject in cat >> "$OUTPUTFILE"
    echo -e "Serial Output:" >> "$OUTPUTFILE"
    { time ./decrypt_serial ciphertext.txt american-english ; } >> "$OUTPUTFILE" 2>&1
    echo -e "\nParallel Output:" >> "$OUTPUTFILE"
    { time -p mpirun --oversubscribe -np 11 ./decryptParallel ciphertext.txt american-english ; } >> "$OUTPUTFILE" 2>&1
    echo "-------------------------------------------------------------------" >> "$OUTPUTFILE"
done
echo "Done!"