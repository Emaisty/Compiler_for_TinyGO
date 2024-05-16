package main

func init() (int,int,int) {
    var a,b,c int
    scan(&a)
    scan(&b)
    scan(&c)
    return a,b,c
}

func main() {
    var a,b,c int = init()
    print(a)
    print(b)
    print(c)
}