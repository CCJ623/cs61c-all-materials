addi t0 x0 -1
addi t1 x0 0
addi t2 x0 1

blt t0 t0 one
blt t1 t0 two
blt t1 t2 three

one:
addi s0 zero 1
j end

two:
addi s1 zero 2
j end

three:
addi ra zero 3
j end

end:
addi sp zero 4