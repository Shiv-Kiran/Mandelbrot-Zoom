/*
    -------------------------------------------------------
        Mandelbrot Set Zoom using OpenGL graphics package
        Created by Shiv Kiran Bagathi
        Roll Number : 200050019
    -------------------------------------------------------
*/

Make sure to install opengl and gmp 

To run make you have files in this order

├── MandelBrot.h
├── README.txt
├── glut32.dll
├── glut32.lib
├── main.cpp
├── resources.txt
├── run.exe
└── src
    └── include
        └── GL
            ├── freeglut.h
            ├── freeglut_ext.h
            ├── freeglut_std.h
            ├── freeglut_ucall.h
            └── glut.h

3 directories, 12 files

******!!!!!! MAKE SURE ALL FILES ARE IN PROPER ORDER BEFORE RUNNING !!!!!!*******
Command to create executable file in windows: g++ -Isrc/Include -o run main.cpp -mwindows glut32.lib -lopengl32 -lglu32 -lgmp 


open run  

q - to quit and close the tab
v - to allow/disallow mouse click to zoom
left click -> zooms by 10x
right click -> zoom out by 10x
[ -> zoom at center by 2X
] -> zoom out by  2x
r - to reset figure
b - to toggle between black-white and color
c - to change color
> or . - to increase iterations 
< or , - to decrease iterations
x - to maximize iterations
z - to minimize itertaions
' ' - to invert colors


any click - centers the point as center of window


Implemented using Opengl Library

/*
	For any point c_0 = a_0 + b_0i
	MandelBrot main idea f(z) = z^2 + c if converged then the initial point c_0 is part of mandelbrot set 
	we have to start with z =0 and keep going f(f(z)) 
	different iterations of converging can be colored differently to show the difference
	Ex;
		   c_0 = a_0 + b_0i
		f(0) = 0 +c -> f(0) = a_0 + b_0 i
		f(f(0)) = a^2 -b^2 +a_0 + i( 2ab + b_0 )
	if the value a^2 + b^2 is > 4 then it diverges

*/




