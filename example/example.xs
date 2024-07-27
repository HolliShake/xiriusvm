
var x = false && true;
var y = 100 * 5 - 1.5;
var z = 1 << 32;

println(x);
println(y);
println(z);
println(3 < 2);

if (true && false || true) 
    println(103);
else if (false || true)
    println(203);
else 
    println(303);