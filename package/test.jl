# test.jl

include("init.jl")


# To modify x in place:
function testf!(x::Array)
     x[:] = transpose(x)
end

TestModule.f()
TestModule.f(6)
x = [1.0 2 3; 4 5 6]

addScalar["Test", [2.2 3]]
x

A = [1 2 3; 4 5 6; 7 8 9]
transpose(A)

function f()
     B = A
end

function g(x)
     global B = x
end


get_variables = "n v nv"
set_variables = " v nv "

function getVarIndices(n::Integer)
    if length(set_variables) < n || n <= 0
        return("")
    end
    set_names = split(strip(set_variables), r" +")
    get_names = split(strip(set_variables), r" +")
    get_names[n]
end

function getVarIndices(n::Integer)
    if length(set_variables) < n || n <= 0
        return("")
    end
    set_names = split(strip(set_variables), r" +")
    get_names = split(strip(set_variables), r" +")
    get_names[n]
end

getVarIndex(-1)
isSetVar("v")

addtodict("HÅ",1)
addtodict("HEJ",1)
getfromdict("HÅ")
dict
Ab = ["x" "y" "z" ;A]

Ab
using DataFrames

df = convert( DataFrame,A)

split("Hej hå", ' ' )
addMacro("test", 2)

test_get_set()

b=IOBuffer()
show(b, "text/plain", rand(3,3))
write(b,"abc")
print(String(b))

function testPrint()
    b=IOBuffer()
    show(b, "text/plain", rand(3,3))
    # write(b,"abc")
    String(b)
end
testPrint()
