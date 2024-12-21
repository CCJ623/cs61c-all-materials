.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
# - If malloc returns an error,
#   this function terminates the program with error code 88.
# - If you receive an fopen error or eof, 
#   this function terminates the program with error code 90.
# - If you receive an fread error or eof,
#   this function terminates the program with error code 91.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 92.
# ==============================================================================
read_matrix:

    # Prologue
    addi sp sp -24
    sw ra 0(sp)
    sw s0 4(sp)
    sw s1 8(sp)
    sw s2 12(sp)
    sw s3 16(sp)
    sw s4 20(sp)
	
    # open file
    jal save_arguments
    mv a1 a0
    li a2 0 
    
    jal fopen

    li t0 -1
    beq a0 t0 fopen_error
    # s0 stores file descriptor
    mv s0 a0
    jal load_arguments

    # read row
    jal save_arguments
    mv a2 a1
    mv a1 s0
    li a3 4

    jal fread

    li t0 4
    bne a0 t0 fread_error
    jal load_arguments
    # s1 stores number of rows
    lw s1 0(a1)

    # read col
    jal save_arguments
    mv a1 s0
    li a3 4

    jal fread

    li t0 4
    bne a0 t0 fread_error
    jal load_arguments
    # s2 stores number of cols
    lw s2 0(a2)

    # allocate memory for matrix
    jal save_arguments
    # s3 stores bytes of matrix
    mul s3 s1 s2
    slli s3 s3 2
    mv a0 s3

    jal malloc

    beq a0 zero malloc_error
    # s4 stores pointer to matrix
    mv s4 a0
    jal load_arguments

    # read matrix
    jal save_arguments
    mv a1 s0
    mv a2 s4
    mv a3 s3

    jal fread

    bne a0 s3 fread_error
    jal load_arguments

    # close file
    jal save_arguments  
    mv a1 s0

    jal fclose

    bne a0 zero fclose_error
    jal load_arguments

    # set return values
    mv a0 s4

    # Epilogue
    lw ra 0(sp)
    lw s0 4(sp)
    lw s1 8(sp)
    lw s2 12(sp)
    lw s3 16(sp)
    lw s4 20(sp)
    addi sp sp 24


    ret

malloc_error:
    li a1 88
    j exit2

fopen_error:
    li a1 90
    j exit2

fread_error:
    li a1 91
    j exit2

fclose_error:
    li a1 92
    j exit2

save_arguments:
    addi sp sp -12
    sw a0 0(sp)
    sw a1 4(sp)
    sw a2 8(sp)
    ret

load_arguments:
    lw a0 0(sp)
    lw a1 4(sp)
    lw a2 8(sp)
    addi sp sp 12
    ret
