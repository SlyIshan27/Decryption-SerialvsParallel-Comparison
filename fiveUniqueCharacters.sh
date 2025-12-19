#!/bin/bash

OUTPUTFILE="outputs/fiveUniqueChars.txt" 

echo "Running five Unique Character Decryption Performance Analysis 30 times..."

echo -e "five Unique Character Performance analysis: \n" > "$OUTPUTFILE"
for i in {1..30}
do
    echo "Run #$i" >> "$OUTPUTFILE"
    ./encrypt the cat >> "$OUTPUTFILE"
    echo -e "Serial Output:" >> "$OUTPUTFILE"
    { time ./decrypt_serial ciphertext.txt american-english ; } >> "$OUTPUTFILE" 2>&1
    echo -e "\nParallel Output:" >> "$OUTPUTFILE"
    { time -p mpirun -np 5 ./decryptParallel ciphertext.txt american-english ; } >> "$OUTPUTFILE" 2>&1
    echo "-------------------------------------------------------------------" >> "$OUTPUTFILE"
done
echo "Done!"