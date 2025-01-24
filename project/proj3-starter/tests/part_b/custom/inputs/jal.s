addi t0 x0 1
addi t1 x0 2
addi t2 x0 3

one:
addi s0 zero 1
jal two
jal three
j end

two:
addi s1 zero 2
ret

three:
addi s1 s1 1
ret

end:
addi sp zero 4