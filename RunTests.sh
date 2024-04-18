cmake -S . -B build
cd build
make
echo "./../Input/test_program1.txt" | ./assembler
echo "./../Input/test_program2.txt" | ./assembler
echo "./../Input/test_program3.txt" | ./assembler
echo "./../Input/test_program4.txt" | ./assembler
echo "./../Input/test_program5.txt" | ./assembler
echo "./../Input/test_program6.txt" | ./assembler
echo "./../Input/error_program1.txt" | ./assembler
echo "./../Input/error_program2.txt" | ./assembler
echo "./../Input/error_program3.txt" | ./assembler
echo "./../Input/error_program4.txt" | ./assembler
echo "./../Input/error_program5.txt" | ./assembler
cd tests
./assembler_test
