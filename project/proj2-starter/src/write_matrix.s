.globl write_matrix

.text
# ==============================================================================
# FUNCTION: Writes a matrix of integers into a binary file
# FILE FORMAT:
#   The first 8 bytes of the file will be two 4 byte ints representing the
#   numbers of rows and columns respectively. Every 4 bytes thereafter is an
#   element of the matrix in row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is the pointer to the start of the matrix in memory
#   a2 (int)   is the number of rows in the matrix
#   a3 (int)   is the number of columns in the matrix
# Returns:
#   None
# Exceptions:
# - If you receive an fopen error or eof,
#   this function terminates the program with error code 93.
# - If you receive an fwrite error or eof,
#   this function terminates the program with error code 94.
# - If you receive an fclose error or eof,
#   this function terminates the program with error code 95.
# ==============================================================================
write_matrix:

    # Prologue
    addi sp sp -12
    sw ra 0(sp)
    sw s0 4(sp)
    sw s1 8(sp)

    # open file
    jal save_arguments
    mv a1 a0
    li a2 1

    jal fopen

    li t0 -1
    beq a0 t0 fopen_error
    # s0 stores file descriptor
    mv s0 a0

    # write numbter of rows and cols
    mv a1 s0
    addi a2 sp 8
    li a3 2
    li a4 4
    
    jal fwrite

    li t0 2
    bne a0 t0 fwrite_error
    jal load_arguments

    # write matrix
    jal save_arguments
    # s1 stores elements in matrix
    mul s1 a2 a3
    mv a2 a1
    mv a1 s0
    mv a3 s1
    li a4 4

    mv a1 a1

    jal fwrite
    
    bne a0 s1 fwrite_error
    jal load_arguments

    # close file
    jal save_arguments
    mv a1 s0

    jal fclose  

    bne a0 zero fclose_error    
    jal load_arguments



    # Epilogue
    lw ra 0(sp)
    lw s0 4(sp)
    lw s1 8(sp)
    addi sp sp 12

    ret

fopen_error:
    li a1 93
    j exit2

fwrite_error:
    li a1 94
    j exit2

fclose_error:
    li a1 95
    j exit2

save_arguments:
    addi sp sp -16
    sw a0 0(sp)
    sw a1 4(sp)
    sw a2 8(sp)
    sw a3 12(sp)
    ret

load_arguments:
    lw a0 0(sp)
    lw a1 4(sp)
    lw a2 8(sp)
    lw a3 12(sp)
    addi sp sp 16
    ret
