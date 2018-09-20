version 13

program calljulia, plugin

matrix A = (1,2\3,4)
matrix B = A

* julia n v nv , using(init.jl)  function(test_get_set)
plugin call calljulia , test_get_set TestStataJulia
matlist A
matlist B
assert B[1,2] == A[2,1]
*program drop calljulia

*program calljulia, plugin
matrix A = 2*(1,2\3,4)

matrix B = A
* plugin call calljulia , "`function'"  "`using'"  "`command'"  "`save'"
plugin call calljulia  , test_get_set TestStataJulia
matlist A
matlist B

matrix A = 3*(1,2\3,4)

matrix B = A
* plugin call calljulia , "`function'"  "`using'"  "`command'"  "`save'"
plugin call calljulia  , test_get_set TestStataJulia
matlist A
matlist B
assert B[1,2] == A[2,1]
