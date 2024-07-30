define fn() {
    const h = 200;

    const f = define(adder) {
        return h * adder;
    };

    return f;
}

var m = 0;
while (m++ < 10) {
    println(">>", fn()(m));
}
