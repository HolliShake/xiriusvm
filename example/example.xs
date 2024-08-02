
define muls(a, b) {
    println("FROM MULS", a, b);
    return a * b;
}

define div(a, b) {
    return a / b;
}


@jit
define add(a, b) {
    muls(4, 5);
    div(2, 3);
    return 5;
}

println(add(1, 2), muls(3, 4));
println("last>>");