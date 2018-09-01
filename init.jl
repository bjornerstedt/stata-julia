# test.jl

matrices = ["A" "B"]
inmatrix = "A"
outmatrix = "B"

# Should set a bunch of vars
function set_global_var(s::AbstractString, v::Any)
         s = Symbol(s)
         @eval (($s) = ($v))
end

function modify_global_var()
    global W = 32
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
     x[:] = 2 .* x
end

set_global_var("x", 2)

x
