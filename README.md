# stata-julia
Stata to Julia interface

A [Stata C Plugin](https://www.stata.com/plugins/) to [interface with Julia](https://docs.julialang.org/en/stable/manual/embedding/#High-Level-Embedding-1).

Note that the __stata-julia__ interface is still work in progress. It currently only "works" on MacOS.

# Using stata-julia

After installing Julia and setting the JULIA_HOME environment variable as described below, Julia commands can be run from Stata. To run, download the content of the `package` foler and run Stata in this. To invoke Julia from Stata, the `julia` program is used:

```
. julia , command(sin(2))
Result: 0.909297
```

To test the program, run `package/test.do` in Stata.

## Syntax

The Stata syntax is

```
julia [varlist] [if] [in] , [function(string)] [module(string)] [command(string)] [save(string)]
```

* `module` - is the Julia module that contains init parameters and the function to invoke.
* `function` - is the function to invoke, defined in the using file
* `command` - can be used to simply invoke a command without creating a script.
* `save` - specifies a filename to serialize the data in Julia in.

A variable `touse` is created with value 1 if in selection, 0 otherwise. This variable is created even if no selection has explicitly been set.

## The Julia code

* All matrices and dataset variables used have to be created in Stata before invoking `julia`.
* Before invoking the Julia method, the interface opens the Julia script specified in `using()` and imports the data as specified in this file.
* All data are in the global structure `stata`. It contins the dataset, and all imported matrices, scalars and macros.

* `stata.variable["name"]` - a Dict with the data variables
* `stata.matrix["name"]` - a Dict with all matrices
* `stata.global_macro["name"]` - a Dict with all macros
* `stata.scalar["name"]` - a Dict with all scalars

The function that is to be invoked has to have a method with one argument of type `StataData`.

### specifying data to be imported / exported

All data to be used has to be specified in advance. This can either be done with Stata options or in the Julia module.

The following options to the Stata `Julia` command can be used to specify what data to import and export:

 `setvariables(namelist)` - Variables to save to Stata
 `matrices(namelist)` - Matrices to use
 `setmatrices(namelist)` - Matrices to save, must also be in the used matrices
 `scalars(namelist)` - scalars to import
 `setscalars(namelist)` - scalars to export
 `macros(namelist)` - macros to import
 `setmacros(namelist)` - macros to export
 
Alternatively, the data required to invoke a function can be specified in the Julia script by defining a method taking no arguments, returning a Dict:

```
# List variables, matrices, macros and scalars to use
# All variables set must be in the corresponding get
my_func() = Dict(
 "set_variables" => "v nv",
 "get_matrices" => "A B nvm",
 "set_matrices" => "A B nvm",
 # macros and scalars can be set without predefining them in Stata
 "get_macros" => "global1 global2",
 "set_macros"  => "global1 global2",
 "get_scalars"  => "scalar1",
 "set_scalars"  => "scalar1 scalar2"
)
```

### The interface

The interface works as follows:

1) The user creates all data, including the variables to be calculated in Julia.
2) The `julia` Stata program is invoked, specifying:
    a) where the Julia init.jl file is
    b) which method to invoke
3) The `julia` program
    a) checks the init.jl script to see what data to import
    b) imports the data to the Julia global environment
    c) invokes the user's Julia method
4) After the method returns, the `julia` program
    a) modifies the Stata variables, and other data as specified in the init file init.jl
    b) returns to Stata, deleting all data in Julia.

The __stata-julia__ interface is not as ambitious as for example the R package `XRJulia`, using socket communication to enable repeated invocations.

## Limitations

- Missing values are not implemented.
- Only numerical variables can be included.

# Installing stata-julia

After installing Julia, the environment variable JULIA_HOME has to be set, specifying where the Julia executable and all it's libraries are located. The executable is then in the folder `$JULIA_HOME/bin`.

The `julia.ado` program is put in the personal ado-file folder. Stata 13 and Julia 1.0 or above are required.
