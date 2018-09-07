module MyModule

export x, y

x() = "x"
y() = "y"
p() = "p"

end

using MyModule

x()

MyModule.p()
