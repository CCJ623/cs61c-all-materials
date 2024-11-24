.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int vectors
# Arguments:
#   a0 (int*) is the pointer to the start of v0
#   a1 (int*) is the pointer to the start of v1
#   a2 (int)  is the length of the vectors
#   a3 (int)  is the stride of v0
#   a4 (int)  is the stride of v1
# Returns:
#   a0 (int)  is the dot product of v0 and v1
# Exceptions:
# - If the length of the vector is less than 1,
#   this function terminates the program with error code 75.
# - If the stride of either vector is less than 1,
#   this function terminates the program with error code 76.
# =======================================================
dot:
    li t0 1
    blt a2 t0 length_error
    blt a3 t0 stride_error
    blt a4 t0 stride_error

    # Prologue
    addi sp sp -12
    sw s0 0(sp)
    sw s1 4(sp)
    sw s2 8(sp)

loop_start:

    # s0 stores pointer to element in v0
    mv s0 a0
    # s1 stores pointer to element in v1
    mv s1 a1
    # s2 stores index of element
    mv s2 zero
    # interpret stride to bytes
    slli a3 a3 2
    slli a4 a4 2
    # set a0 as result
    mv a0 zero

loop_continue:
    lw t0 0(s0)
    lw t1 0(s1)
    mul t0 t0 t1
    add a0 a0 t0

loop_end:
    add s0 s0 a3
    add s1 s1 a4
    addi s2 s2 1
    blt s2 a2 loop_continue

    # Epilogue
    lw s0 0(sp)
    lw s1 4(sp)
    lw s2 8(sp)
    addi sp sp 12
    
    ret

length_error:
    li a1 75
    j exit2

stride_error:
    li a1 76
    j exit2