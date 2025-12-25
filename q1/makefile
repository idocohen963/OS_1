all: divide_by_zero stack_overflow invalid_memory divide_by_zero_g stack_overflow_g invalid_memory_g

divide_by_zero: divide_by_zero.c
	gcc $< -o $@

stack_overflow: stack_overflow.c
	gcc $< -o $@

invalid_memory: invalid_memory.c
	gcc $< -o $@
divide_by_zero_g: divide_by_zero.c
	gcc -g $< -o $@

stack_overflow_g: stack_overflow.c
	gcc -g $< -o $@

invalid_memory_g: invalid_memory.c
	gcc -g $< -o $@

clean:
	rm -f divide_by_zero stack_overflow invalid_memory divide_by_zero_g stack_overflow_g invalid_memory_g core.*


#	ulimit -c unlimited
# Allow the shell to generate core dump files (must be enabled per terminal)

#sudo sh -c 'echo core.%e > /proc/sys/kernel/core_pattern'
# Configure the kernel to write core files to the current working directory, named by executable

#cat /proc/sys/kernel/core_pattern
# Verify the current core dump naming pattern

#./program_name
# Run the program; if it crashes, a core.<executable> file will be created in this directory

#gdb ./program_name core.program_name
# Start GDB with the executable and its corresponding core dump
