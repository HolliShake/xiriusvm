
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