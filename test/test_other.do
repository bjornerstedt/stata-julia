* Test calling statajulia
*   - command option
*   - serialising in Julia
*   - setting with Stata options instead of in Julia
*   - printing output to buffer

run initvars.do


julia , command(sin(2))

* Provide variables in Stata instead of Julia
julia * , module(TestStataJulia) func(test_rw_stata) mat(A B) setmat(matfromvar) mac(global1) setmac(global2) scal(scalar1) setscal(scalar2) setvar(var2)

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

* Save file
julia * ,  save("test_serial.bin") mat(A B) mac(global1) scal(scalar1) 
