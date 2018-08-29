# test.jl

function f(x::Array,y::Array)
     y = 2.*x
 end

function f(x::String)
    "X"
    # ("X", "Y")
end

g(x,y) = x + 2y

h(x...) = min(x...)
