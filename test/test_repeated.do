* Test calling statajulia
*   - several times
*   - without using julia.ado

version 13

program statajulia, plugin

matrix A = (1,2\3,4)
matrix B = A

* julia n v nv , using(init.jl)  function(test_get_set)
plugin call statajulia , test_repeated TestStataJulia
matlist A
matlist B
assertcount B[1,2] == A[2,1]
*program drop statajulia

*program statajulia, plugin
matrix A = 2*(1,2\3,4)

matrix B = A
* plugin call statajulia , "`function'"  "`using'"  "`command'"  "`save'"
plugin call statajulia  , test_repeated TestStataJulia
matlist A
matlist B

matrix A = 3*(1,2\3,4)

matrix B = A
* plugin call statajulia , "`function'"  "`using'"  "`command'"  "`save'"
plugin call statajulia  , test_repeated TestStataJulia
matlist A
matlist B
assertcount B[1,2] == A[2,1]
