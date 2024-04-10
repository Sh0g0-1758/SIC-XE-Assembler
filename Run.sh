cmake -S . -B build
cd build
make
echo "./../Input/error_program.txt" | ./assembler