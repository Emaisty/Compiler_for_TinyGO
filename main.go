package main

type life struct {
    age, h int
}

type human struct {
    a, b int
    stats life
}

func copy(a *human, b human) {
    *a = b
}

func notChangeH(b human) {
    b.stats.h = 200;
}

func main() {
    var a, b human
    b.a = 12
    b.b = 20
    b.stats.age = 30
    b.stats.h = 189
    notChangeH(b)
    copy(&a,b)
    print(a.stats.h)
}