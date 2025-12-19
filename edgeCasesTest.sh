#!/bin/bash

OUTPUT="outputs/edgeCaseTest.txt"

echo "Edge Case Test 1" > "$OUTPUT"
./encrypt fortnite >> "$OUTPUT"

echo -e "\nSerial Output:" >> "$OUTPUT"
./decrypt_serial ciphertext.txt american-english >> "$OUTPUT" 2>&1

echo -e "\nParallel Output:" >> "$OUTPUT"
mpirun -np 7 ./decryptParallel ciphertext.txt american-english ciphertext.txt american-english >> "$OUTPUT"
echo -e "--------------------------------------------------------------------\n" >> "$OUTPUT"
echo "Edge Case Test 2" >> "$OUTPUT"
./encrypt bruh >> "$OUTPUT"

echo -e "\nSerial Output:" >> "$OUTPUT"
./decrypt_serial ciphertext.txt american-english >> "$OUTPUT" 2>&1

echo -e "\nParallel Output:" >> "$OUTPUT"
mpirun -np 4 ./decryptParallel ciphertext.txt american-english ciphertext.txt american-english >> "$OUTPUT"
echo -e "--------------------------------------------------------------------\n" >> "$OUTPUT"
echo "Edge Case Test 3" >> "$OUTPUT"
./encrypt test task! >> "$OUTPUT"

echo -e "\nSerial Output:" >> "$OUTPUT"
./decrypt_serial ciphertext.txt american-english >> "$OUTPUT" 2>&1

echo -e "\nParallel Output:" >> "$OUTPUT"
mpirun -np 5 ./decryptParallel ciphertext.txt american-english ciphertext.txt american-english >> "$OUTPUT"
echo -e "--------------------------------------------------------------------\n" >> "$OUTPUT"
echo "Edge Case Test 4" >> "$OUTPUT"
./encrypt the cat >> "$OUTPUT"

echo -e "\nSerial Output:" >> "$OUTPUT"
./decrypt_serial ciphertext.txt american-english >> "$OUTPUT" 2>&1

echo -e "\nParallel Output:" >> "$OUTPUT"
mpirun -np 2 ./decryptParallel ciphertext.txt american-english ciphertext.txt american-english >> "$OUTPUT"
echo -e "\nNote: This code only gives two of the possible 4 answers as there are not enough processes to get all 4 answers and brute force all permutations and combinations." >> "$OUTPUT"
echo -e "--------------------------------------------------------------------\n" >> "$OUTPUT"