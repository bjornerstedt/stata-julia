capture program drop hello
program hello, plugin

// Note that a plugin is called with the plugin command, not just with its name.
plugin call hello
