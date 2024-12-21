.globl classify

.text
classify:
    # =====================================
    # COMMAND LINE ARGUMENTS
    # =====================================
    # Args:
    #   a0 (int)    argc
    #   a1 (char**) argv
    #   a2 (int)    print_classification, if this is zero, 
    #               you should print the classification. Otherwise,
    #               this function should not print ANYTHING.
    # Returns:
    #   a0 (int)    Classification
    # Exceptions:
    # - If there are an incorrect number of command line args,
    #   this function terminates the program with exit code 89.
    # - If malloc fails, this function terminats the program with exit code 88.
    #
    # Usage:
    #   main.s <M0_PATH> <M1_PATH> <INPUT_PATH> <OUTPUT_PATH>

    # s0 stores pointer to m0
    # s1 stores pointer to number of rows of m0
    # s2 stores pointer to number of cols of m0
    # s3 stores pointer to m1
    # s4 stores pointer to number of rows of m1
    # s5 stores pointer to number of cols of m1
    # s6 stores pointer to input_matrix
    # s7 stores pointer to number of rows of input_matrix
    # s8 stores pointer to number of cols of input_matrix
    # s9 stores pointer to hidden_layer
    # s10 stores pointer to number of rows of hidden_layer
    # s11 stores pointer to number of cols of hidden_layer
    addi sp sp -52
    sw ra 0(sp)
    sw s0 4(sp)
    sw s1 8(sp)
    sw s2 12(sp)
    sw s3 16(sp)
    sw s4 20(sp)
    sw s5 24(sp)
    sw s6 28(sp)
    sw s7 32(sp)
    sw s8 36(sp)
    sw s9 40(sp)
    sw s10 44(sp)
    sw s11 48(sp)

    li t0 5
    bne a0 t0 args_error


	# =====================================
    # LOAD MATRICES
    # =====================================
    # allocate space for number of rows and cols
    jal save_arguments
    li a0 8

    jal malloc

    beq a0 zero malloc_error
    # t5 stores pointer to number of rows
    # t6 stores pointer to number of cols
    mv s5 a0
    addi s6 s5 4
    jal load_arguments
    mv t5 s5
    mv t6 s6

    # Load pretrained m0
    jal save_arguments
    # load file path
    lw a0 4(a1)
    mv a1 t5
    mv a2 t6 

    jal read_matrix

    mv s0 a0
    jal load_arguments
    lw s1 0(t5)
    lw s2 0(t6)

    # Load pretrained m1
    jal save_arguments
    # load file path
    lw a0 8(a1)
    mv a1 t5
    mv a2 t6

    jal read_matrix

    mv s3 a0
    jal load_arguments
    lw s4 0(t5)
    lw s5 0(t6)

    # Load input matrix
    jal save_arguments
    # load file path
    lw a0 12(a1)
    mv a1 t5
    mv a2 t6

    jal read_matrix

    mv s6 a0
    jal load_arguments
    lw s7 0(t5)
    lw s8 0(t6)

    # =====================================
    # RUN LAYERS
    # =====================================
    # 1. LINEAR LAYER:    m0 * input
    # 2. NONLINEAR LAYER: ReLU(m0 * input)
    # 3. LINEAR LAYER:    m1 * ReLU(m0 * input)

    mv s10 s1
    mv s11 s8
    jal save_arguments
    # allocate space for hidden layer
    mul a0 s10 s11
    slli a0 a0 2

    jal malloc

    beq a0 zero malloc_error
    mv s9 a0

    # 1.linear layer
    mv a0 s0
    mv a1 s1
    mv a2 s2
    mv a3 s6
    mv a4 s7
    mv a5 s8
    mv a6 s9

    jal matmul
    
    # 2.nonlinear layer
    mv a0 s9
    mul a1 s10 s11

    jal relu

    # change m0 to scores matrix
    mv a0 s0
    jal free

    mv s1 s4
    mv s2 s11
    mul a0 s1 s2
    slli a0 a0 2

    jal malloc

    li t0 0
    beq a0 t0 malloc_error
    mv s0 a0

    # 3.linear layer
    mv a0 s3
    mv a1 s4
    mv a2 s5
    mv a3 s9
    mv a4 s10
    mv a5 s11
    mv a6 s0

    jal matmul

    jal load_arguments

    # =====================================
    # WRITE OUTPUT
    # =====================================
    # Write output matrix
    jal save_arguments
    # load file path
    lw a0 16(a1)
    mv a1 s0
    mv a2 s1
    mv a3 s2

    jal write_matrix

    # mv a0 s0
    # mv a1 s1
    # mv a2 s2
    # jal print_int_array

    # =====================================
    # CALCULATE CLASSIFICATION/LABEL
    # =====================================
    # Call argmax
    mv a0 s0
    mul a1 s1 s2

    jal argmax

    mv t0 a0
    jal load_arguments
    mv a0 t0

    bne a2 zero end_of_function
    # Print classification
    jal save_arguments
    mv a1 a0

    jal print_int

    # Print newline afterwards for clarity
    li a1 10

    jal print_char

    jal load_arguments

end_of_function:
    jal save_arguments
    mv a0 s0
    jal free
    
    mv a0 s3
    jal free

    mv a0 s6
    jal free

    mv a0 s9
    jal free

    jal load_arguments

    lw ra 0(sp)
    lw s0 4(sp)
    lw s1 8(sp)
    lw s2 12(sp)
    lw s3 16(sp)
    lw s4 20(sp)
    lw s5 24(sp)
    lw s6 28(sp)
    lw s7 32(sp)
    lw s8 36(sp)
    lw s9 40(sp)
    lw s10 44(sp)
    lw s11 48(sp)
    addi sp sp 52


    ret

save_arguments:
    addi sp sp -20
    sw a0 0(sp)
    sw a1 4(sp)
    sw a2 8(sp)
    sw t5 12(sp)
    sw t6 16(sp)
    ret

load_arguments:
    lw a0 0(sp)
    lw a1 4(sp)
    lw a2 8(sp)
    lw t5 12(sp)
    lw t6 16(sp)
    addi sp sp 20
    ret

malloc_error:
    li a1 88
    j exit2

args_error:
    li a1 89
    j exit2
