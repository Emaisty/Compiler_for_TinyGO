package main

func main() {
    var a = 0
    var b int
    scan(&b)

    for i := 0; i < b; ++i {
        a += 2
    }
    print(a)
}