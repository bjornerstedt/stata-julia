# test.jl

function f(x::Array,y::Array)
     y = 2.*x
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
     x[:] = 3.*x
end

function rowcol()
    [1 2; 3 4; 5, 6]
end