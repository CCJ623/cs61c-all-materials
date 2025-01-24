addi t0 zero two
addi t1 zero three
addi t2 zero end

one:
addi s0 zero 1
jalr ra t0 0
jalr ra t1 0
jalr ra t2 0

two:
addi s1 zero 2
ret

three:
addi s1 s1 1
ret

end:
addi sp zero 4