package main

func main() {
    var a **int

    var pb *int
    var pc *int

    var b = 20
    var c = 40

    pb = &b
    pc = &c

    a = &pb

    **a = 10
    print(b)

    *pc = **a
    print(c)
}
