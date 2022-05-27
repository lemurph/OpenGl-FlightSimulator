// Logan Murphy - B00835378 - Intro to Computer Graphics and Animation Final Project

// Include header files
#include <freeglut.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "./stb_image.h"
#include "./deque.c"





// Define constants
#define PI 3.14159265
// The grid will have tiwice this many vertices
#define GRID_SIZE 50
#define HEIGHT_MAX 20
#define HEIGHT_MIN 0
// Size for mountain mesh
#define SIZE 64


// Texture ID's
GLint mountainTexture, seaTexture, skyTexture;

// Texture parameters
GLint mountainTexWidth, mountainTexHeight;
GLint skyTexWidth, skyTexHeight;
GLint seaTexWidth, seaTexHeight;

unsigned char* mountainTexPixels;
unsigned char* skyTexPixels;
unsigned char* seaTexPixels;

GLUquadric* diskQuad;
GLUquadric* cylinderQuad;

// Mountain height arrays
float m1[64][64];
float m2[64][64];
float m3[64][64];

// Mountain normal arrays
float normalsM1[64][64][3];
float normalsM2[64][64][3];
float normalsM3[64][64][3];

// Arrays for cessna file input
GLint cessnaID, propellerID;

// Struct for point data (normals and vertices)
typedef struct NV {
	float vertX, vertY, vertZ, normX, normY, normZ;
}NV;

int propRotate = 0;

// Window size parameters
int windowHeight = 800;
int windowWidth = 1400;

// Declare initial camera position
double camPos[3] = { 1.0, 1.0, 10.0 };;
// Initial camera target
double lookatPosition[] = { 0.0, 0.0, -1.0 };
float cameraAngle = 0.0;

// Keypress flags
GLboolean pageUp = 0;
GLboolean pageDown = 0;
GLboolean downArrow = 0;
GLboolean upArrow = 0;

// Key toggles
GLboolean drawWireframe = 1;
GLboolean fullscreen = 0;
GLboolean gridView = 1;
GLboolean fogToggle = 0;
GLboolean mountainTextureMode = 0;
GLboolean rollingLeft = 0;
GLboolean rollingRight = 0;
GLboolean stealthMode = 0;

// Material arrays			  
GLfloat yellow[4] = { 0.847, 0.721, 0.074, 0.1 };
GLfloat black[4] = { 0, 0, 0, 0.1 };
GLfloat lightPurple[4] = { 0.619, 0.168, 0.631, 0.1 };
GLfloat blue[4] = { 0.223, 0.262, 0.635, 0.1 };
GLfloat red[4] = { 0.960, 0.258, 0.345, 0.1 };
GLfloat grey[4] = { 0.411, 0.411, 0.411, 0.1 };


// Initial speed for plane
float speed = 0.03;

// Variables for mouse controls
int mouseX, mouseY;
float planeRoll = 0;


/************************************************************************
	Function:		createMountain
	
	Sources Used:	https://learn.64bitdragon.com/articles/computer-science/procedural-generation/the-diamond-square-algorithm
					https://github.com/c-utils/dque

	Description:	Create a mountain
*************************************************************************/
void generateMountain(float mountainVertices[64][64]) {
	// Initial random amount
	int random = rand() % 16 + 5;

	// Calculate middle
	int midX = (SIZE - 1) / 2;
	int midY = (SIZE - 1) / 2;

	// Set height of middle vertex 
	mountainVertices[midX][midY] = random + 1;

	// Create deque and queue the top, left, bottom, right and random amount
	deque_type* dqueue = deque_alloc();
	GLboolean firstIteration = 1;
	int qsize = 5;
	deque_push_back(dqueue, 0);
	deque_push_back(dqueue, 0);
	deque_push_back(dqueue, (SIZE - 1));
	deque_push_back(dqueue, (SIZE - 1));
	deque_push_back(dqueue, random);
	
	// While there are still items in the queue
	while ( qsize != 0) {
		
		int top, left, bottom, right;

		// Dequeue the five items from the front
		top = deque_pop_front(dqueue);
		left = deque_pop_front(dqueue);
		bottom = deque_pop_front(dqueue);
		right = deque_pop_front(dqueue);
		random = deque_pop_front(dqueue);

		// The queue has five less items
		qsize -= 5;

		// Calculate middle of the five dequeued parameters
		midX = (left + right) / 2;
		midY = (top + bottom) / 2;
		

		/* Calculate the following (X's)
	        O----X----O
			|	 |	  |        
			X----X----X
			|    |    |
			O----X----O
		*/
		// If it's the first iteration; dont add the random amount
		if (firstIteration) {
			// Four edges
			mountainVertices[midX][top] = ((mountainVertices[left][top] + mountainVertices[right][top]) / 2);
			mountainVertices[midX][bottom] = ((mountainVertices[left][bottom] + mountainVertices[right][bottom]) / 2);
			mountainVertices[left][midY] = ((mountainVertices[left][top] + mountainVertices[left][bottom]) / 2);
			mountainVertices[right][midY] = ((mountainVertices[right][top] + mountainVertices[right][bottom]) / 2);

			firstIteration = 0;
		}
		else {
			// Four edges
			mountainVertices[midX][top] = ((mountainVertices[left][top] + mountainVertices[right][top]) / 2 + random);
			mountainVertices[midX][bottom] = ((mountainVertices[left][bottom] + mountainVertices[right][bottom]) / 2 + random);
			mountainVertices[left][midY] = ((mountainVertices[left][top] + mountainVertices[left][bottom]) / 2 + random);
			mountainVertices[right][midY] = ((mountainVertices[right][top] + mountainVertices[right][bottom]) / 2 + random);
			// Middle Point
			mountainVertices[midX][midY] = ((mountainVertices[left][top] + mountainVertices[right][top]
				+ mountainVertices[left][bottom] + mountainVertices[right][bottom]) / 4 + random);
		}
		

		// If we can make another loop queue the bounding parameters like this as to form four quadrants
		if ((right - left) > 2) {
			deque_push_back(dqueue, top);
			deque_push_back(dqueue, left);
			deque_push_back(dqueue, midY);
			deque_push_back(dqueue, midX);
			deque_push_back(dqueue, (random / 2));

			deque_push_back(dqueue, top);
			deque_push_back(dqueue, midX);
			deque_push_back(dqueue, midY);
			deque_push_back(dqueue, right);
			deque_push_back(dqueue, (random / 2));

			deque_push_back(dqueue, midY);
			deque_push_back(dqueue, left);
			deque_push_back(dqueue, bottom);
			deque_push_back(dqueue, midX);
			deque_push_back(dqueue, (random / 2));

			deque_push_back(dqueue, midY);
			deque_push_back(dqueue, midX);
			deque_push_back(dqueue, bottom);
			deque_push_back(dqueue, right);
			deque_push_back(dqueue, (random / 2));

			qsize += 20;
		}
		
		
	}

}

/************************************************************************
	Function:		calcMountainNormals

	Source:			https://stackoverflow.com/questions/13983189/opengl-how-to-calculate-normals-in-a-terrain-height-grid

	Description:	Calculates approximated vertex normals for the mountains
*************************************************************************/
void calcMountainNormals() {

	// Iterate over the heightmap of vertices of m1 (non-inclusive)
	for (int x = 1; x < 63; x++) {
		for (int z = 1; z < 63; z++) {
			// Gather the heights of the four adjacent vertices
			float hL = m1[x - 1][z];
			float hR = m1[x + 1][z];
			float hD = m1[x][z + 1];
			float hU = m1[x][z - 1];

			// Approximate the current vertex normal
			normalsM1[x][z][0] = hL - hR;
			normalsM1[x][z][1] = hD - hU;
			normalsM1[x][z][2] = 2.0;
		}
	}

	// Iterate over the heightmap of vertices of m2 (non-inclusive)
	for (int x = 1; x < 63; x++) {
		for (int z = 1; z < 63; z++) {
			// Gather the heights of the four adjacent vertices
			float hL = m2[x - 1][z];
			float hR = m2[x + 1][z];
			float hD = m2[x][z + 1];
			float hU = m2[x][z - 1];

			// Approximate the current vertex normal
			normalsM2[x][z][0] = hL - hR;
			normalsM2[x][z][1] = hD - hU;
			normalsM2[x][z][2] = 2.0;
		}
	}

	// Iterate over the heightmap of vertices of m2 (non-inclusive)
	for (int x = 1; x < 63; x++) {
		for (int z = 1; z < 63; z++) {
			// Gather the heights of the four adjacent vertices
			float hL = m3[x - 1][z];
			float hR = m3[x + 1][z];
			float hD = m3[x][z + 1];
			float hU = m3[x][z - 1];

			// Approximate the current vertex normal
			normalsM3[x][z][0] = hL - hR;
			normalsM3[x][z][1] = hD - hU;
			normalsM3[x][z][2] = 2.0;
		}
	}
}

/************************************************************************
	Function:		initializeMountains

	Description:	Initialize mountain heightarrays
*************************************************************************/
void initializeMountains() {

	// Iterate over all of the height maps and initialize to 0
	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			m1[i][j] = 0;
			m2[i][j] = 0;
			m2[i][j] = 0;
		}
	}

	// Pass the height maps to the generate mountain function
	generateMountain(&m1);
	generateMountain(&m2);
	generateMountain(&m3);

	// Call the function to calculate the mountain normals
	calcMountainNormals();
}

/************************************************************************
	Function:		placeMountain

	Description:	Places mountain in the scene
*************************************************************************/
void placeMountain(float scale[3], float translate[3], float normals[64][64][3], float vertices[64][64]) {

	// Push the current transformation matrix
	glPushMatrix();
		// Scale and translate
		glScalef(scale[0], scale[1], scale[2]);
		glTranslatef(translate[0], translate[1], translate[2]);

		// Ensure lighting is enabled and set materials 
		glEnable(GL_LIGHTING);
		float diffuse[3] = {0.6, 0.6, 0.6};
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuse);
		glMaterialf(GL_FRONT, GL_SPECULAR, 50.0);

		// If the mountains are in texture mode
		if (mountainTextureMode) {
			// Enable and bind mountain texture
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, mountainTexture);
			// Begin drawing quads
			glBegin(GL_QUADS);

				// Iterate over the vertices
				for (int x = 1; x < 63; x++) {
					for (int z = 1; z < 63; z++) {

						// Set color to white for the texture, set normal, set texture coord, set vertex
						glColor3f(1.0, 1.0, 1.0);
						glNormal3fv(normals[x][z]);
						glTexCoord2f((float)x/64, (float)z/64);
						glVertex3f(x, vertices[x][z], z);

						// Set color to white for the texture, set normal, set texture coord, set vertex
						glNormal3fv(normals[x][z + 1]);
						glTexCoord2f((float)x/64, ((float)z + 1)/64);
						glVertex3f(x, vertices[x][z + 1], z + 1);

						// Set color to white for the texture, set normal, set texture coord, set vertex
						glNormal3fv(normals[x + 1][z + 1]);
						glTexCoord2f(((float)x + 1)/64, ((float)z + 1)/64);
						glVertex3f(x + 1, vertices[x + 1][z + 1], z + 1);

						// Set color to white for the texture, set normal, set texture coord, set vertex
						glNormal3fv(normals[x + 1][z]);
						glTexCoord2f(((float)x + 1)/64, (float)z/64);
						glVertex3f(x + 1, vertices[x + 1][z], z);
					}
				}
			glEnd();
			glDisable(GL_TEXTURE_2D);

		}
		// Texture mode is not enabled
		else {
			//Begin drawing quads
			glBegin(GL_QUADS); 
			// Iterate over height map vertices
			for (int x = 1; x < 63; x++) {
				for (int z = 1; z < 63; z++) {

					// Set k for a (bad) height RGB function
					float k = 0;

					// set k for current vertex, set color, set normal, then draw
					k = vertices[x][z] / HEIGHT_MAX * 1.2;
					glColor3f(k*1.0, 1.0, k*1.0);
					glNormal3fv(normals[x][z]);
					glVertex3f(x, vertices[x][z], z);

					// Same as above
					k = vertices[x][z + 1] / HEIGHT_MAX * 1.2;
					glColor3f(k * 1.0, 1.0, k * 1.0);
					glNormal3fv(normals[x][z + 1]);
					glVertex3f(x, vertices[x][z + 1], z + 1);
					
					// Same as above
					k = vertices[x + 1][z + 1]/ HEIGHT_MAX * 1.2;
					glColor3f(k * 1.0, 1.0, k * 1.0);
					glNormal3fv(normals[x + 1][z + 1]);
					glVertex3f(x + 1, vertices[x + 1][z + 1], z + 1);

					// Same as above
					k = vertices[x + 1][z]/ HEIGHT_MAX * 1.2;
					glColor3f(k * 1.0, 1.0, k * 1.0);
					glNormal3fv(normals[x + 1][z]);
					glVertex3f(x + 1, vertices[x + 1][z], z);
				}
			}
			glEnd();
		}
			
	glPopMatrix();
}

/************************************************************************
	Function:		drawMountains

	Description:	Draw the initialized mountains
*************************************************************************/
void drawMountains() {

	// Set three max variablese to determine where to place the mountains on the y-axis
	float max1 = 0;
	float max2 = 0;
	float max3 = 0;
	// Iterate over the edges of the height map and find the highest point
	for (int x = 0; x < 64; x++) {
		if (m1[x][0] > max1) { max1 = m1[x][0]; }
		if (m1[x][63] > max1) { max1 = m1[x][63]; }
		
		if (m2[x][0] > max2) { max2 = m2[x][0]; }
		if (m2[x][63] > max2) { max2 = m2[x][63]; }
		
		if (m3[x][0] > max3) { max3 = m3[x][0]; }
		if (m3[x][63] > max3) { max3 = m3[x][63]; }
	}

	for (int z = 0; z < 64; z++) {
		if (m1[0][z] > max1) { max1 = m1[0][z]; }
		if (m1[63][z] > max1) { max1 = m1[63][z]; }

		if (m2[0][z] > max2) { max2 = m2[0][z]; }
		if (m2[63][z] > max2) { max2 = m2[63][z]; }

		if (m3[0][z] > max3) { max3 = m3[0][z]; }
		if (m3[63][z] > max3) { max3 = m3[63][z]; }
	}

	// Set transformation matrices and pass to placeMountain function
	float scale1[3] = { 0.1, 0.5, 0.2 };
	float translate1[3] = { -50, -max1, -80 };
	placeMountain(scale1, translate1, &normalsM1, &m1);

	float scale2[3] = { 0.7, 0.3, 0.2 };
	float translate2[3] = { 50, -max2, -70 };
	placeMountain(scale2, translate2, &normalsM2, &m2);

	float scale3[3] = { 0.3, 0.3, 0.3 };
	float translate3[3] = { 50, -max3, 70 };
	placeMountain(scale3, translate3, &normalsM3, &m3);
	

	glDisable(GL_TEXTURE_2D);

}

/************************************************************************
	Function:		drawCessna

	Description:	Draws the cessna plane
*************************************************************************/
void drawCessna() {
	// Disable color material and draw
	glDisable(GL_COLOR_MATERIAL);
	glCallList(cessnaID);
	glEnable(GL_COLOR_MATERIAL);
}

/************************************************************************
	Function:		drawPropellers

	Description:	Draws and rotates the propellers
*************************************************************************/
void drawPropellers() {
	glDisable(GL_COLOR_MATERIAL);
	glMatrixMode(GL_MODELVIEW);

	// Push cuurent transformation matrix
	glPushMatrix();
		// Translate and rotate propeller
		glTranslatef(-0.01, -0.14, 0.35);
		glRotatef(propRotate, 1, 0, 0);
		glTranslatef(0.01, 0.14, -0.35);
		// Call cached display list to draw propeller
		glCallList(propellerID);
	glPopMatrix();

	glPushMatrix();
		// Translate and rotate propeller
		glTranslatef(0.0, 0.0, -0.7);
		glTranslatef(-0.01, -0.14, 0.35);
		glRotatef(propRotate, 1, 0, 0);
		glTranslatef(0.01, 0.14, -0.35);
		// Call cached display list to draw propeller
		glCallList(propellerID);
	glPopMatrix();
	
	glEnable(GL_COLOR_MATERIAL);
}



/************************************************************************
	Function:		drawFog

	Description:	Draws fog in the distance
*************************************************************************/
void drawFog() {
	// Enable fog and set parameters
	glEnable(GL_FOG);
	GLfloat fogColor[4] = { 0.976, 0.545, 0.886, 0.1 };
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_MODE, GL_EXP);
	glFogf(GL_FOG_DENSITY, 0.005);
}

/************************************************************************
	Function:		loadTextures

	Library Source:	https://github.com/nothings/stb

	Description:	sets texture ID's from bitmap files
*************************************************************************/
void loadTextures() {

	// Generate new quadrics for disk and cylinder
	diskQuad = gluNewQuadric();
	cylinderQuad = gluNewQuadric();

	glEnable(GL_TEXTURE_2D);
	int nrChannels;
	
	// Set Sky texture properties
	glGenTextures(1, &skyTexture);
	glBindTexture(GL_TEXTURE_2D, skyTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Load sky .bmp using stb library
	skyTexPixels = stbi_load("sky08.bmp", &skyTexWidth, &skyTexHeight, &nrChannels, 0);

	// If it worked, generate a mipmap
	if (skyTexPixels) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, skyTexWidth, skyTexHeight, GL_RGB, GL_UNSIGNED_BYTE, skyTexPixels);
	}
	// Free the image pixels
	stbi_image_free(skyTexPixels);

	// Set Sea texture properties
	glGenTextures(1, &seaTexture);
	glBindTexture(GL_TEXTURE_2D, seaTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Load sea .bmp using stb library
	seaTexPixels = stbi_load("sea02.bmp", &seaTexWidth, &seaTexHeight, &nrChannels, 0);

	// If it worked, generate a mipmap
	if (seaTexPixels) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, seaTexWidth, seaTexHeight, GL_RGB, GL_UNSIGNED_BYTE, seaTexPixels);

	}
	// Free the image pixels
	stbi_image_free(seaTexPixels);


	// Set mountain texture properties
	glGenTextures(1, &mountainTexture);
	glBindTexture(GL_TEXTURE_2D, mountainTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Load sea .bmp using stb library
	mountainTexPixels = stbi_load("mount03.bmp", &mountainTexWidth, &mountainTexHeight, &nrChannels, 0);

	// If it worked, generate a mipmap
	if (mountainTexPixels) {
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, mountainTexWidth, mountainTexHeight, GL_RGB, GL_UNSIGNED_BYTE, mountainTexPixels);
	}

	// Free the image pixels
	stbi_image_free(mountainTexPixels);

	
	glDisable(GL_TEXTURE_2D);



}


/************************************************************************
	Function:		drawSky

	Description:	Draws the sky texture and object
*************************************************************************/
void drawSky() {
	// Set emission material
	GLfloat emission[] = { 1, 1, 1, 0.8 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
	glPushMatrix();
		// Set color to white for texture multiplcation
		glColor4f(1.0, 1.0, 1.0, 1.0);
		// Transalte and rotate to correct location
		glTranslatef(0.0, 149.0, 0.0);
		glRotatef(90, 1, 0, 0);
		// Draw cylinder
		gluCylinder(cylinderQuad, 175, 175, 150, 60, 20);
	glPopMatrix();
	// Set current emission to none
	GLfloat noEmission[] = { 0.0, 0.0, 0.0, 0.0 };
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, noEmission);
}

/************************************************************************
	Function:		drawSea

	Description:	Draws the sea texture and object
*************************************************************************/
void drawSea() {
	glPushMatrix();
		// Set color to white for texture 
		glColor4f(1.0, 1.0, 1.0, 1.0);
		// rotate and draw
		glRotatef(270, 1, 0, 0);
		gluDisk(diskQuad, 0, 176, 30, 20);
	glPopMatrix();
}



/************************************************************************
	Function:		drawGrid

	Description:	Draws the grid
*************************************************************************/
void drawGrid() {
	glEnable(GL_COLOR_MATERIAL);
	glBegin(GL_QUADS);
	// Set color
	glColor3f(0.415, 0.325, 0.674);
	// Iterate over grid size and draw the quads
	for (int x = -GRID_SIZE; x < GRID_SIZE; x++) {
		for (int z = GRID_SIZE; z > -GRID_SIZE; z--) {

			glNormal3f(0, 1, 0);
			glVertex3f(x, 0, z);
			glVertex3f(x, 0, z - 1);
			glVertex3f(x + 1, 0, z - 1);
			glVertex3f(x + 1, 0, z);
		}
	}
	glEnd();
	glDisable(GL_COLOR_MATERIAL);

}




/************************************************************************
	Function:		display

	Description:	Main looping display function
*************************************************************************/
void display(void)
{
	/* display callback, clear frame buffer and z buffer */

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset transformations and set modes
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// set the camera position
	gluLookAt(camPos[0], camPos[1], camPos[2],
			camPos[0] + lookatPosition[0], camPos[1] + lookatPosition[1], camPos[2] + lookatPosition[2],
			0.0, 1.0, 0.0);

	// Set position of light
	GLfloat lightPosition[] = {0.0, 50.0, 0.0, 0.0};
	GLfloat lightDirection[] = { -1.0, -0.5, 2.0, 0.0 };
	GLfloat lightIntensity[] = { 0.1, 0.1, 0.1, 0.8 };

	// Set light properties
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	// If the scene is in grid view
	if (gridView) {
		
		//Draw grid
		drawGrid();

		// Draw little white sphere and XYZ lines at origin
		glDisable(GL_LIGHTING);
		glColor3f(1.0, 1.0, 1.0);
		glutSolidSphere(0.05, 50, 20);
		glLineWidth(5.0);
		glBegin(GL_LINES);

		// Draw X line
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0, 0, 0);
		glVertex3f(1.0, 0.0, 0.0);
		// Draw Y line
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0, 0, 0);
		glVertex3f(0.0, 1.0, 0.0);
		// Draw Z line
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0, 0, 0);
		glVertex3f(0.0, 0.0, 1.0);
		glEnd();
		glEnable(GL_LIGHTING);

		glLineWidth(1.0);

	}
	// Otherwise it is in sea/sky view
	else {
		glEnable(GL_TEXTURE_2D);

		// Set quadric texture properties for disk and cylinder
		gluQuadricTexture(diskQuad, GLU_TRUE);
		gluQuadricTexture(cylinderQuad, GLU_TRUE);
		gluQuadricNormals(diskQuad, GLU_SMOOTH);
		gluQuadricNormals(cylinderQuad, GLU_SMOOTH);

		// Bind sea texture
		glBindTexture(GL_TEXTURE_2D, seaTexture);
		// If fog is enabled, draw it
		if (fogToggle) {
			drawFog();
		}
		// Draw the sea
		drawSea();
		// Disable fog
		glDisable(GL_FOG);

		// Bind sky texture and draw
		glBindTexture(GL_TEXTURE_2D, skyTexture);
		drawSky();

		glDisable(GL_TEXTURE_2D);
	}
	
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	// Draw the mountains
	drawMountains();
	glDisable(GL_COLOR_MATERIAL);


	// Push current matrix to the stack
	glPushMatrix();
		// Translate to the camera
		glTranslatef(camPos[0], camPos[1], camPos[2]);
		// Rotate to point where camera is looking
		glRotatef(-1 * cameraAngle * (180 / PI), 0, 1, 0);

		// Offset to draw the plane
		glTranslatef(0.00, -0.75, -3.50);

		// Handle rotation once plane is in place
		glRotatef(-1 * planeRoll, 0, 0, 1);
		glRotatef(-90, 0, 1, 0);
		
		// Draw the plane
		if (stealthMode) {
			glEnable(GL_BLEND);
		}
		drawPropellers();
		drawCessna();
		glDisable(GL_BLEND);
	// Pop the matrix from the stack
	glPopMatrix();

	
	

	glFlush();
	glutSwapBuffers();
}

/************************************************************************
	Function:		specialKeys

	Description:	Callback function for dealing with special keypresses
*************************************************************************/
void specialKeys(unsigned char key, int x, int y) {

	// If any of our special keys are pressed, set the flags to true

	if (key == GLUT_KEY_PAGE_UP) {
		pageUp = 1;
	}

	if (key == GLUT_KEY_PAGE_DOWN) {
		pageDown = 1;
	}

	if (key == GLUT_KEY_UP) {
		upArrow = 1;
	}

	if (key == GLUT_KEY_DOWN) {
		downArrow = 1;
	}

	if (rollingRight == 0 && rollingLeft == 0 && key == GLUT_KEY_LEFT) {
		rollingLeft = 1;
	}

	if (rollingRight == 0 && rollingLeft == 0 && key == GLUT_KEY_RIGHT) {
		rollingRight = 1;
	}
}

/************************************************************************
	Function:		specialKeys

	Description:	Callback function for dealing with special keypresses
*************************************************************************/
void specialKeysUp(unsigned char key, int x, int y) {

	// If any of our special keys are released, set the flags to false

	if (key == GLUT_KEY_PAGE_UP) {
		pageUp = 0;
	}

	if (key == GLUT_KEY_PAGE_DOWN) {
		pageDown = 0;
	}

	if (key == GLUT_KEY_UP) {
		upArrow = 0;
	}

	if (key == GLUT_KEY_DOWN) {
		downArrow = 0;
	}
}

/************************************************************************
	Function:		mousePos

	Description:	Gets mouse position
*************************************************************************/
void mousePos(int x, int y) {
	mouseX = x;
	mouseY = y;
}
/************************************************************************
	Function:		idle

	Description:	Idle function
*************************************************************************/
void idle(void)
{

	// Increase propeller rotation
	if (propRotate >= 360) {
		propRotate = 0;
	}
	propRotate += 360*speed;

	// Determine mouse distance from center X
	float distanceFromCenterX = mouseX - windowWidth / 2.0f;
	
	// Calculate camera angle
	cameraAngle += distanceFromCenterX / windowWidth * 0.025;

	// Calculate where the camera should be looking
	lookatPosition[0] = 1.2*sin(cameraAngle);
	lookatPosition[2] = 1.2*-cos(cameraAngle);
	
	// If the right arrow key has been pressed, do a barrel roll to the right
	if (rollingRight) {
		if (planeRoll >= 360) {
			planeRoll = 0;
			rollingRight = 0;
		}
		planeRoll += 5;
	}
	// If the left arrow key has been pressed, do a barrel roll to the right
	else if (rollingLeft) {
		if (planeRoll <= -360) {
			planeRoll = 0;
			rollingLeft = 0;
		}
		planeRoll -= 5;
	}
	// Otherwise, use the mouse to control the planes roll
	else {
		if (speed <= 0) {
			planeRoll = 0;
		}
		else {
			planeRoll = distanceFromCenterX / 15;
		}
	}
	
	

	// Always moving forward
	camPos[0] += lookatPosition[0] * speed;
	camPos[2] += lookatPosition[2] * speed;

	// If the speed is 0, the plane should drop
	
		
	// Camera position
	// If PAGE_UP is currently pressed, change the campera position accordingly
	if (pageUp) {
		if (speed < 0.2) {
			speed += 0.0005;
		}
	}
	// If PAGE_DOWN is currently pressed, change the campera position accordingly
	if (pageDown) {
		if (speed > 0.0) {
			speed -= 0.0005;
		}
	}
	if (speed <= 0) {
		if (camPos[1] > 0.8) {
			camPos[1] *= 0.95;
		}
		
	}
	// If UP key is currently presed, increase the y-value of the camera position
	if (upArrow) {
		camPos[1] += speed / 3;

	}
	// If DOWN key is currently presed, decrease the y-value of the camera position
	if (downArrow) {
		camPos[1] -= speed / 3;
	}

	glutPostRedisplay();

}

/************************************************************************
	Function:		keys

	Description:	Callback function for dealing with regular keypresses
*************************************************************************/
void keys(unsigned char key, int x, int y) {

	// Toggle mountain texture
	if (key == 't') {
		if (mountainTextureMode) {
			mountainTextureMode = 0;
		}
		else {
			mountainTextureMode = 1;
		}
	}

	// Toggle fog
	if (key == 'b') {
		if (fogToggle) {
			fogToggle = 0;
		}
		else {
			fogToggle = 1;
		}
	}

	// Wireframe
	if (key == 'w') {
		if (drawWireframe) { 
			drawWireframe = 0;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else { 
			drawWireframe = 1; 
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
	}

	// Toggle grid View 
	if (key == 's') {
		if (gridView) {
			gridView = 0;
		}
		else {
			gridView = 1;
		}
	}

	// Toggle stealth mode
	if (key == 'a') {
		if (stealthMode) {
			stealthMode = 0;
		}
		else {
			stealthMode = 1;
		}
	}

	// Quit from application
	if (key == 'q') {
		exit(1);
	}

	// Fullscreen
	if (key == 'f') {
		glutFullScreenToggle();
	}


	// Redraw the scene
	glutSwapBuffers();
	glutPostRedisplay();
}

/************************************************************************

	Function:		reshape

	Description:	Changes the OpenGL aspect ratio to match a GLUT
					window resize.

*************************************************************************/
void reshape(int newWidth, int newHeight)
{
	// update the viewport to still be all of the window
	glViewport(0, 0, (GLsizei)newWidth, (GLsizei)newHeight);

	// enter GL_PROJECTION mode so that we can change the 2D coordinates 
	glMatrixMode(GL_PROJECTION);

	// clear the projection matrix
	glLoadIdentity();

	// Set camera perspective
	gluPerspective(60, (GLfloat)newWidth / (GLfloat)newHeight, 1.0, 1000);   

	// set modelview matrix
	glMatrixMode(GL_MODELVIEW);
}

/****************************************************************************

	Function:		readCessna

	Description:	reads in Cessna from cessna.txt

*****************************************************************************/
void readCessna() {

	// Allocate for NV array to store point data
	NV* data = malloc(sizeof(NV) * 6764);

	// Counter variables
	char name[100];
	char c;
	int vertexCount = 0;
	int normalCount = 0;
	int objectCount = -1;


	// Store the display list id
	cessnaID = glGenLists(1);
	glNewList(cessnaID, GL_COMPILE);

	// Open cessna.txt
	FILE* file = fopen("cessna.txt", "r");

	if (file != NULL) {

		char line[500];

		// Get lines from the file until there are no more lines
		while (fgets(line, 500, file) != NULL) {

			// If the line matches the vertex format, store vertices and increment vertex count
			if (sscanf(line, "v %f %f %f", &data[vertexCount].vertX, &data[vertexCount].vertY, &data[vertexCount].vertZ) == 3) {
				vertexCount++;
			}
			// If the line matches the normal format, store normals and increment normal count
			else if (sscanf(line, "n %f %f %f", &data[normalCount].normX, &data[normalCount].normY, &data[normalCount].normZ) == 3) {
				normalCount++;
			}
			// If the line matches the sub-object format, increment object count
			else if (sscanf(line, "g %s", name) != 0) {
				objectCount++;

			}
			// If the line matches the face format...
			else if (sscanf(line, "%c ", &c) != 0 && c == 'f') {

				// Set material color based on sub-object number
				if (objectCount <= 3 || (objectCount >= 8 && objectCount <= 13) || (objectCount >= 26 && objectCount <= 32)) {
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow);
				}
				else if (objectCount >= 4 && objectCount <= 5) {
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, black);
				}
				else if (objectCount == 6) {
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lightPurple);
				}
				else if (objectCount == 7 || (objectCount >= 14 && objectCount <= 25)) {
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
				}

				// Set specular material and shininess
				glMaterialf(GL_FRONT, GL_SPECULAR, 1.0);
				glMaterialf(GL_FRONT, GL_SHININESS, 100.0);

				// variables for tokenizing a line of unknown length
				char* token;
				int faceNumber;
				token = strtok(line, " ");

				// We draw the face as it's read in to be stored in display list
				glBegin(GL_POLYGON);

				// While the token still has a value
				while (token != NULL) {
					// Convert face number to an integer and subtract one (dealing with indices starting at 0)
					faceNumber = atoi(token) - 1;

					if (faceNumber >= 0) {
						// Set normal and draw vertex
						glNormal3f(data[faceNumber].normX, data[faceNumber].normY, data[faceNumber].normZ);
						glVertex3f(data[faceNumber].vertX, data[faceNumber].vertY, data[faceNumber].vertZ);
					}

					// tokenize
					token = strtok(NULL, " ");
				}
				glEnd();
			}
		}
		// Cose the file
		fclose(file);
	}
	else {
		// print an error
		perror("cessna.txt");
	}


	glEndList();
	// Free the dynamically allocated data 
	free(data);

}

/****************************************************************************

	Function:		readPropeller

	Description:	reads in propeller model from propeller.txt

*****************************************************************************/
void readPropeller() {

	// Allocate for NV array to store point data
	NV* data = malloc(sizeof(NV) * 6764);

	// Counter variables
	char name[100];
	char c;
	int vertexCount = 0;
	int normalCount = 0;
	int objectCount = -1;

	/****************************************
	* Same Process as reading in the Cessna *
	*****************************************/

	// Store the display list id
	propellerID = glGenLists(1);
	glNewList(propellerID, GL_COMPILE);

	FILE* file = fopen("propeller.txt", "r");

	if (file != NULL) {

		char line[500];

		while (fgets(line, 500, file) != NULL) {

			if (sscanf(line, "v %f %f %f", &data[vertexCount].vertX, &data[vertexCount].vertY, &data[vertexCount].vertZ) == 3) {
				vertexCount++;
			}
			else if (sscanf(line, "n %f %f %f", &data[normalCount].normX, &data[normalCount].normY, &data[normalCount].normZ) == 3) {
				normalCount++;
			}
			else if (sscanf(line, "g %s", name) != 0) {
				objectCount++;

			}
			else if (sscanf(line, "%c ", &c) != 0 && c == 'f') {

				// Set material color based on sub-object
				if (objectCount == 0) {
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, grey);
				}
				else {
					glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
				}

				glMaterialf(GL_FRONT, GL_SPECULAR, 1.0);
				glMaterialf(GL_FRONT, GL_SHININESS, 1.0);

				char* token;
				int faceNumber;
				token = strtok(line, " ");

				glBegin(GL_POLYGON);

				while (token != NULL) {
					faceNumber = atoi(token) - 1;

					if (faceNumber >= 0) {

						glNormal3f(data[faceNumber].normX, data[faceNumber].normY, data[faceNumber].normZ);
						glVertex3f(data[faceNumber].vertX, data[faceNumber].vertY, data[faceNumber].vertZ);
					}

					token = strtok(NULL, " ");
				}
				glEnd();
			}
		}
		fclose(file);
	}
	else {
		perror("propeller.txt"); //print the error message on stderr.
	}

	glEndList();
	free(data);

}

/************************************************************************

	Function:		initializeGL

	Description:	Initializes the necessary properties

*************************************************************************/
void initializeGL(void)
{

	// enable depth testing and lighting
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);

	// Enable color materials (will be disabled when necessary)
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// set background color to be black
	glClearColor(0, 0, 0, 1.0);

	// change into projection mode so that we can change the camera properties
	glMatrixMode(GL_PROJECTION);

	// load the identity matrix into the projection matrix
	glLoadIdentity();

	// gluPerspective(fovy, aspect, near, far)
	gluPerspective(45, (float)windowWidth / (float)windowHeight, 1, 500);

	// change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);

	

}


void main(int argc, char** argv)
{
	// Print keyboard controls to console
	printf("Scene Controls\n------------------\n");
	printf("t  :   Toggle mountain textures\n");
	printf("w  :   Toggle wireframe mode\n");
	printf("s  :   Toggle between grid and sea/sky\n");
	printf("a  :   Toggle stealth mode\n");
	printf("f  :   Toggle fullscreen mode\n");
	printf("q  :   Quit\n");
	printf("b  :   Toggle fog\n\n");
	printf("Plane Controls\n------------------\n");
	printf("Mouse Position:   Turn the plane.\n");
	printf("   Up Arrow   :   Move up\n");
	printf(" Down Arrow   :   Move down\n");
	printf(" PAGE UP      :   Increase Speed\n");
	printf(" PAGE DOWN    :   Decrease Speed\n");






	// Initialize the toolkit
	glutInit(&argc, argv);
	// Set display mode
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	// Set window size
	glutInitWindowSize(windowWidth, windowHeight);
	// Set window position on screen
	glutInitWindowPosition(250, 150);
	// Open the screen window
	glutCreateWindow("Plane Sim");
	// Register redraw function
	glutDisplayFunc(display);
	// Register window reshape function
	glutReshapeFunc(reshape);
	// Register special function for keypresses
	glutSpecialFunc(specialKeys);
	// Register function for when special keys are released
	glutSpecialUpFunc(specialKeysUp);
	// Register keyboard function
	glutKeyboardFunc(keys);
	// Register idle function
	glutIdleFunc(idle);
	// Register mouse function
	glutPassiveMotionFunc(mousePos);
	// Initialize the rendering context
	initializeGL();
	// Initialize randomizer 
	srand(time(NULL));
	// Read in objects
	readCessna();
	readPropeller();
	// Load textures
	loadTextures();
	// Initialize Mountains
	initializeMountains();
	// Go into a perpetual loop
	glutMainLoop();
}