#include <Cocoa/Cocoa.h>
#include <stdlib.h>
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#include <QTKit/QTMovie.h>
#include "agk.h"
#include "interpreter.h"

using namespace AGK;

// declare a mustic event listener
@interface MusicListener: NSObject {
	
}

-(id) init;
-(void) musicDidFinish:(NSNotification *)notification;
@end

// define the music event listener
@implementation MusicListener

-(id) init
{
	if ( self = [super init] )
	{
		// register to receive QTMovie finish callback
		[[NSNotificationCenter defaultCenter] addObserver:self
		 		 selector:@selector(musicDidFinish:)
		 		 name:QTMovieDidEndNotification object:nil];
	}
	return self;
}

// the function that will be called by the callback
-(void) musicDidFinish:(NSNotification *)notification
{
	agk::HandleMusicEvents(NULL);
}
@end

// general variables
bool bDrawing = false;
MusicListener *g_musicListener;

void display(void)
{
	/*
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	static float y = 100;
	static float vy = -1.6f;
	y += vy;
	if ( y <= 0 ) vy = 1.6f;
	if ( y >= 100 ) vy = -1.6f;
	
	glDepthFunc(GL_LEQUAL);
	
	glDisable(GL_TEXTURE_2D);
	glColor3f(1,0,0);
	float z = -2.0f;
	glBegin(GL_TRIANGLES);
    	glVertex3f(50, 50,  z);
	    glVertex3f(100, 100,z);
		glVertex3f(100, 50, z);
	glEnd();
	glColor3f(0,1,0);
	z = 0.0f;
	glBegin(GL_TRIANGLES);
		glVertex3f(100, 0,  z);
		glVertex3f(100, 100,z);
		glVertex3f(50, 100, z);
	glEnd();
	glutSwapBuffers();*/
	
	//if ( !bDrawing )
	{
		bDrawing = true;
		App.Loop();
		bDrawing = false;
		glutPostRedisplay();
	}
}

void reshape(int w, int h)
{
	agk::UpdateDeviceSize();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
		case 27:
			exit(0);
			break;
	}
}

bool bLeftDown = false;

void mouse(int button, int state, int x, int y)
{
	if ( button == GLUT_LEFT_BUTTON )
	{
		if ( state == GLUT_DOWN && !bLeftDown ) 
		{
			bLeftDown = true;
			agk::TouchPressed( 1, x,y );
		}
		
		if ( state == GLUT_UP && bLeftDown ) 
		{
			bLeftDown = false;
			agk::TouchReleased( 1, x,y );
		}
	}
}

void mouseMove(int x, int y)
{
	if ( bLeftDown )
	{
		agk::TouchMoved( 1, x,y );
	}
}

void idle()
{
	glutPostRedisplay();
}

/*
 * Main Loop 
 */
int main(int argc, char **argv)
{
	NSAutoreleasePool *pool;
    pool = [[NSAutoreleasePool alloc] init];
	
	g_musicListener = [[MusicListener alloc] init];

	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize( 320, 480 );
	glutInitWindowPosition( 50,350 );
	glutCreateWindow( argv[0] );
	
	agk::InitGL( 0 );
	App.Begin();
	
	glutReshapeFunc( reshape );
	glutDisplayFunc( display );
	glutKeyboardFunc( keyboard );
	glutMouseFunc( mouse );
	glutMotionFunc( mouseMove );
	glutPassiveMotionFunc( mouseMove );
	glutIdleFunc( idle );
	
	glutMainLoop();
	App.End();
	return 0;
	
	[pool drain];
}

void AppQuitNow ( void )
{
	// force mac to quit!
	App.g_iAppControlStage = 91;
}

