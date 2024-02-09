import time

def fibonacci(n):
    if n <= 1:
       return n
    else:
       return(fibonacci(n-1) + fibonacci(n-2))

n = 35

start_time = time.time()
fib_num = fibonacci(n)
end_time = time.time()

print(f'Time taken: {end_time - start_time} seconds')
print(f'The {n}th Fibonacci number: {fib_num}')

