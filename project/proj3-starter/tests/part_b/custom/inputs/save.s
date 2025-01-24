lui t0 0x12345
addi t0 t0 0x678

addi t1 x0 2
addi t2 x0 3

addi sp x0 1024
sb t0 0(sp)
lw t1 0(sp)
sh t0 0(sp)
lw t2 0(sp)
sw t0 0(sp) 
lw s0 0(sp)