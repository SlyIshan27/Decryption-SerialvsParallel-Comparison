#!/bin/bash

OUTPUTFILE="outputs/threeUniqueChars.txt" 

echo "Running Three Unique Character Decryption Performance Analysis 30 times..."

echo -e "Three Unique Character Performance analysis: \n" > "$OUTPUTFILE"
for i in {1..30}
do
    echo "Run #$i" >> "$OUTPUTFILE"
    ./encrypt cat >> "$OUTPUTFILE"
    echo -e "Serial Output:" >> "$OUTPUTFILE"
    { time ./decrypt_serial ciphertext.txt american-english ; } >> "$OUTPUTFILE" 2>&1
    echo -e "\nParallel Output:" >> "$OUTPUTFILE"
    { time -p mpirun -np 3 ./decryptParallel ciphertext.txt american-english ; } >> "$OUTPUTFILE" 2>&1
    echo "-------------------------------------------------------------------" >> "$OUTPUTFILE"
done
echo "Done!"