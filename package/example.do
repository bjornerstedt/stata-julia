* Example script calling statajulia

matrix A = (1,2\3,4)
* Create matrix to store results in:
matrix B = (0,0\0,0)

scalar scalar1 = 123.4

* Call julia sending scalar1 and matrix A
julia , module(StataExample)  function(example_func) scalars(scalar1) matrices(A)

di scalar1
di scalar2

matlist A
* Transpose of A:
matlist B
