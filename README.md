# Simple Virtual Machine

This project emulates a simple virtual machine of 8-bit architecture which performs operations using pseudo assembly instructions.

##### How to run the source code:
1. Clone the Github repository
2. Clean previous builds if exist by running the command: `make clean`
3. To run the virtual machine emulation, run the command: `make vm_x2017`, followed by: `./vm_x2017 tests/0/0.x2017`, where “tests/0/0.x2017” is an example of an exisiting binary file of pseudo assembly instuctions.
4. To run the dissembler, run the command: `make objdump_x2017`, followed by: `./objdump_x2017 tests/0/0.x2017`

##### Testing
1. All the tests are contained in the folder named “tests”. 
2. To run all tests, run the command: `make test`, followed by: `make run_tests`
