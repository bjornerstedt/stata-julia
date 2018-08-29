all: hello.plugin
	/Applications/Julia-0.6.app/Contents/Resources/julia/share/julia/julia-config.jl --cflags --ldflags --ldlibs | xargs gcc -bundle -DSYSTEM=APPLEMAC stplugin.c hello.c -o hello.plugin

clean:
	 rm hello.plugin
