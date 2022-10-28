#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#if _WIN32
# include <windows.h>
#endif
#if __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
# include <GLUT/glut.h>
#else
# include <GL/gl.h>
# include <GL/glu.h>
# include <GL/glut.h>
#endif

#define KEY_ESC 27

typedef struct
{
    int width, height, x_pos, y_pos;
    bool is_fullscreen;
} window_t;

window_t g_mainwin;
float world_size = 25.0f;
float direction = 1.0f;
float rotation = 0.0f;
float g_last_time = 0.0f;;

void end_app()
{
	exit(EXIT_SUCCESS);
}

void render_object()
{

  // pivot point offset
  const float model_offset = 0.0f;
  // first vertex - local coordinate
  const float v1x = 0.0f + model_offset;
  const float v1y = 2.5f + model_offset;
  // second vertex - local coordinate
  const float v2x = 1.0f + model_offset;
  const float v2y = -0.5f + model_offset;
  // third vertex - local coordinate
  const float v3x = -1.0f + model_offset;
  const float v3y = -0.5f + model_offset;
  /*
  * Filling up the interior of the Spaceship with Red Colour.
  */
  glPushMatrix();
  glLoadIdentity();
  glRotatef(rotation, 0.0f, 0.0f, 1.0f);
  glScalef(0.5, 0.5, 0.0);

  glBegin(GL_TRIANGLES);
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(v1x, v1y, 0.0);
  glVertex3f(v2x, v2y, 0.0);
  glVertex3f(v3x, v3y, 0.0);
  glEnd();
  /*
  * Drawing the Outline for the Triangular Spaceship with Blue Colour
  */
  glBegin(GL_LINE_LOOP);
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(v1x, v1y, 0.0);
  glVertex3f(v2x, v2y, 0.0);
  glVertex3f(v3x, v3y, 0.0);
  glEnd();
  glPopMatrix();
}
/* Display callback */ 
void on_display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  render_object();
  int err;
  while ((err = glGetError()) != GL_NO_ERROR)
    printf("display: %s\n", gluErrorString(err));
  glutSwapBuffers();
}
void ship_control(char dir)
{
	if (dir == 'l')
	{
		rotation += 20;
	}
	else if (dir == 'r')
	{
		rotation += -20;
	}
	glutPostRedisplay();
}
/* Keyboard callback */
void on_keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
    case KEY_ESC:
		end_app();
		break;
	case 'a':
		ship_control('l');
		break;
	case 'd':
		ship_control('r');
		break;
    default:
      break;
   }
}
void on_idle()
{
    float cur_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    float dt = cur_time - g_last_time;
    //ship_movement(dt);
	//ship_control();
    g_last_time = cur_time;
    glutPostRedisplay();
}
void on_reshape(int w, int h)
{
	g_mainwin.width = w;
	g_mainwin.height = h;

	const float half_world_size = world_size / 2.0;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
	{
		float aspect = (float)h / (float)w;
		glOrtho(-half_world_size, half_world_size, -half_world_size * aspect, half_world_size * aspect, -half_world_size, half_world_size);
	}
	else
	{
		float aspect = (float)w / (float)h;
		glOrtho(-half_world_size * aspect, half_world_size * aspect, -half_world_size, half_world_size, -half_world_size, half_world_size);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void init_app(int* argcp, char** argv, window_t* mainwinp)
{
	// GLUT & OpenGL setup
	glutInit(argcp, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	// Display related setup
	glutInitWindowPosition(mainwinp->x_pos, mainwinp->y_pos);
	glutInitWindowSize(mainwinp->width, mainwinp->height);
	glutCreateWindow("PLAYER SPACESHIP");

	if (mainwinp->is_fullscreen == true)
	{
		glutFullScreen();
	}

	glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);
	glutKeyboardFunc(on_keyboard);
	// Define the idle function
	glutIdleFunc(on_idle);
	g_last_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
}

void run_app()
{
	glutMainLoop();
}

void load_config(int* argcp, char** argv, window_t* mainwin_p)
{
	mainwin_p->width = 1024;
	mainwin_p->height = 768;
	mainwin_p->is_fullscreen = true;
}

int main(int argc, char** argv)
{
	load_config(&argc, argv, &g_mainwin);
	init_app(&argc, argv, &g_mainwin);
	run_app();
	return (EXIT_SUCCESS);
}