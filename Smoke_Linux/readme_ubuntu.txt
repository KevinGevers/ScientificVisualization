Compilation and execution:
1) Open a terminal using "CTRL+T" command
2) Run "cd" command to change the working directory to the directory where the code is downloaded (Normally, you should find it in Downloads)
3) Run "make" command. It creates an executable called "smoke"
4) Execute it using "./smoke"

Note:
 If compilation fails, check if the makefile links the glut libraries properly. It should link "-lglut -lGL -lGLU -lGLEW -lm" in LIBS
