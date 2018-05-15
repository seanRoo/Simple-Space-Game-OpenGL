// OpenGL_Game.cpp

#include "stdafx.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>    // Required for sound only (consider OpenAL), 
#include <mmsystem.h>   // Again only required for sound only

#include <freeglut.h>   // GLUT OpenGL Utility Toolkit
#include "Sprite.h"     // Prototype Sprite class

// 3 Sprites are used in the "game"
Sprite player;
Sprite ball;
Sprite ball2;
bool ball1_moving=true;
bool ball2_moving=false;
bool stay_position=true;
bool restart=false;
bool youWin=false;
bool movable = false;
Sprite background;
Sprite catapult;

// Score data and method
int lives=5;
int difficultyCounter=0;
char score_text[20];
char game_over[20];
char you_win[20];
char restart_message[20];


// Callback function called each time screen is to be redrawn (called by OS not you directly)
static void display(void)                                 
{															
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f) ;
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		background.drawSprite();							
		player.drawSprite();									
		ball.drawSprite();		
		ball2.drawSprite();
	    
		sprintf(score_text,"LIVES: %d",lives);               
		Sprite::text(830,420,score_text);

		if(lives==0){ //if player has no lives left tell them, offer opportunity to restart

		Sprite::text(465, 420, game_over);
		sprintf(game_over, "GAME OVER");
		Sprite::text(450, 380, restart_message);
		sprintf(restart_message, "PRESS S TO RESTART");
		}

		if(youWin){ //if user wins tell them, restart option also

			Sprite::text(450, 210, you_win);
			sprintf(you_win, "YOU WIN");
			Sprite::text(420, 180, restart_message);
			sprintf(restart_message, "PRESS S TO RESTART");
		}

		player.advance_frame(0,2); 

		glutSwapBuffers();  // Display buffer just drawn and then switch to draw on the other buffer
}


// Callback function to allow dynamic resizing of runtime display
static void resize(int width, int height)
{
    glViewport(0, 0, width, height);
	Sprite::spriteModeOn(0,0);  // (0,0) Causes orginal screen size to be reused
}

// Callback to manage standard keys on the keyboard
// In this case 'q' or 'Q' causes program to terminate
static void qwerty_keys(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'Q':
    case 'q': glutLeaveMainLoop () ; break;
	case 's': restart=true;// (Stop!)
			
    default: break;
    }
}

// Callback for cursor keys on the keyboard
// <- and -> cursors keys cause player to move left and right
static void function_keys(int key, int x, int y)
{
	if(movable){//only if game is in seesion, otherwise false

    switch (key)
    {

    case GLUT_KEY_LEFT:
			player.advance_frame(3,3);
			player.sprite_x_position-=20;
			player.direction=true;
		break;


    case GLUT_KEY_RIGHT: 
		    player.advance_frame(4,4);
			player.sprite_x_position+=20;
			player.direction=true;
		break;
	
    case GLUT_KEY_UP: 
		    player.advance_frame(0,2);
			player.sprite_y_position+=20;
		break;

	case GLUT_KEY_DOWN:
			player.advance_frame(0,2);
			player.sprite_y_position-=20;
		break;
		

    default: break;
    }
	}
}

// Game loop
// static void idle()    // if using  glutIdleFunc(idle); this line should replace the one below
static void idle(int v)  // Runs between frame updates in doublebuffer mode, game logic goes here
{	
	while(ball1_moving)
	{

	  movable=true; //ship can move
	  ball.advance_frame(0,3); 
	  ball.sprite_x_position-=7;

	  if(ball.sprite_x_position<0)   //if the asteroid has travelled the whole length of the screen
	  { 

		  difficultyCounter++; //increment the difficulty counter that will eventually introduce a second asteroid

		  //random horizontal positions for incoming asteroids
		  ball.sprite_y_position=50+(rand()%200);
		  //asteroid starts offscreen
		   ball.sprite_x_position=1500;
		   //random velocities for asteroids
		   ball.sprite_x_velocity=50+(rand()%150);

	  }
	  //boolean that introduces the second ball
	  if(difficultyCounter>=4){
			
		  ball2_moving = true;

	  }

	  if(player.sprite_x_position>1024){//if the player has made it all the way

		  youWin=true; //they win
		  movable=false; //cant move ship anymore
	  }

	  break;
	}
	
	while(ball2_moving){
	  difficultyCounter=0;
	  ball2.advance_frame(0,3); 
	  ball2.sprite_x_position-=7;

	  if(ball2.sprite_x_position<0)   
	  { 
		  
		  ball2.sprite_y_position=50+(rand()%200);
		  ball2.sprite_x_position=2000;
		  ball2.sprite_x_velocity=50+(rand()%150);
	  }

	  break;
	}
	if(!ball2_moving){
		ball2.sprite_x_position=1500;
	}

	if(lives==0 || youWin){//if the game has ended

		ball1_moving=false; //stop the asteroids
		ball2_moving=false;
		movable=false; //cant move ship

		if(restart==true){ //if the user has pressed 's' for restart
			//restart
			player.sprite_x_position=40;
			player.sprite_y_position=100;
			//reset
			lives=5;
			difficultyCounter=0;
			ball1_moving=true;
			ball2_moving=false;
			restart=false;
			youWin=false;

		}
	}
	

	if(player.collision(ball)){//if player collides with asteroid
			//launch new asteroid
		   ball.sprite_y_position=50+(rand()%200);
		   ball.sprite_x_position=1000;
		   ball.sprite_x_velocity=50+(rand()%150);
		lives--; //decrement their lives
	}

	else if(player.collision(ball2)){//same as ball 1
		   ball2.sprite_y_position=50+(rand()%200);
		   ball2.sprite_x_position=1000;
		   ball2.sprite_x_velocity=50+(rand()%150);
		lives--;
	}

    glutPostRedisplay();      // Send message to ask for screen to be redrawn
	glutTimerFunc(10,idle,0); // Restart the timer (remove line if using the more common glutIdleFunc(idle) approach)
}

/* Program entry point - starts here */
int main(int argc, char *argv[])
{
	Sprite::spriteModeOn(1024,480);		// Create a screen 640x480 to contain game

	// Start playing background music (uses Windows calls, there are other ways incluing OpenAL)
	//PlaySound(L"Music.wav",NULL, SND_LOOP| SND_ASYNC);  

	// load sprite files (24bit BMP), Frames (X,Y), position (X,Y), size (Width,Height), direction
	// Use sprites with Width and Height divisible by 4, ideally powers of two e.g. 256x128
	player.load("mySpritesheet.bmp",5,1,40,100,100,100,true);  
	ball.load("myAsteroid_sheet.bmp",4,1,1024,35,50,50,true); 
	ball2.load("myAsteroid_sheet.bmp",4,1,1500,35,50,50,true); 
	background.load("myBackground.bmp",1,1,0,0,1024,480,true); 
	
    glutReshapeFunc(resize);			// Called when window is resized (not required)
	glutDisplayFunc(display);			// Callback function to redraw, an event created by glutPostRedisplay()
    glutKeyboardFunc(qwerty_keys);		// QWERTY Keys
    glutSpecialFunc(function_keys);		// Cursor keys etc 

	//glutIdleFunc(idle);			    // Called during idle time 
	glutTimerFunc(10,idle,0);           // Use timer callback for more consistent timing
	
	glutMainLoop();						// Start main loop running (go!)
    
	return EXIT_SUCCESS;
}

