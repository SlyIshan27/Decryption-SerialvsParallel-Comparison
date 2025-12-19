#!/bin/bash
#To run this script just do a simple: ./simpleTests.sh
#You will see the output in outputs/simpleTest.txt

OUTPUT="outputs/simpleTest.txt"

echo "Simple Test 1" > "$OUTPUT"
./encrypt first >> "$OUTPUT"

echo -e "\nSerial Output:" >> "$OUTPUT"
./decrypt_serial ciphertext.txt american-english >> "$OUTPUT" 2>&1

echo -e "\nParallel Output:" >> "$OUTPUT"
mpirun -np 5 ./decryptParallel ciphertext.txt american-english ciphertext.txt american-english >> "$OUTPUT"
echo -e "--------------------------------------------------------------------\n" >> "$OUTPUT"
echo "Simple Test 2" >> "$OUTPUT"
./a2encrypt task >> "$OUTPUT"

echo -e "\nSerial Output:" >> "$OUTPUT"
./a2decrypt_serial ciphertext.txt american-english >> "$OUTPUT" 2>&1

echo -e "\nParallel Output:" >> "$OUTPUT"
mpirun -np 4 ./a2decrypt ciphertext.txt american-english ciphertext.txt american-english >> "$OUTPUT"
echo -e "--------------------------------------------------------------------\n" >> "$OUTPUT"
echo "Simple Test 3" >> "$OUTPUT"
./a2encrypt begin >> "$OUTPUT"

echo -e "\nSerial Output:" >> "$OUTPUT"
./a2decrypt_serial ciphertext.txt american-english >> "$OUTPUT" 2>&1

echo -e "\nParallel Output:" >> "$OUTPUT"
mpirun -np 5 ./a2decrypt ciphertext.txt american-english ciphertext.txt american-english >> "$OUTPUT"
echo -e "--------------------------------------------------------------------\n" >> "$OUTPUT"
echo "Simple Test 4" >> "$OUTPUT"
./a2encrypt iceboxes >> "$OUTPUT"

echo -e "\nSerial Output:" >> "$OUTPUT"
./a2decrypt_serial ciphertext.txt american-english >> "$OUTPUT" 2>&1

echo -e "\nParallel Output:" >> "$OUTPUT"
mpirun -np 7 ./a2decrypt ciphertext.txt american-english ciphertext.txt american-english >> "$OUTPUT"
echo -e "--------------------------------------------------------------------\n" >> "$OUTPUT"
echo "Simple Test 5" >> "$OUTPUT"
./a2encrypt the cat >> "$OUTPUT"

echo -e "\nSerial Output:" >> "$OUTPUT"
./a2decrypt_serial ciphertext.txt american-english >> "$OUTPUT" 2>&1

echo -e "\nParallel Output:" >> "$OUTPUT"
mpirun -np 5 ./a2decrypt ciphertext.txt american-english ciphertext.txt american-english >> "$OUTPUT"