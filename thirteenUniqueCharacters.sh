#!/bin/bash

OUTPUTFILE="outputs/thirteenUniqueChars.txt" 

echo "Running thirteen Unique Character Decryption Performance Analysis 30 times..."

echo -e "Thirteen Unique Character Performance analysis: \n" > "$OUTPUTFILE"
for i in {1..30}
#Timing out if it exceeds 5 mins for both serial and parallel
do
    echo "Run #$i" >> "$OUTPUTFILE"
    ./encrypt Peter piper picked a peck of pickled peppers >> "$OUTPUTFILE"
    echo -e "Serial Output:" >> "$OUTPUTFILE"
    { time timeout 5m ./decrypt_serial ciphertext.txt american-english ; } >> "$OUTPUTFILE" 2>&1
    ret=$?
    if [ $ret -eq 124 ]; then
        echo ">>> TIMEOUT (Serial) <<<" >> "$OUTPUTFILE"
    fi
    echo -e "\nParallel Output:" >> "$OUTPUTFILE"
    { time timeout 5m mpirun --oversubscribe -np 13 ./decryptParallel ciphertext.txt american-english ; } >> "$OUTPUTFILE" 2>&1
    ret=$?
    if [ $ret -eq 124 ]; then
        echo ">>> TIMEOUT (Parallel) <<<" >> "$OUTPUTFILE"
    fi
    echo "-------------------------------------------------------------------" >> "$OUTPUTFILE"
done
echo "Done!"