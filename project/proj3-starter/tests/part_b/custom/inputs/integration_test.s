
_start:
    # 加载 n 的值到 a0

    addi a0 zero 10

    # 调用 Fibonacci 函数
    call fibonacci_iter
	j end

# Fibonacci 迭代函数
# 输入：n (a0)
# 输出：Fibonacci(n) (a0)
fibonacci_iter:
    li t0, 0                # t0 = 0 (Fibonacci(0))
    li t1, 1                # t1 = 1 (Fibonacci(1))
    mv s0, a0               # s0 = n，保存输入值 n
    beqz s0, fib_done       # 如果 n == 0，直接返回 t0

fib_loop:
    add t2, t0, t1          # t2 = t0 + t1 (Fibonacci(i) = Fibonacci(i-1) + Fibonacci(i-2))
    mv t0, t1               # t0 = t1
    mv t1, t2               # t1 = t2
    addi s0, s0, -1         # n = n - 1
    bnez s0, fib_loop       # 如果 n != 0，继续循环

    mv a0, t1               # 将结果保存到 a0

fib_done:
    ret                     # 返回

end: