.globl relu

.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
# 	a0 (int*) is the pointer to the array
#	a1 (int)  is the # of elements in the array
# Returns:
#	None
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 78.
# ==============================================================================
relu:
    # exit with error code 78
    li t0 1
    blt a1 t0 length_error

    # Prologue
    addi sp sp -12
    sw s0 0(sp)
    sw s1 4(sp)
    sw s2 8(sp)

loop_start:
    # s0 stores pointer to processing element
    mv s0 a0
    # s1 stores index of element
    mv s1 zero

loop_continue:
    # s2 stores processing element
    lw s2 0(s0)
    bge s2 zero loop_end
    sw zero 0(s0)

loop_end:
    addi s0 s0 4
    addi s1 s1 1
    blt s1 a1 loop_continue

    # Epilogue
    lw s0 0(sp)
    lw s1 4(sp)
    lw s2 8(sp)
    addi sp sp 12
    
	ret

length_error:
    li a1 78
    j exit2
