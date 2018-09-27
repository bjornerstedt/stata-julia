* initvars.do - generates data to be used in tests

version 13
clear all
program statajulia, plugin using(statajulia.plugin)
* Generate example dataset:
set obs 10
gen n = _n
gen var1 = 10 * runiform()
gen var2 = 0.0
mkmat var2 , matrix( matfromvar)

* Generate other data
matrix A = (1,2\3,4)
* Matrix B will hold the transpose of A
matrix B = A

scalar scalar1 = 123.4
scalar scalar2 = 0

global global1 "Hello"
