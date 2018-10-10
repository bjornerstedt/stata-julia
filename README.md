# stata-julia
Stata to Julia interface

A [Stata C Plugin](https://www.stata.com/plugins/) to [interface with Julia](https://docs.julialang.org/en/stable/manual/embedding/#High-Level-Embedding-1).

Note that the __stata-julia__ interface is still work in progress. It currently only "works" on MacOS.

# Using stata-julia

After installing Julia and setting the JULIA_HOME environment variable as described below, Julia commands can be run from Stata. To run, download the content of the `package` folder and run Stata in this folder. To invoke Julia from Stata, the `julia` program is used:

```
. julia , command(sin(2))
Result: 0.909297
```

## Syntax

The Stata syntax is

```
julia [varlist] [if] [in] , [FUNCtion(name)] [module(name)] [command(string)] [save(string)] [MATrices(namelist)] [SCALars(namelist)] [MACros(namelist)] [all]
```

* `module` - is the Julia module that contains init parameters and the function to invoke.
* `function` - is the function to invoke, defined in the using file
* `command` - can be used to simply invoke a command without creating a script.
* `save` - specifies a filename to serialize the data in Julia in.

A variable `touse` is created with value 1 if in selection, 0 otherwise. This variable is created even if no selection has explicitly been set.

There is also a Julia method
```
statadata = stata(command::String, data::String, collect::String)
```

where collect can be `all|e|r|e r`. Note that this command is for the patient, as Julia is loaded each time the command is invoked.

## The Julia code

* Stata matrices and dataset variables cannot be created in Julia. To return data to Stata, dataset variables and matrices of the correct dimensions have to be created in Stata before invoking `julia`.
* All data are in the Julia struct `stata`, passed as the only argument to the function specified in the Stata `julia` command. It contains the dataset, and all imported matrices, scalars and macros.

* `stata.data["name"]` - a DataFrame with the data variables including `touse'
* `stata.matrix["name"]` - a Dict with all matrices
* `stata.global_macro["name"]` - a Dict with all macros
* `stata.scalar["name"]` - a Dict with all scalars

The function that is to be invoked has to have a method with one argument of type `StataData`.

### specifying data to be imported / exported

All data to be used has to be specified in advance. This can either be done with Stata options or in the Julia module.

The following options to the Stata `Julia` command can be used to specify what data to import and export:

- `matrices(namelist)` - matrices to use
- `scalars(namelist)` - scalars to import
- `macros(namelist)` - macros to import
- `collect(all|r e)` - Import all matrices, scalars and macros, including r() and e() values, or only r and/or e values. To import these Stata return values, copies with prefix `e_` and `r_` are created. These are deleted when the Julia method invoked returns.

To return data from Julia to Stata, the function `putstata()` is used:

```
putstata(stata, "scalars", "newscalar", 3)
putstata(stata, "matrices", "newmat", [1 2; 3 4])
putstata(stata, "macros", "newmacro", "Hello")
putstata(stata, "variables", "newvar", stata.data[:var1])
```

- Numerical data is imported and exported as doubles.
- Macros are strings. Local macros can be imported and exported by prefixing the name with an underscore `_` character.

### The interface

The interface works as follows:

1) The user creates all data, including the variables to be calculated in Julia.
2) The `julia` Stata program is invoked, specifying:
    a) what Stata data is to be sent to Julia
    b) which module and function to invoke in Julia
3) The `julia` program invokes the user's Julia function, providing a StataData struct as the only argument.
4) The user invokes `putstata` to indicate what data should be returned to Stata.
4) After the user function returns, the `julia` program
    a) modifies the variables in the Stata dataset, matrices, macros and scalars
    b) returns to Stata

## Limitations

- Missing values are not implemented.
- Only numerical variables can be included.

# Installing stata-julia

After installing Julia, the environment variable JULIA_HOME has to be set, specifying where the Julia executable and all it's libraries are located. The executable is then in the folder `$JULIA_HOME/bin`.

The `julia.ado` program is put in the personal ado-file folder. Stata 13 and Julia 1.0 or above are required.
