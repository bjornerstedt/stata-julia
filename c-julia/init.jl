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
    return [1.0 2.0; 3.0 4.0; 5.0 6.0]
end

macros = "global1 global2"
matrices = "A B"
function initpars()
     return  macros
end
