* Test calling calljulia
*   - with dataset returning modified
*   - with macro
*   - with scalar

run initvars.do

plugin call calljulia , test_rw TestStataJulia

di "Global macro: global1: $global1"
di "Global macro: global2: $global2"
assertcount ("$global2" == "Modified Hello")

* Return: scalar2 = 2*scalar1
di scalar1
di scalar2
assertcount scalar2 == 2*scalar1

* Return: var2 = 4*var1
list
