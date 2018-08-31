# stata-julia
Stata to Julia interface

A [Stata C Plugin](https://www.stata.com/plugins/) to
[interface with Julia](https://docs.julialang.org/en/stable/manual/embedding/#High-Level-Embedding-1).

The program simplecall takes a Julia command as string and returns a Stata scalar.
Run `test.sh` to test the program.

The interface works as follows:

1) The user creates all data, including the variables to be calculated in Julia.
2) The `julia` Stata program is invoked, specifying
  - where the Julia init.jl file is
  - which method to invoke
3) The `julia` program
  1) checks the init.jl script to see what data to import
  2) imports the data to the Julia global environment
  3) invokes the user's Julia method
4) After the method returns, the `julia` program
  1) modifies the Stata variables, and other data as specified in init.jl
  2) returns to Stata, deleting all data in Julia.

The __stata-julia__ interface is not as ambitious as for example the R package `XRJulia`, using socket communication to enable repeated invocations.

# Using stata-julia

To invoke Julia from Stata, the `julia` program is invoked. The syntax is

```
julia [varlist] [if] [in] , method(string) using(string) [command(string)] [all]
```

* method - is the method to invoke
* using - is the Julia script that contains init parameters and the method to invoke.
* command - can be used to simply invoke a command without creating a script.
* all - import all matrices, macros and scalars from Stata

* Note that varlist, if and in are not implemented yet.


## The Julia code

* All matrices and dataset variables used have to be created in Stata before invoking `julia`.
* All data are in the global environment, as in Stata.
* Before invoking the Julia method, the interface opens the Julia init.jl script and
imports the data as specified in this file.

[Specify Julia code]

# Installing stata-julia

After installing Julia, the environment variables JULIA_HOME and... have to be set.
The `julia.ado` program is put in the personal ado-file folder. Stata 13 or above is required.
