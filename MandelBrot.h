/*
    -------------------------------------------------------
        Mandelbrot Set Zoom using OpenGL graphics package
        Created by Shiv Kiran Bagathi
        Roll Number : 200050019
    -------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include<fstream>
#include <iostream>
#include <gmp.h>

using namespace std;

/* 

	GLOBAL VARIABLES

*/

int windowWidth = 750;
int windowHeight = 750;
int centerX = -0.5;
int centerY = 0;
bool play = false;
int playX = -1.5;
int playY = 0;


int title;   // For title in window
GLuint texture; // unsigned  binary integer
int width, height;
int color_width, color_height;
double scale = 4./windowWidth;
// double cx = -exp(1)/7, cy = -(exp(1))/20;
double cx = centerX , cy = centerY;

int color_rotate = 0; // extra features rotation of 6 variations
int BlacknWhite = 0; // extra features  for black and white
int invert = 0; // for inverting color
int maximumIterations = 256;



/*
	Datatype defined to store colors
*/

typedef struct {
	unsigned char redColor, greenColor, blueColor;
	} pixel_color;
pixel_color **pixel_data = 0; // storing n*m pixels color



/* 

	Functions Used

*/


void setWindowTexture();
void zoomIn();
void zoomOut();
void gmpZoom();
void init_gfx(int *c, char **v);
void resize(int w, int h);
void mouseclick(int button, int state, int x, int y);
void alloc_pixels();
void gmpZoom();
int getRed(int hue);
int getGreen(int hue);
int getBlue(int hue);
void render();
void moveUp();
void moveDown();
void moveLeft();
void moveRight();
void controlKeys(int key, int x, int y);
void keypress(unsigned char key, int x, int y);
void color_pixel(int hue, int min, int max, pixel_color *p);
double map_cordi(double value, double diff_in,double scale, double diff_out);
void calc_mandel();
void calc_mandel1();


/*
	Functions Used End

*/



// mpf_set_default_prec (20);

// Precision for infinite zoom without pixelating
// mpz_t PRECISION = 20;
// mpf_set_default_prec (20);


int color_pallete[16][3] = { {66,30,15},
						{25, 7, 26},
						{9, 1, 47},
						{4, 4, 73},
						{0, 7, 100},
						{12, 44, 138},
						{24, 82, 177},
						{57, 125, 209},
						{134, 181, 229},
						{211, 236, 248},
						{241, 233, 191},
						{248, 201, 95},
						{255, 170, 0},
						{204, 128, 0},
						{153, 87, 0},
						{106, 52, 3}
};

// FUNCTIONS DEFINITIONS

/* 
	Functions to map special color from color pallete ( This color is in wikepedia and available on stackoverflow)
*/
int getRed(int hue) {
	if (hue < 10)return 0;
	else return color_pallete[hue % 16][0];
}

int getGreen(int hue) {
	if (hue < 10 )return 0;
	else return color_pallete[hue % 16][1];
}
int getBlue(int hue) {
	if (hue < 0) return 0;
	else return color_pallete[hue % 16][2];
}

/*

	FUNCTION TO DISPLAY IMAGE TO SCREEN

*/

void render()
{
	double	x = (double)width /color_width;
	double	y = (double)height/color_height;

	glClear(GL_COLOR_BUFFER_BIT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, texture);

	glBegin(GL_QUADS); // for the window
	// vertex are end points of window in which the image is displayed
	// texCoord2f changes the texture cordinates inside the images i.e
	// distorts the image

	glTexCoord2f(0, 0); glVertex2i(0, 0); // setting coordinates of texture
	// glTexCoord2f sets the coordinates of texture
	glTexCoord2f(x, 0); glVertex2i(width, 0);
	glTexCoord2f(x, y); glVertex2i(width, height);
	glTexCoord2f(0, y); glVertex2i(0, height);

	glEnd();

	glFlush();
	glFinish();
}

/* Function when arrow keys are pressed */


void moveUp(){

	cy -= ( - height/ 4) * scale; // cy mouseclick zooms y position to that point

}
void moveLeft(){

	cx += ( - width / 4) * scale; // moves left by the small distance

}
void moveRight(){

	cx -= ( - width / 4) * scale; // moves left by the small distance

}
void moveDown(){

	cy += ( - height/ 4) * scale; // cy mouseclick zooms y position to that point

}

/* Function which directs arrow keys */

void controlKeys(int key, int x, int y){
	switch(key){
		case GLUT_KEY_UP :moveUp(); break;

		case GLUT_KEY_DOWN : moveDown(); break;

		case GLUT_KEY_LEFT: moveLeft(); break;

		case GLUT_KEY_RIGHT: moveRight(); break;
	}
	setWindowTexture();
}

/* Function which directs alphabet keys */

void keypress(unsigned char key, int x, int y)
{
	switch(key) {
		case 'v':
				if(play){
					play = false;
				}else play = true;
				break;

		case 'q':	glFinish();
					glutDestroyWindow(title); // to close the progam
					exit(0);
					glutPostRedisplay();
					return;

		case 'r':	scale = 4./windowHeight;
					cx = centerX, cy = centerY;
					//  cx = -.6; cy = 0;
					break;

		case 'c':	color_rotate = (color_rotate + 1) % 6;
					break;

		case '>': case '.':
				maximumIterations += 128;
				if (maximumIterations > 1 << 12){
					maximumIterations = 1 << 12;
				}
				break;

		case '<': case ',':
				maximumIterations -= 128;
				if (maximumIterations < 128) {
					maximumIterations = 128;
					}
				break;

		case 'b':	BlacknWhite = 1 - BlacknWhite;
				break;

		case 'x':	maximumIterations = 4096; break;

		case 'z':	maximumIterations = 128; break;

		case ']':  zoomIn(); return;

		case '[': zoomOut(); return;

		case 'g': gmpZoom();return;

		case ' ':	invert = !invert;

	}
	setWindowTexture(); // repeads the process of rendering
}

/* Function which colors every pixel */

void color_pixel(int hue, int min, int max, pixel_color *p)
{
	// hue tells how many iterations the point need to diverge
	if (min == max) max = min + 1;

	if (invert) hue = max - (hue - min); // inverting colors

	if (!BlacknWhite) {
		p->redColor = p->greenColor = p->blueColor = 255 * (max - hue) / (max - min);
		return;
	}

	double h = fmod(color_rotate + 1e-4 + 4.0 * (hue - min) / (max - min), 6); // each h creates new color
	int color_max = 255;
	double c = color_max * BlacknWhite;
	double X = c * (1 - fabs(fmod(h, 2) - 1)); // create alternate color depending on h

	p->redColor = p->greenColor = p->blueColor = 0;

	switch((int)h) {

	case 0: p->redColor = c; p->greenColor = X; return;
	case 1:	p->redColor = X; p->greenColor = c; return;
	case 2: p->greenColor = c; p->blueColor = X; return;
	case 3: p->greenColor = X; p->blueColor = c; return;
	case 4: p->redColor = X; p->blueColor = c; return;
	case 5: p->redColor = getRed(hue);
	 p->greenColor = getGreen(hue);
	 p->blueColor = getBlue(hue); return;
	default:p->redColor = c; p->blueColor = X;

	}
}

/* Function which maps x , y values */

double map_cordi(double value, double diff_in,double scale, double diff_out) {

	return (value - diff_in) * scale + diff_out;
}

/* Function to map to special color */

void to_color(int hue,pixel_color *p){
	int red = getRed(hue);
	int blue = getBlue(hue);
	int green = getGreen(hue);
	p->redColor = red ;
	p->greenColor = green;
	p->blueColor = blue;
}



/*

	GMP implementation 

*/
/* 		

	Tried to Implement gmp to tackle precision problem present in double
	But did'nt get expected results 

*/


void init_gmp( mpf_t *const var, int bit_size){
	mpf_init(*var);
	mpf_set_prec(*var, bit_size);
}

// mpz_t ->integer
// mpq_t -> rational number
// mpf_t -> floating point


void calc_mandel1()
{
	int  iter, min, max;

	// iter for calculating iterations for that pixel
	pixel_color *px;

	mpf_t x,y,zx,zy,zx2,zy2,length;
	// mpf_init(x);
	// mpf_init(y);
	// mpf_init(zx);
	// mpf_init(zy);
	// mpf_init(zx2);
	// mpf_init(zy2);
	// mpf_init(length);

	mpf_init_set_d(x,0);
	mpf_init_set_d(y,0);
	mpf_init_set_d(zx ,0);
	mpf_init_set_d(zy ,0);
	mpf_init_set_d(zx2 ,0);
	mpf_init_set_d(zy2 ,0);
	mpf_init_set_d(length,1);

	init_gmp(&x,40);
	init_gmp(&y,40);
	init_gmp(&zx ,40);
	init_gmp(&zy ,40);
	init_gmp(&zx2 ,40);
	init_gmp(&zy2 ,40);
	init_gmp(&length ,40);

	min = maximumIterations; max = 0;
	// double b = map(y, 0, HEIGHT/2, min, max);
	int i,j;
	for (int i = 0; i < height; i++) {
		px = pixel_data[i];
		//  taking first pixel row

		for (int j = 0; j  < width; j++, px++) {

			mpf_t  tempi,tempj,temph,tempw,tempsc,tempcy,tempcx;
			mpf_init(tempi);
			mpf_init(tempj);
			mpf_init(temph);
			mpf_init(tempw);
			mpf_init(tempsc);
			mpf_init(tempcy);
			mpf_init(tempcx);

			mpf_set_d(tempi,i );
			mpf_set_d(tempj,j );
			mpf_set_d(temph,height/2 );
			mpf_set_d(tempw, width/2 );
			mpf_set_d(tempsc, scale );
			mpf_set_d(tempcy, cy );
			mpf_set_d(tempcx,cx );



			mpf_sub(y , tempi,temph);
			mpf_mul(y,y,tempsc);
			mpf_add(y,y,tempcy);

			mpf_sub(x, tempj,tempw);
			mpf_mul(x,x,tempsc);
			mpf_add(x,x,tempcx);

			// y = (i - height/2) * scale + cy;
			// x = (j - width/2) * scale + cx;


			for (iter = 0; iter < maximumIterations ; iter++) {

				// zy = 2 * zx * zy + y; // 2ab + b_0
				mpf_mul(zy, zx, zy );
				mpf_mul_ui(zy,zy,2);
				mpf_add(zy,zy,y);

				// zx = zx2 - zy2 + x; // a^2 +b^2 + a_0
				mpf_sub(zx,zx2,zy2);
				mpf_add(zx,zx,x);

				mpf_mul(zx2,zx,zx);
				mpf_mul(zy2,zy,zy);

				// zx2 = zx * zx; // zx2 will is a^2 in next step
				// zy2 = zy * zy; // zy2 is b^2 in next step

				mpf_add(length,zx2,zy2);

				if( mpf_cmp_ui(length,16) > 0 ){
					break;
					// to check if the values are divergining
				}
			}

			if (iter < min) min = iter; // if pixel is inside
			if (iter > max) max = iter; // if pixel is outside
			*(unsigned short *)px = iter; // for different colors at every pixel
		}
	}

	for ( i = 0; i < height; i++){
		for ( j = 0, px = pixel_data[i]; j  < width; j++, px++){
			color_pixel(*(unsigned short*)px, min, max, px);
			// to_color(*(unsigned short*) px ,px);
		}
	}

}

void gmpZoom(){
	alloc_pixels();
	calc_mandel1();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, color_width, color_height,
		0, GL_RGB, GL_UNSIGNED_BYTE, pixel_data[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	render();

}

/* End of GMP implementation */

/*

  	MAIN PART OF CHECKING OF A POINT IF IT BELONG TO MANDELBROT SET 

*/

void calc_mandel()
{

	int  iter, min, max;
	// iter for calculating iterations for that pixel
	pixel_color *px;
	double x, y, zx, zy, zx2, zy2;
	min = maximumIterations; max = 0;
	// double b = map(y, 0, HEIGHT/2, min, max);
	int i,j;
	for (int i = 0; i < height; i++) {
		px = pixel_data[i];
		//  taking first pixel row

		for (int j = 0; j  < width; j++, px++) {
			// incrementing px column element
			y = map_cordi (i,height/2,scale,cy);
			x = map_cordi(j,width/2,scale,cx);

/*

			MandelBrot main idea f(z) = z^2 + c if converged then the initiak point c_0 is part of mandelbrot set
			we have to start with z =0 and keep going f(f(z))
			different iterations of converging can be colored differently to show the difference
			Ex;
		   		c_0 = a_0 + b_0i
				f(0) = 0 +c -> f(0) = a_0 + b_0 i
				f(f(0)) = a^2 -b^2 +a_0 + i( 2ab + b_0 )

*/

			// counting iterations for color variation

			// for all other values of x,y
			zx = zy = zx2 = zy2 = 0;
			for (iter = 0; iter < maximumIterations ; iter++) {

				zy = 2 * zx * zy + y; // 2ab + b_0
				zx = zx2 - zy2 + x; // a^2 +b^2 + a_0
				zx2 = zx * zx; // zx2 will is a^2 in next step
				zy2 = zy * zy; // zy2 is b^2 in next step

				if( zx2 + zy2 > 4){
					break;
					// to check if the values are divergining
				}
			}

			if (iter < min) min = iter; // for colors
			if (iter > max) max = iter; // for color pallete
			*(unsigned short *)px = iter; // for different colors at every pixel
		}
	}

	for ( i = 0; i < height; i++){
		for ( j = 0, px = pixel_data[i]; j  < width; j++, px++){
			color_pixel(*(unsigned short*)px, min, max, px);
			// to_color(*(unsigned short*) px ,px);
		}
	}

}


/*

 Function to initialize pixel_data variable according to size of window

*/

void alloc_pixels() // initializing  pixel_data
{
	int i, initial_w = color_width, initial_h = color_height;

	for (color_width = 1; color_width < width;  color_width <<= 1);
	// set one bit left if color_width < widht
	for (color_height = 1; color_height < height; color_height <<= 1);
	// similary one bit left if color_height < height till it reaches height

	// if color_height or color_width are different from inital then reallocation of pixel_data -> variable for storing color on whole window
	if (color_height != initial_h || color_width != initial_w){
		pixel_data = (pixel_color**) realloc(pixel_data, color_height * color_width * 3 + color_height * sizeof(pixel_color*));
		//  3*color_height*color_width for r g b for all pixels
		// allocating pixel_data newly (or first time based on window width)
	}


	for (pixel_data[0] = (pixel_color *)(pixel_data + color_height), i = 1; i < color_height; i++){
		pixel_data[i] = pixel_data[i - 1] + color_width; // each row having color_width column
	}

}

/*	

	 MAIN FUNCTION TO INITIALIZE TEXTURE OF IMAGE DATA , PARAMETERS RELATED TO OPENGL
	 FINALLY SENDING TO RENDER FUNCTION 

*/

void setWindowTexture()
{
	alloc_pixels();
	calc_mandel();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture); // binding GL_TEXTURE_2D only texture  as we are dealing with images
	glTexImage2D(GL_TEXTURE_2D, 0, 3, color_width, color_height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixel_data[0]);
	// 0 means base level, 3 is for 3 colors
	// color_width and height of the image

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	// these gl_texture_min_filter and mag_filter makes the pixels smoother
	// min for zooming out max is for zooming in
	render();
}

/* 
	FUNCTION WHICH OBSERVES MOUSE CLICKS

*/

void mouseclick(int button, int state, int x, int y)
{
	if (state != GLUT_UP) return; // if the state is pressed dont do anything
	if(play){
		cx += (x - width / 2) * scale; // cx determine the zoom position using mouseclick
		cy -= (y - height/ 2) * scale; // cy mouseclick zooms y position to that point

		switch(button) {
		case GLUT_LEFT_BUTTON: // left click zooms
			// if (scale > fabs(x) * 1e-16 && scale > fabs(y) * 1e-16)
				scale /= 10; // zooming by 1000%
			break;
		case GLUT_RIGHT_BUTTON: // right click zoom outs
			scale *= 10; // zooming out by 10%
			break;
		/* any other button recenters */
		}

	}
	else{
		cx += (x - width / 2) * scale;
		cy -= (y - height/ 2) * scale;

	}
	setWindowTexture();
}

// // best point to zoom is -e/7 , -e/20

void zoomIn(){

	scale = scale/2;
	setWindowTexture();
}

void zoomOut(){

	scale = scale*2;
	setWindowTexture();
}

/* FUNCTION TO OBSERVE CHANGES TO WINDOW */

void resize(int w, int h)
{ 
	// function for resizing but not completely implemented
	width = w;
	height = h;
	// float aspectRatio = (float) w/h;
	// glLoadIdentity();
	glViewport(0, 0, w, h);
	glOrtho(0, w, 0, h, -1, 1); // perspective matrix in standarad forms
	// gluPerspective(45,aspectRatio,.001,1000);
	setWindowTexture();
}

/*	
	FUNCTION TO INITIALIZE OPENGL LIBRARY 

*/

void init_gfx(int *c, char **v)
{

	glutInit(c, v); // initializing glut library
	glutInitDisplayMode(GLUT_RGB); // setting initial display
	glutInitWindowSize(windowWidth, windowHeight); // window size
	glutInitWindowPosition(100, 10); // window position on the laptop screen

	title = glutCreateWindow("Mandelbrot ZooooooMMMM");// for title
	glutDisplayFunc(render); // sets display callback for current window
// all the rendering is done to current window
	glutSpecialFunc(controlKeys);

	glutKeyboardFunc(keypress);
	// keypress function is acted on current window
	glutMouseFunc(mouseclick); // similary mouse clicks left and right
	//  are acted on current window
	glutReshapeFunc(resize);
	// whenever resize function make height and widht to be current height and width
	glGenTextures(1, &texture);
	// one texture of GL_TEXTURE_2D for images are images are 2d arrays
// OpenGl Object that contains images of same format

	setWindowTexture();
}
