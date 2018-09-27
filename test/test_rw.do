* Test calling statajulia
*   - with dataset returning modified
*   - with macro
*   - with scalar

run initvars.do

* varnames have to be sent explicitly, as Stata plugin doesn't report them
plugin call statajulia n var1 var2, test_rw TestStataJulia "n var1 var2"

di "Global macro: global1: $global1"
di "Global macro: global2: $global2"
assertcount ("$global2" == "Modified Hello")

* Return: scalar2 = 2*scalar1
di scalar1
di scalar2
assertcount scalar2 == 2*scalar1

* Return: var2 = 3*var1
list
assertcount 3 * n[1] == var2[1]


matlist matfromvar
assertcount var1[1] == matfromvar[1,1]
