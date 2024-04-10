cmake -S . -B build
cd build
make
echo "./../Input/test_program1.txt" | ./assembler
echo "./../Input/test_program2.txt" | ./assembler
echo "./../Input/test_program3.txt" | ./assembler
cd tests
./assembler_test