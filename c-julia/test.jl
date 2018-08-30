# test.jl

include("init.jl")
x = f("test")
x

# Create matrices and test
X = [1 2;3 4]
Y = [1 2;3 4]
f(X,Y)

Y

h(2,1,3)

module TestModule
i = 1
global i

h(x...) = min(x...)

f() = i
function f(x)
    global i
    i = x
end

end

# To modify x in place:
function testf!(x::Array)
     x[:] = transpose(x)
end

TestModule.f()
TestModule.f(6)
x = [1.0 2 3; 4 5 6]


testf!(x)

A = [1 2 3; 4 5 6; 7 8 9]
transpose(A)

function f(x::Array, y::Array)
     y[1,1] = 3
 end

y = A

testf!(A)

A
