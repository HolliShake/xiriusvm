
define add(a, b) {
    println(a);
    println(b);
}

add(101, 201);
add(10, 20);
add(1 << 32, 2 * 5);

println("CALLED!");
{
    local h = "Hey!";
    {
        h = h + "Yow!";
        println(h);
    }
}

define loopFN(limit) {
    local index = 0;
    while (index < limit) {
        index = index + 1;
        println(index);
    }
}

println("BEGIN:");
    loopFN(2);

const mul = define(a, b, c) {
    println(a);
    println(b);
    println(c(5));
};

println(mul("Hello", "World!", (define(g) { println("Hehe!"); })));

var h = (define(z) {
    println("LOOOOL!!");
    println(z);
})(20);

println("_________");
println(h);
println(1, 2 ,3, 4, 5, 6, 7);

const f = define(a) {
    a();
};

var m = 0;
while (m < 10) {
    local g = m * 2;
    f(define() { println(g); });
    m = m + 1;
}