#include "MandelBrot.h"


int main(int c, char **v)
{

	init_gfx(&c, v); // starting  opengl
	
 
	glutMainLoop(); // enter glut event processing loop
	return 0;
}