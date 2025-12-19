#!/bin/bash

OUTPUTFILE="outputs/nineUniqueChars.txt" 

echo "Running nine Unique Character Decryption Performance Analysis 30 times..."

echo -e "Nine Unique Character Performance analysis: \n" > "$OUTPUTFILE"
for i in {1..30}
do
    echo "Run #$i" >> "$OUTPUTFILE"
    ./encrypt I tested in the cat >> "$OUTPUTFILE"
    echo -e "Serial Output:" >> "$OUTPUTFILE"
    { time ./decrypt_serial ciphertext.txt american-english ; } >> "$OUTPUTFILE" 2>&1
    echo -e "\nParallel Output:" >> "$OUTPUTFILE"
    { time -p mpirun --oversubscribe -np 9 ./decryptParallel ciphertext.txt american-english ; } >> "$OUTPUTFILE" 2>&1
    echo "-------------------------------------------------------------------" >> "$OUTPUTFILE"
done
echo "Done!"