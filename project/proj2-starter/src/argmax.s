.globl argmax

.text
# =================================================================
# FUNCTION: Given a int vector, return the index of the largest
#	element. If there are multiple, return the one
#	with the smallest index.
# Arguments:
# 	a0 (int*) is the pointer to the start of the vector
#	a1 (int)  is the # of elements in the vector
# Returns:
#	a0 (int)  is the first index of the largest element
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 77.
# =================================================================
argmax:
    li t0 1
    blt a1 t0 error_exit

    # Prologue
    addi sp sp -12
    sw s0 0(sp)
    sw s1 4(sp)
    sw s2 8(sp)

loop_start:
    # s0 stores pointer to element
    mv s0 a0
    # s1 stores index of element
    mv s1 zero
    # s2 stores max element
    mv s2 zero

loop_continue:
    lw t0 0(s0)
    ble t0 s2 loop_end
    mv s2 t0
    mv a0 s1

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

error_exit:
    li a1 77
    j exit2

