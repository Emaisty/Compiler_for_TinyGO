package main

type student struct {
    grade, age int
}

func main() {
    var a student
    scan(&a.grade)
    scan(&a.age)

    print(a.grade)
    print(a.age)
}