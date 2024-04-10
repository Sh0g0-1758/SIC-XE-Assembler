cmake -S . -B build
cd build
make
echo "./../Input/test_program3.txt" | ./assembler