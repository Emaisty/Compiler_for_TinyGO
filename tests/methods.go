package main

type human struct {
    age int
}

func (h *human) setAge(val int) {
    h.age = val
}

func (h human) getDoubleAge() int {
    h.age *= 2
    return h.age
}

func main() {
    var a human
    a.setAge(12)
    print(a.getDoubleAge())
    print(a.age)
}