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
