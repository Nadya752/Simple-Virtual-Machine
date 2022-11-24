CC=gcc
CFLAGS=-fsanitize=address -Wvla -Wall -Werror -g -std=gnu11 -lasan

vm_x2017: vm_x2017.c disassembler_x2017.c
	$(CC) $(CFLAGS) $^ -c  
	$(CC) $(CFLAGS) vm_x2017.o disassembler_x2017.o -o $@

objdump_x2017: objdump_x2017.c disassembler_x2017.c
	$(CC) $(CFLAGS) $^ -c
	$(CC) $(CFLAGS) objdump_x2017.o disassembler_x2017.o -o $@

tests: export ASAN_OPTIONS=verify_asan_link_order=0

run_tests:
	bash test.sh

clean:
	rm objdump_x2017
	rm vm_x2017

