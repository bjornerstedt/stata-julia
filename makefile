all: hello.plugin
	gcc -bundle -DSYSTEM=APPLEMAC stplugin.c hello.c -o hello.plugin

clean:
	 rm hello.plugin
