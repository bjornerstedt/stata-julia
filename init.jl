# test.jl

matrices = ["A" "B"]
inmatrix = "A"
outmatrix = "B"

function set_global_var(x)
     global B = x
end

function doubleall(x::Array, y::Array)
     y[:] = 2.*x
end

 function testf(x::Array)
     y = 2.*x
     x = y
 end

function f(x::String)
    "X"
    # ("X", "Y")
end

g(x,y) = x + 2y

h(x...) = min(x...)

rev = reverse!

function testf!(x::Array)
     x[:] = x
end
