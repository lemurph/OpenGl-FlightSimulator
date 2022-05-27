AUTHOR: Logan Murphy, B00835378

This is a readme file for my submission of the final project of CSCI 3161.

SOURCES/LIBRARIES:
	- https://learn.64bitdragon.com/articles/computer-science/procedural-generation/the-diamond-square-algorithm
	- https://github.com/c-utils/dque
	- https://stackoverflow.com/questions/13983189/opengl-how-to-calculate-normals-in-a-terrain-height-grid
	- https://github.com/nothings/stb

To run this program, simply run "Final Project.exe" with all of the dependencies in the same directory.

The controls are as follow:

	Scene Controls
		t  :   Toggle mountain textures
		w  :   Toggle wireframe mode
		s  :   Toggle between grid and sea/sky
		a  :   Toggle stealth mode
		f  :   Toggle fullscreen mode
		q  :   Quit
		b  :   Toggle fog

	Plane Controls
		Mouse Position:   Turn the plane.
		Up Arrow      :   Move up
		Down Arrow    :   Move down
		PAGE UP       :   Increase Speed
		PAGE DOWN     :   Decrease Speed


My "Additional Enhancements":
	1. Pressing the 'a' key toggles stealth mode for the plane, making it transparent
	2. Pressing the left arrow key performs a barrel roll to the left; 
		the right arrow key performs a barrel roll to the right.
	3. Reducing the speed to 0 will cause the plane to fall out of the sky and into the sea.
		 increasing the speed and holding the up arrow allows the plane to recover.