# test.jl

include("init.jl")


# To modify x in place:
function testf!(x::Array)
     x[:] = transpose(x)
end

TestModule.f()
TestModule.f(6)
x = [1.0 2 3; 4 5 6]


x

A = [1 2 3; 4 5 6; 7 8 9]
transpose(A)

function f()
     B = A
end

function g(x)
     global B = x
end

dict = Dict()

function addtodict(x::String, y)
    dict[x] = y
end

function getfromdict(x::String)
    dict[x]
end

addtodict("HÅ",1)
addtodict("HEJ",1)
getfromdict("HÅ")
dict
