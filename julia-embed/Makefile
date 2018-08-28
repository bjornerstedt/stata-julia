all: invoke_julia
	gcc -o test -fPIC -I$JULIA_DIR/include/julia -L$JULIA_DIR/lib invoke_julia.c -ljulia $JULIA_DIR/lib/julia/libstdc++.so.6

clean:
	 rm hello.plugin
