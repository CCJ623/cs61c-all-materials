.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
# 	d = matmul(m0, m1)
# Arguments:
# 	a0 (int*)  is the pointer to the start of m0 
#	a1 (int)   is the # of rows (height) of m0
#	a2 (int)   is the # of columns (width) of m0
#	a3 (int*)  is the pointer to the start of m1
# 	a4 (int)   is the # of rows (height) of m1
#	a5 (int)   is the # of columns (width) of m1
#	a6 (int*)  is the pointer to the the start of d
# Returns:
#	None (void), sets d = matmul(m0, m1)
# Exceptions:
#   Make sure to check in top to bottom order!
#   - If the dimensions of m0 do not make sense,
#     this function terminates the program with exit code 72.
#   - If the dimensions of m1 do not make sense,
#     this function terminates the program with exit code 73.
#   - If the dimensions of m0 and m1 don't match,
#     this function terminates the program with exit code 74.
# =======================================================
matmul:

    # Error checks
    li t0 1
    blt a1 t0 m0_error
    blt a2 t0 m0_error
    blt a4 t0 m1_error
    blt a5 t0 m1_error
    bne a2 a4 match_error

    # Prologue
    addi sp sp -28
    sw s0 0(sp)
    sw s1 4(sp)
    sw s2 8(sp)
    sw s3 12(sp)
    sw s4 16(sp)
    sw s5 20(sp)
    sw s6 24(sp)

    # s0 stores index of m0 row
    mv s0 zero
    # s1 stores pointer to m0 row
    mv s1 a0
    # s2 stores distance to m0 next_row
    mv s2 a2
    slli s2 s2 2
    # s3 stores pointer to element in d
    mv s3 a6

outer_loop_start:

    # s4 stores index of m1 col
    mv s4 zero
    # s5 stores pointer to m1 col
    mv s5 a3
    # s6 stores distance to m1 next_row
    mv s6 a5

inner_loop_start:

    # prologue
    addi sp sp -32
    sw a0 0(sp)
    sw a1 4(sp)
    sw a2 8(sp)
    sw a3 12(sp)
    sw a4 16(sp)
    sw a5 20(sp)
    sw a6 24(sp)
    sw ra 28(sp)

    mv a0 s1
    mv a1 s5
    mv a2 a2
    li a3 1
    mv a4 s6

    jal dot

    sw a0 0(s3)

    # epilogue
    lw a0 0(sp)
    lw a1 4(sp)
    lw a2 8(sp)
    lw a3 12(sp)
    lw a4 16(sp)
    lw a5 20(sp)
    lw a6 24(sp)
    lw ra 28(sp)
    addi sp sp 32

inner_loop_end:
    addi s3 s3 4
    addi s4 s4 1
    addi s5 s5 4
    blt s4 a5 inner_loop_start

outer_loop_end:
    addi s0 s0 1
    add s1 s1 s2
    blt s0 a1 outer_loop_start

    # Epilogue
    lw s0 0(sp)
    lw s1 4(sp)
    lw s2 8(sp)
    lw s3 12(sp)
    lw s4 16(sp)
    lw s5 20(sp)
    lw s6 24(sp)
    addi sp sp 28
    
    ret

m0_error:
    li a1 72
    j exit2

m1_error:
    li a1 73
    j exit2

match_error:
    li a1 74
    j exit2
