package main

func main() {
    var a,b int
    scan(&a)
    scan(&b)

    var add, sub, mul, div = a + b, a - b, a * b, a / b
    print(add)
    print(sub)
    print(mul)
    print(div)
}