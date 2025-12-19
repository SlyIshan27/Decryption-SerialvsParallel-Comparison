#!/bin/bash

OUTPUT="outputs/longerTests.txt"

echo "Long Test 1" > "$OUTPUT"
./encrypt cat in the hat >> "$OUTPUT"

echo -e "\nSerial Output:" >> "$OUTPUT"
./decrypt_serial ciphertext.txt american-english >> "$OUTPUT" 2>&1

echo -e "\nParallel Output:" >> "$OUTPUT"
mpirun -np 7 ./decryptParallel ciphertext.txt american-english ciphertext.txt american-english >> "$OUTPUT"
echo -e "--------------------------------------------------------------------\n" >> "$OUTPUT"
echo "Long Test 2" >> "$OUTPUT"
./encrypt fort is big >> "$OUTPUT"

echo -e "\nSerial Output:" >> "$OUTPUT"
./decrypt_serial ciphertext.txt american-english >> "$OUTPUT" 2>&1

echo -e "\nParallel Output:" >> "$OUTPUT"
mpirun -np 8 ./decryptParallel ciphertext.txt american-english ciphertext.txt american-english >> "$OUTPUT"
echo -e "--------------------------------------------------------------------\n" >> "$OUTPUT"
