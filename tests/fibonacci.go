package main

func fib(n int) int {
    if n < -1 {
        return -1
    }
    if n == 1 || n == 2 {
        return n
    }

    var a = fib(n - 1)
    var b = fib(n - 2)
    return a + b
}

func main() {
    var a int
    scan(&a)
    print(fib(a))
}