cmake -S . -B build
cd build
make
echo "./../Input/test_program4.txt" | ./assembler