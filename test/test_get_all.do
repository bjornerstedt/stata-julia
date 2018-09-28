* Test calling statajulia
*   - getting all data with the all option

run initvars.do

quietly reg var1 n
matlist e(b)
sum var1

matrix eb = 0 * e(b)
matlist eb
global global2

julia * if n < 6, module(TestStataJulia2)  function(test_get_all) all

local x : all matrices
di "Matrices: `x'"
matrix eb0 = e(b)
matlist eb
assertcount eb0[1, 1] == eb[1, 1]

di "Created scalar: ", newscalar
assertcount newscalar == 3

di "Created macro: $newmacro"
assertcount ("$newmacro" == "Hej")

list
assertcount var2[1] == var1[1]
