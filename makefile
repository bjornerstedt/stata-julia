all: simplecall.plugin
	/Applications/Julia-0.6.app/Contents/Resources/julia/share/julia/julia-config.jl --cflags --ldflags --ldlibs | xargs gcc -bundle -DSYSTEM=APPLEMAC stplugin.c simplecall.c -o simplecall.plugin

clean:
	 rm simplecall.plugin
