#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <conio.h>
#include <math.h>
#include <time.h>
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
#define PI 3.14

//Creating new pieces of Data Strcutures..................
typedef enum 
{
	ALIVE, DEAD
}obj_life;

typedef struct
{
	int stimer, mstimer, mtimer;
	int wave, wave_timer;
	int offset, acTimer, decTimer;
	float dt;
}timers;

typedef struct
{
    int width, height, x_pos, y_pos;
    bool is_fullscreen;
} window_t;

typedef struct
{
	float w_border_limit;
	float h_border_limit;
	float color;
}arena;

typedef struct
{
	float vx, vy;
}vector;

typedef struct
{
	vector v1, v2, v3;
	float velocity, angle;
	bool speedDown, speedUp;
	vector transform, unit;
	obj_life status;
}space_ship;

typedef struct
{
	bool fw, lf, rf, shoot;
	bool start, reset, toggle_weapon;
}key_stats;

typedef struct
{
	vector v1, v2, v3, v4, v5;
	vector v6, v7, v8, v9, v10;
	float color;
}ship_weapon;

typedef struct
{
	vector v1, v2, v3;
	vector v4, v5, v6;
	vector v7, v8, v9;
	vector transform, unit;
	float angle, velocity;
	obj_life life;
}ship_ammo;

typedef struct
{
	vector v, v1, v2, v3, v4, v5, v6, v7;
	vector unit, transform;
	float radius, angle, velocity, color1, color2, color3;
	float rotation;
	int hit_points, hit;
	obj_life status;
}asteroid;

typedef struct
{
	int score;	
}gameplay;

//Global Struct and normal(primitive type) variables
window_t g_mainwin;
arena as_ar;
asteroid astr[1000];
space_ship pl_ship;
ship_weapon gun, mslauncher;
ship_ammo bullets[2000];
key_stats keyboard;
timers aatime;
gameplay player;

float world_size = 25.0f;
float g_last_time = 0.0f;
int bullet_length = 0, astr_length = 0;//Array sizes for bullets and asteroids...................
int nearAstr = 0;//Variable for seeking nearby asteroid index for the missile launcher.................

//All function declerations
void init_ship();
void init_bullets();
void init_shipGun();
void init_shipMsLauncher();
void init_asteroids();
void init_object();
float calcCollDist(vector, vector);
void detect_collide_arena();
void detect_collide_astrship();
void detect_collide_bullarena();
void astr_color_update(int);
void detect_collide_astrbull();
void detect_collide_astrarena();//For Asteroid Bouncing off from the screen edges.........
void detect_collide_astrbounce();
void render_arena();
void draw_spaceship_antenna();
void draw_spaceship();
void draw_gun();
void draw_mslauncher_fill();
void draw_mslauncher();
void draw_bullets(int);
void draw_asteroid(int);
void render_spaceship();
void render_bullets();
void render_asteroid();
void render_string(float, float, void*, const char[], int);
void calcGameTime();
void render_time();
void render_score();
void on_display();
void on_release(unsigned char, int, int);
void on_keyboard(unsigned char, int, int);
vector calcUniVec(float, vector);
vector calcTransform(vector, vector, float);
void add_bullet();
bool shipTrackAstr(float, float);
void seek_missile();
void gun_activate();//Bullet Movement......
int calcFirstAstrPos();
int calcNearAstr();
void ship_controller();
void shipSpeedingUpState();
void shipSpeedingDownState();
void transform_astroid();
float calcAngleDir(float, float, float, float);
float calcAngle(vector, vector);
float retRandX(float);
int calcHit(float);
void add_asteroid(int);
int numOnScreenAstr();
void asteroid_attack(int);
void asteroid_rotation();
vector Wbound(float, vector);
vector Hbound(float, vector);
vector calcAstrPos(vector);
void reset_ship();
void end_app();
void run_game();
void on_idle();
void on_reshape(int, int);
void init_game(int*, char**, window_t*);
void play_game();
void load_config(int*, char**, window_t*);
float calcDeltaTime();


int main(int argc, char** argv)
{
	/**
	* Starting of this entire Programm.................
	*/
	srand(time(NULL));
	init_object();
	load_config(&argc, argv, &g_mainwin);
	init_game(&argc, argv, &g_mainwin);
	play_game();
	return (EXIT_SUCCESS);
}

/*Section 1***************INITIALIZING THE OBJECTS****************/
void init_ship()
{
	pl_ship.status = ALIVE;
	pl_ship.speedUp = pl_ship.speedDown = false;
	pl_ship.v1.vx = 0.0f;
	pl_ship.v1.vy = 2.25f;
	pl_ship.v2.vx = 1.0f;
	pl_ship.v2.vy = -0.5f;
	pl_ship.v3.vx = -1.0f;
	pl_ship.v3.vy = -0.5f;
	pl_ship.velocity = pl_ship.transform.vx = pl_ship.transform.vy = pl_ship.angle = 0.0f;
	pl_ship.unit = pl_ship.transform;
}

void init_bullets()
{
	for (int i = 0; i < 2000; i++)
	{
		bullets[i].v1.vy = bullets[i].v2.vy = bullets[i].v3.vy = 0.0f;
		bullets[i].v4.vy = bullets[i].v5.vy = bullets[i].v6.vy = 0.5f;
		bullets[i].v7.vy = bullets[i].v8.vy = bullets[i].v9.vy = -0.5f;
		bullets[i].v1.vx = bullets[i].v4.vx = bullets[i].v7.vx = 0.0f;
		bullets[i].v2.vx = bullets[i].v5.vx = bullets[i].v8.vx = 0.5f;
		bullets[i].v3.vx = bullets[i].v6.vx = bullets[i].v9.vx = -0.5f;
		bullets[i].transform.vx = bullets[i].transform.vy = 0.0f;
		bullets[i].unit.vx = bullets[i].unit.vy = 0.0f;
		bullets[i].velocity = 0.0f;
	}
}

void init_shipGun()
{
	gun.v1.vx = gun.v4.vx = -0.2f;
	gun.v2.vx = gun.v3.vx = 0.2f;
	gun.v1.vy = gun.v2.vy = 2.25f;
	gun.v3.vy = gun.v4.vy = 2.5f;
	gun.color = 0.0f;
}

void init_shipMsLauncher()
{
	mslauncher.color = 0.0f;

	mslauncher.v1.vx = -1.0f;
	mslauncher.v2.vx = 1.0f;
	mslauncher.v1.vy = mslauncher.v2.vy = 2.25f;

	mslauncher.v3.vx = -1.0f;
	mslauncher.v3.vy = 2.25f;
	mslauncher.v4.vx = -1.0f;
	mslauncher.v4.vy = 2.5f;
	mslauncher.v5.vx = -0.5f;
	mslauncher.v5.vy = 2.5f;
	mslauncher.v6.vx = -0.5f;
	mslauncher.v6.vy = 2.25f;

	mslauncher.v7.vx = 1.0f;
	mslauncher.v7.vy = 2.25f;
	mslauncher.v8.vx = 1.0f;
	mslauncher.v8.vy = 2.5f;
	mslauncher.v9.vx = 0.5f;
	mslauncher.v9.vy = 2.5f;
	mslauncher.v10.vx = 0.5f;
	mslauncher.v10.vy = 2.25f;
}

void init_asteroids()
{
	for (int i = 0; i < 1000; i++)
	{
		astr[i].angle = astr[i].velocity = astr[i].radius = astr[i].rotation = 0.0f;
		astr[i].unit.vx = astr[i].unit.vy = 0.0f;
		astr[i].v = astr[i].transform = astr[i].unit;
		astr[i].v1 = astr[i].v2 = astr[i].v3 = astr[i].v4 = astr[i].v5 = astr[i].v6 = astr[i].v7 = astr[i].v;
		astr[i].color1 = astr[i].color2 = astr[i].color3 = 1.0f;
		astr[i].status = ALIVE;
		astr[i].hit_points = astr[i].hit = 0;
	}
}

void init_object()
{
	as_ar.h_border_limit = as_ar.w_border_limit = 0.0f;
	as_ar.color = 1.0f;
	init_ship();
	init_shipGun();
	init_shipMsLauncher();
	init_bullets();
	init_asteroids();
	keyboard.fw = keyboard.lf = keyboard.rf = keyboard.shoot = keyboard.start = keyboard.reset = keyboard.toggle_weapon = false;
	aatime.mstimer = aatime.mtimer = aatime.stimer = aatime.wave = aatime.offset = aatime.wave_timer = aatime.acTimer = aatime.decTimer = 0;
	aatime.dt = 0.0f;
	player.score = 0;
}

/*Section 2*************Defining the Collison Protocols************************/
float calcCollDist(vector v1, vector v2)
{
	float distance = 0.0;
	const float param1 = (v1.vx) - (v2.vx);
	const float param2 = (v1.vy) - (v2.vy);
	distance = sqrt((param1 * param1) + (param2 * param2));
	return distance;
}

void detect_collide_arena()
{
	/*
	* This function checks whether the ship collides with the border/arena
	*/
	const float collide_offset_x = as_ar.w_border_limit;
	const float collide_offset_y = as_ar.h_border_limit;//hit bound marked as Red
	const float collide_bound_x = as_ar.w_border_limit - 2.0f;
	const float collide_bound_y = as_ar.h_border_limit - 1.2f;//warning bound marked as Yellow
	float bound_radius_y = (2.5f * 0.5f);
	float bound_radius_x = (2.5f * 0.5f);

	if (pl_ship.transform.vy < 0)
		bound_radius_y *= -1;
	if (pl_ship.transform.vx < 0)
		bound_radius_x *= -1;

	const float collide_distance_y = pl_ship.transform.vy + bound_radius_y;
	const float collide_distance_x = pl_ship.transform.vx + bound_radius_x;
		
	if ((collide_distance_y >= collide_bound_y && collide_distance_y <collide_offset_y)|| 
	(collide_distance_y <= -collide_bound_y && collide_distance_y >-collide_offset_y)||
	(collide_distance_x >= collide_bound_x && collide_distance_x < collide_offset_x) ||
	(collide_distance_x <= -collide_bound_x && collide_distance_x > -collide_offset_x))
	{
		as_ar.color = 0.0f;
	}
	else if ((collide_distance_y<collide_bound_y && collide_distance_y > -collide_bound_y) &&
		(collide_distance_x<collide_bound_x && collide_distance_x > -collide_bound_x))
	{
		  as_ar.color = 1.0f;
	}
		
	else
	{
		//Spaceship destroy
		pl_ship.status = DEAD;
		//Animate
	}
}

void detect_collide_bullarena()
{
	/*
	* Detects the collisions between the arena boundaries and the bullets..............
	*/
	const float collide_bound_x = as_ar.w_border_limit - 0.5f;
	const float collide_bound_y = as_ar.h_border_limit - 0.5f;//warning bound marked as Yellow
	float collide_distance_y = 0.0f;
	float collide_distance_x = 0.0f;
	float bound_radius_y = 0.125f;
	float bound_radius_x = 0.125f;

	for (int i = 0; i < bullet_length; i++)
	{
		if (bullets[i].life == ALIVE)
		{
			if (bullets[i].transform.vy < 0)
				bound_radius_y *= -1;
			if (bullets[i].transform.vx < 0)
				bound_radius_x *= -1;

			collide_distance_y = bullets[i].transform.vy + bound_radius_y;
			collide_distance_x = bullets[i].transform.vx + bound_radius_x;

			if (collide_distance_y >= collide_bound_y ||
				collide_distance_y <= -collide_bound_y ||
				collide_distance_x >= collide_bound_x ||
				collide_distance_x <= -collide_bound_x)
			{
				bullets[i].life = DEAD;
			}
		}
	}
}


void astr_color_update(int pos)
{
	/**
	* Function to update the asteroid colour dependning on its hitpoints (used only in the function below)
	*/
	if (astr[pos].hit_points == 1)
	{
		astr[pos].color1 = 1.0f;
		astr[pos].color2 = astr[pos].color3 = 0.0f;
	}
	else if (astr[pos].hit_points == 2)
	{
		astr[pos].color1 = astr[pos].color3 = 0.0f;
		astr[pos].color2 = 1.0f;
	}
	else
	{
		astr[pos].color1 = 0.0f;
		astr[pos].color2 = astr[pos].color3 = 1.0f;
	}
}

void detect_collide_astrbull()
{
	/*
	*Detects the collision between bullets and asteroids.............
	*/
	const float bullet_radi = 0.125f;
	float distance, asteroid_radi = 0.0f;

	for (int i = 0; i < bullet_length; i++)
	{
		if (bullets[i].life == ALIVE)
		{
			for (int j = 0; j < astr_length; j++)
			{
				if (astr[j].status == ALIVE)
				{
					/**
					* Using the Circle Collision detection rule...........
					*/
					asteroid_radi = astr[j].radius;
					distance = calcCollDist(bullets[i].transform, astr[j].transform);

					if (distance < (asteroid_radi + bullet_radi))
					{
						bullets[i].life = DEAD;
						if (astr[j].hit_points == 0)
						{
							astr[j].status = DEAD;
							player.score++;
							//Animate
						}
						else
						{
							astr_color_update(j);
							astr[j].hit_points--;
						}
					}
				}
			}
			//distance = 0.0f;
		}
	}
}

void detect_collide_astrship()
{
	/*
	*Detects the Collision between asteroid and the player spaceship
	*/
	float shp_radius = (2.5f * 0.5f);
	float asteroid_radi = 0.0f;
	float distance = 0.0f;
	
	int i = 0;
	while (i < astr_length)
	{
		if (astr[i].status == ALIVE)
		{
			asteroid_radi = astr[i].radius;
			distance = calcCollDist(astr[i].transform, pl_ship.transform);
			if (distance < (asteroid_radi + shp_radius))
			{
				astr[i].status = DEAD;
				pl_ship.status = DEAD;
				//Animate....
				break;
			}
		}
		distance = 0.0f;
		i++;
	}
}

void detect_collide_astrarena()
{
	/**
	* This is another collison detection mechanism where each asteroid once collided with the arena boundary,
	* bounces back at a desired angle......................
	*/
	const float collide_bound_x = as_ar.w_border_limit - 0.1f;
	const float collide_bound_y = as_ar.h_border_limit - 0.1f;//warning bound marked as Yellow
	float collide_distance_y, collide_distance_x, bound_radius_y, bound_radius_x = 0.0f;

	for (int i = 0; i < astr_length; i++)
	{
		bound_radius_x = bound_radius_y = astr[i].radius;
		if (astr[i].status == ALIVE)
		{
			if (astr[i].transform.vy < 0)
				bound_radius_y *= -1;
			if (astr[i].transform.vx < 0)
				bound_radius_x *= -1;

			collide_distance_y = astr[i].transform.vy + bound_radius_y;
			collide_distance_x = astr[i].transform.vx + bound_radius_x;

			if (collide_distance_y >= collide_bound_y ||
				collide_distance_y <= -collide_bound_y)
			{
				if (astr[i].hit == 1)
				{
					/**
					* calculating the angle to bounce of from the Height of the frame
					*/
					astr[i].angle = (180 - astr[i].angle);
					astr[i].unit = calcUniVec(astr[i].angle, astr[i].unit);
					astr[i].transform = calcTransform(astr[i].transform, astr[i].unit, astr[i].velocity);
				}
			}
			else if (collide_distance_x >= collide_bound_x ||
				collide_distance_x <= -collide_bound_x)
			{
				if (astr[i].hit == 1)
				{
					/**
					* calculating the angle to bounce of from the Width of the frame
					*/
					astr[i].angle = (-astr[i].angle);
					astr[i].unit = calcUniVec(astr[i].angle, astr[i].unit);
					astr[i].transform = calcTransform(astr[i].transform, astr[i].unit, astr[i].velocity);
				}
			}
			else
			{
				astr[i].hit = 1;
			}
		}
	}
}

void detect_collide_astrbounce()
{
	/*
	*Detects the collision between each and every asteroids to perform bounces.............
	*/
	float astr_radi1, astr_radi2 = 0.0f;
	float distance = 0.0f;

	for (int i = 0; i < astr_length; i++)
	{
		if (astr[i].status == ALIVE)
		{
			for (int j = 0; j < astr_length; j++)
			{
				if (i!=j && astr[j].status == ALIVE)
				{
					/**
					* Using the Circle Collision detection rule...........
					*/
					astr_radi1 = astr[i].radius;
					astr_radi2 = astr[j].radius;;
					distance = calcCollDist(astr[i].transform, astr[j].transform);

					if (distance < (astr_radi1 + astr_radi2) && astr[i].hit == 1 && astr[j].hit == 1)
					{
						astr[i].angle = (- astr[i].angle);
						astr[i].unit = calcUniVec(astr[i].angle, astr[i].unit);
						astr[i].transform = calcTransform(astr[i].transform, astr[i].unit, astr[i].velocity);
					}
				}
			}
			distance = 0.0f;
		}
	}
}

/*Section 3***********DRAWING AND RENDERING OBJECTS*****************/
void render_arena()
{
	glPushMatrix();
	glLoadIdentity();
	glBegin(GL_LINE_LOOP);
	glColor3f(1.0f, as_ar.color, as_ar.color);
	glVertex3f(-as_ar.w_border_limit, as_ar.h_border_limit, 0.0f);
	glVertex3f(-as_ar.w_border_limit, -as_ar.h_border_limit, 0.0f);
	glVertex3f(as_ar.w_border_limit, -as_ar.h_border_limit, 0.0f);
	glVertex3f(as_ar.w_border_limit, as_ar.h_border_limit, 0.0f);
	glEnd();
	glPopMatrix();
}

void draw_spaceship_antenna()
{
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex3f(pl_ship.v3.vx, pl_ship.v3.vy, 0.0);
	glVertex3f(-1.0f, -1.5f, 0.0);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(pl_ship.v2.vx, pl_ship.v2.vy, 0.0);
	glVertex3f(1.0f, -1.5f, 0.0);
	glEnd();
}

void draw_spaceship()
{
	glBegin(GL_TRIANGLES);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(pl_ship.v1.vx, pl_ship.v1.vy, 0.0);
	glVertex3f(pl_ship.v2.vx, pl_ship.v2.vy, 0.0);
	glVertex3f(pl_ship.v3.vx, pl_ship.v3.vy, 0.0);
	glEnd();
	/*
	* Drawing the Outline for the Triangular Spaceship with Blue Colour
	*/
	glBegin(GL_LINE_LOOP);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(pl_ship.v1.vx, pl_ship.v1.vy, 0.0);
	glVertex3f(pl_ship.v2.vx, pl_ship.v2.vy, 0.0);
	glVertex3f(pl_ship.v3.vx, pl_ship.v3.vy, 0.0);
	glEnd();
}

void draw_gun()
{
	glBegin(GL_POLYGON);
	glColor3f(gun.color, gun.color, gun.color);
	glVertex3f(gun.v1.vx, gun.v1.vy, 0.0f);
	glVertex3f(gun.v2.vx, gun.v2.vy, 0.0f);
	glVertex3f(gun.v3.vx, gun.v3.vy, 0.0f);
	glVertex3f(gun.v4.vx, gun.v4.vy, 0.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(gun.v1.vx, gun.v1.vy, 0.0f);
	glVertex3f(gun.v2.vx, gun.v2.vy, 0.0f);
	glVertex3f(gun.v3.vx, gun.v3.vy, 0.0f);
	glVertex3f(gun.v4.vx, gun.v4.vy, 0.0f);
	glEnd();
}

void draw_mslauncher_fill()
{
	glColor3f(mslauncher.color, mslauncher.color, mslauncher.color);
	glBegin(GL_POLYGON);
	glVertex3f(mslauncher.v3.vx, mslauncher.v3.vy, 0.0f);
	glVertex3f(mslauncher.v4.vx, mslauncher.v4.vy, 0.0f);
	glVertex3f(mslauncher.v5.vx, mslauncher.v5.vy, 0.0f);
	glVertex3f(mslauncher.v6.vx, mslauncher.v6.vy, 0.0f);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3f(mslauncher.v7.vx, mslauncher.v7.vy, 0.0f);
	glVertex3f(mslauncher.v8.vx, mslauncher.v8.vy, 0.0f);
	glVertex3f(mslauncher.v9.vx, mslauncher.v9.vy, 0.0f);
	glVertex3f(mslauncher.v10.vx, mslauncher.v10.vy, 0.0f);
	glEnd();
}

void draw_mslauncher()
{
	draw_mslauncher_fill();

	glColor3f(1.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_LOOP);
	glVertex3f(mslauncher.v1.vx, mslauncher.v1.vy, 0.0f);
	glVertex3f(mslauncher.v2.vx, mslauncher.v2.vy, 0.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(mslauncher.v3.vx, mslauncher.v3.vy, 0.0f);
	glVertex3f(mslauncher.v4.vx, mslauncher.v4.vy, 0.0f);
	glVertex3f(mslauncher.v5.vx, mslauncher.v5.vy, 0.0f);
	glVertex3f(mslauncher.v6.vx, mslauncher.v6.vy, 0.0f);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(mslauncher.v7.vx, mslauncher.v7.vy, 0.0f);
	glVertex3f(mslauncher.v8.vx, mslauncher.v8.vy, 0.0f);
	glVertex3f(mslauncher.v9.vx, mslauncher.v9.vy, 0.0f);
	glVertex3f(mslauncher.v10.vx, mslauncher.v10.vy, 0.0f);
	glEnd();
}

void render_spaceship()
{
	glPushMatrix();
	glLoadIdentity();
	/**
	* Performing all the transformations
	*/
	glTranslatef(pl_ship.transform.vx, pl_ship.transform.vy, 0.0f);
	glRotatef(pl_ship.angle, 0.0f, 0.0f, 1.0f);
	glScalef(0.5f, 0.5f, 0.0f);
	draw_spaceship();//Drawing the Space Ship in it's local coordinates
	draw_spaceship_antenna();
	if (keyboard.toggle_weapon == false)
		draw_gun();
	else
		draw_mslauncher();
	glPopMatrix();
}

void draw_bullets(int i)
{
	glBegin(GL_POINTS);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(bullets[i].v1.vx, bullets[i].v1.vy, 0.0f);
	glVertex3f(bullets[i].v2.vx, bullets[i].v2.vy, 0.0f);
	glVertex3f(bullets[i].v3.vx, bullets[i].v3.vy, 0.0f);
	glVertex3f(bullets[i].v4.vx, bullets[i].v4.vy, 0.0f);
	glVertex3f(bullets[i].v5.vx, bullets[i].v5.vy, 0.0f);
	glVertex3f(bullets[i].v6.vx, bullets[i].v6.vy, 0.0f);
	glVertex3f(bullets[i].v7.vx, bullets[i].v7.vy, 0.0f);
	glVertex3f(bullets[i].v8.vx, bullets[i].v8.vy, 0.0f);
	glVertex3f(bullets[i].v9.vx, bullets[i].v9.vy, 0.0f);
	glEnd();
}

void render_bullets()
{
	for (int i = 0; i < bullet_length; i++)
	{
		if (bullets[i].life == ALIVE)
		{
			glPushMatrix();
			glLoadIdentity();
			glTranslatef(bullets[i].transform.vx, bullets[i].transform.vy, 0.0f);
			glRotatef(bullets[i].angle, 0.0f, 0.0f, 1.0f);
			glScalef(0.15f, 0.15f, 0.0f);
			draw_bullets(i);
			glPopMatrix();
		}
	}
}

void draw_asteroid(int i)
{
	glColor3f(astr[i].color1, astr[i].color2, astr[i].color3);

	glBegin(GL_LINE_LOOP);
	glVertex3f(astr[i].v1.vx, astr[i].v1.vy, 0);
	glVertex3f(astr[i].v2.vx, astr[i].v2.vy, 0);
	glVertex3f(astr[i].v3.vx, astr[i].v3.vy, 0);
	glVertex3f(astr[i].v4.vx, astr[i].v4.vy, 0);
	glVertex3f(astr[i].v5.vx, astr[i].v5.vy, 0);
	glVertex3f(astr[i].v6.vx, astr[i].v6.vy, 0);
	glVertex3f(astr[i].v7.vx, astr[i].v7.vy, 0);
	glEnd();
}

void render_asteroid()
{
	for (int i = 0; i < astr_length; i++)
	{
		if (astr[i].status == ALIVE)
		{
			glPushMatrix();
			glLoadIdentity();
			glTranslatef(astr[i].transform.vx, astr[i].transform.vy, 0.0f);
			glRotatef(astr[i].rotation, 0.0f, 0.0f, 1.0f);
			draw_asteroid(i);
			glPopMatrix();
		}
	}
}

void render_string(float x, float y, void* font, const char string[], int size)
{
	const float half_world_size = 25.0f / 2.0;
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	// Set the projection - use this to render text
	if (g_mainwin.width <= g_mainwin.height)
	{
		float aspect = (float)g_mainwin.height / (float)g_mainwin.width;
		glOrtho(-half_world_size, half_world_size, -half_world_size * aspect, half_world_size * aspect, -half_world_size, half_world_size);
	}
	else
	{
		float aspect = (float)g_mainwin.width / (float)g_mainwin.height;
		glOrtho(-half_world_size * aspect, half_world_size * aspect, -half_world_size, half_world_size, -half_world_size, half_world_size);
	}
	glMatrixMode(GL_MODELVIEW);

	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2f(x, y);

	int i = 0;
	while(i < size)
	{
		glutBitmapCharacter(font, string[i]);
		i++;
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

/*int calcIntSize(int num)
{
	int count = 0;
	if (num == 0)
	{
		count = 1;
	}
	else
	{
		while (num != 0)
		{
			count++;
			num /= 10;
		}
	}
	return count;
}
char* convertToString(int num)
{

	const int size = calcIntSize(num);
	int n = size - 1;
	char main_string[100];
	int part = -1;
	int i = 0;
	while (i< size)
	{
		part = num % 10;
		main_string[n] = (part + 48);
		num /= 10;
		n--;
		i++;
	}


	return main_string;
}
*/

void render_score()
{
	printf("\nAsteroids Destroyed: %d", player.score);
}

void calcGameTime()
{
	/*
	* Calculates the elapsed running time of the entire game........
	*/
	aatime.offset++;
	aatime.offset = aatime.offset % 30;
	if(aatime.offset==0)
	{ 
		aatime.mstimer++;
		aatime.mstimer = aatime.mstimer % 60;
		if (aatime.mstimer == 0)
		{
			aatime.stimer++;
			aatime.stimer = aatime.stimer % 60;
			if (aatime.stimer == 0)
				aatime.mtimer++;
		}
	}
}
void render_time()
{
	printf("\n TIME: MIN: %d SEC: %d MS: %d", aatime.mtimer, aatime.stimer, aatime.mstimer);
}

/* Display callback */ 
void on_display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  /**
  * Drawing all the objects in the world co ordinates
  */
  if (keyboard.start == false)
  {
	  //RENDER WELCOMING TEXT HERE>>>>>>
	  render_string(0, 0, GLUT_BITMAP_HELVETICA_18, "+", 1);
  }
  else
  {
	  render_arena();
	  render_asteroid();
	  render_bullets();

	  if (pl_ship.status == ALIVE)
	  {
		  render_spaceship();
	  }
	  else
	  {
		  //RENDER RESTART MESSAGE
		  render_string(-5, 0, GLUT_BITMAP_HELVETICA_18, "< PRESS R KEY TO PLAY THE GAME AGAIN >", 38);
	  }
	  //render_score();
	  //render_time();
  }
  /**
  * error checking
  */
  int err;
  while ((err = glGetError()) != GL_NO_ERROR)
    printf("display: %s\n", gluErrorString(err));

  glutSwapBuffers();
}

/*Section 4********KEYBOARD CONTROLLERS***************/
void on_release(unsigned char key, int x, int y)
{
	if (key == 'w')
	{
		pl_ship.velocity = 10.0f;//On Key release the velocity will decelerate to 10 till 0............
		pl_ship.speedDown = true;
		keyboard.fw = false;
	}
	if (key == 'a')
	{
		keyboard.lf = false;
	}
	if (key == 'd')
	{
		keyboard.rf = false;
	}
	if (key == ' ')
	{
		keyboard.shoot = false;
		if (keyboard.toggle_weapon == false)
			gun.color = 0.0f;
		else
		{
			keyboard.toggle_weapon = false;
			mslauncher.color = 0.0f;
		}
	}
	if (key == 'r')
	{
		keyboard.reset = false;
	}
}

/* Keyboard callback */
void on_keyboard(unsigned char key, int x, int y)
{
	if (keyboard.start == false)
		keyboard.start = true;
	else if (key == ' ')
		keyboard.shoot = true;
	else if (key == 'w')
		keyboard.fw = true;
	else if (key == 'a')
		keyboard.lf = true;
	else if (key == 'd')
		keyboard.rf = true;
	else if (key == 'r')
		keyboard.reset = true;
	else if (key == 'z')
	{
		if (keyboard.toggle_weapon == false)
			keyboard.toggle_weapon = true;
		else
			keyboard.toggle_weapon = false;
	}
	else if (key == KEY_ESC)
		end_app();
}

/*Sectopn 5*********Vector Transformations**************/
vector calcUniVec(float angle, vector v)
{
	const float angle_rad = (angle + 90.0f) * (PI / 180.0f);
	v.vx = cos(angle_rad);
	v.vy = sin(angle_rad);
	return v;
}

vector calcTransform(vector transform, vector unit, float speed)
{
	//Smoother motion calculation......
	//(dt * 0.5f) to make the transformation even more smoother

	transform.vx += (unit.vx * speed * (aatime.dt * 0.5f));
	transform.vy += (unit.vy * speed * (aatime.dt * 0.5f));
	return transform;
}

/*Sectopn 6***************BULLETS AND GUNS CALCULATIONS************************/
void add_bullet()
{
	if (bullet_length < 1900)
	{
		bullets[bullet_length].life = ALIVE;
		bullets[bullet_length].unit = calcUniVec(pl_ship.angle, bullets[bullet_length].unit);
		bullets[bullet_length].velocity = 45.0f;
		bullets[bullet_length].transform = pl_ship.transform;

		bullets[bullet_length].transform.vx += (bullets[bullet_length].unit.vx * 1.5f);
		bullets[bullet_length].transform.vy += (bullets[bullet_length].unit.vy * 1.5f);
		/*The above 2 linese are used to initially translate 1.5 units from the origin(0,0), so that the bullet starts
		*shooting from the tip of the spaceship rather than the center of the spaceship.
		*/
		bullet_length++;
	}
}

void seek_missile()
{
	astr[nearAstr].status = DEAD;
	player.score++;
}

void gun_activate()
{
	//Bullets Transformation gets updated here in the background......
	for (int i = 0; i < bullet_length; i++)
	{
		if (bullets[i].life == ALIVE)
			bullets[i].transform = calcTransform(bullets[i].transform, bullets[i].unit, bullets[i].velocity);
	}
}

int calcFirstAstrPos()
{
	int pos = 0;
	for (int i = 0; i < astr_length; i++)
	{
		if (astr[i].status == ALIVE)
		{
			pos = i;
			break;
		}
	}
	return pos;
}

int calcNearAstr()
{
	const int init_pos = calcFirstAstrPos();
	int ret_pos = init_pos;
	float d1 = calcCollDist(astr[init_pos].transform, pl_ship.transform);
	float d2 = 0.0f;

	for (int i = init_pos + 1; i < astr_length; i++)
	{
		if (astr[i].status == ALIVE)
		{
			d2 = calcCollDist(astr[i].transform, pl_ship.transform);
			if (d1 > d2)
			{
				d1 = d2;
				ret_pos = i;
			}
		}
	}
	return ret_pos;
}

/*Section 7**********SPACESHIP TRANSFORMATIONS AND FUNCTIONALITIES**********************/
void shipSpeedingUpState()
{
	/**
	*Accelration of the spaceship when the 'w' key is pressed down.............. (BONUS FEATURE)
	*/
	if (aatime.acTimer == 200)
	{
		aatime.acTimer = 0;
		pl_ship.velocity += 5.0f;
		if (pl_ship.velocity >= 25)
			pl_ship.speedUp = false;
	}
	else
	{
		aatime.acTimer++;
	}
}

void shipSpeedingDownState()
{
	/**
	*Deceleration of the spaceship when the 'w' key has been released..............(BONUS FEATURE)
	*/
	if (aatime.decTimer == 400)
	{
		aatime.decTimer = 0;
		pl_ship.velocity--;
		if (pl_ship.velocity == 0)
			pl_ship.speedDown = false;
	}
	else
	{
		aatime.decTimer++;
	}
}

void ship_controller()
{
	/*
	* *
	* Basic Motions
	*/
	if (keyboard.fw == true && pl_ship.velocity <= 25.0f)
	{
		pl_ship.speedUp = true;
		shipSpeedingUpState();
		pl_ship.unit = calcUniVec(pl_ship.angle, pl_ship.unit);
		pl_ship.speedDown = false;
	}
    if (keyboard.lf == true)
	{
		pl_ship.angle += (300.0f * aatime.dt);//Smoother rotation (frame independency)
		keyboard.fw = false;
	}
	if (keyboard.rf == true)
	{
		pl_ship.angle -= (300.0f * aatime.dt);//Smoother rotation (frame independency)
		keyboard.fw = false;
	}
	if (pl_ship.speedDown == true)
	{
		shipSpeedingDownState();
	}
	if (keyboard.shoot == true)
	{
		/**
		* Shooting............
		*/
		if (keyboard.toggle_weapon == false)
		{
			add_bullet();
			gun.color = 1.0f;
		}
		else if (keyboard.toggle_weapon == true && 
			shipTrackAstr(pl_ship.angle, calcAngle(pl_ship.transform, astr[nearAstr].transform)))
		{
			seek_missile();
			mslauncher.color = 1.0f;
		}
		keyboard.shoot = false;
	}

	pl_ship.transform = calcTransform(pl_ship.transform, pl_ship.unit, pl_ship.velocity);
}

bool shipTrackAstr(float angle1, float angle2)
{
	/**
	* This function is used to detect whether the spaceship points towards the nearest asteroid for missile launching.......................
	*/
	bool flag = false;
	int a1 = (int)angle1 % 360;
	int a2 = (int)angle2 % 360;
	if (a1 < 0)
		a1 *= -1;
	if (a2 < 0)
		a2 *= -1;
	a1 = 360 - a1;
	if (a1 >= a2 && a1 <= (a2 + 10))
	{
		flag = true;
	}
	if (flag == false)
	{
		a1 = 360 - a1;
		if (a1 >= a2 && a1 <= (a2 + 10))
		{
			flag = true;
		}
	}
	return flag;
}

/*Section 8*************ASTEROIDS FEATURES**************************/
void transform_astroid()
{
	for (int i = 0; i < astr_length; i++)
	{
		if(astr[i].status == ALIVE)
			astr[i].transform = calcTransform(astr[i].transform, astr[i].unit, astr[i].velocity);
	}
}

float calcAngleDir(float angle, float y1, float y2, float x2)
{
	int dir = 1;

	if (angle < 0)
		angle *= -1;

	if (y2 > y1)
	{
		angle += 90;
	}
	else
	{
		angle = 90 - angle;
	}

	if (x2 > 0)
		dir = 1;
	else if (x2 < 0)
		dir = -1;

	angle = angle * dir;
	return	angle;
}

float calcAngle(vector v1, vector v2)
{
	/**
	* Calculates the Asteroid angle to collide the ships in reference to it's current state 
	*(each angle differs in different attack waves)............
	*/
	float rat = 0, x = 0, y = 0, angle = 0;
	float x1 = v2.vx, x2 = v1.vx;
	float y1 = v2.vy, y2 = v1.vy;

	if (x1 == x2)
	{
		if (y2 > 0)
		{
			angle = 180;
		}
		else if (y2 < 0)
		{
			angle = 0;
		}
		return angle;
	}
	else
		x = x2 - x1;

	if (y2 == y1)
	{
		if (x2 > 0)
			angle = 90;
		else if (x2 < 0)
			angle = -90;
		return angle;
	}
	else
	{
		y = y2 - y1;

		rat = y / x;
		angle = atan(rat) * (180.0f/PI);
		angle = calcAngleDir(angle, y1, y2, x2);
	}
	return angle;
}

float retRandX(float r)
{
	const int maxX = (r * 10) + 1;
	const int minX = (r * 10) / 2;
	return ((float)(minX + (rand() % (int)(maxX - minX))) / 10);
}

asteroid calcAstrShape(asteroid a)
{
	const float r = a.radius;
	a.v1.vx = retRandX(r);
	a.v1.vy = sqrt((r * r) - (a.v1.vx * a.v1.vx));
	a.v2.vx = 0.0f;
	a.v2.vy = retRandX(r);
	a.v3.vx = -retRandX(r);
	a.v3.vy = sqrt((r * r) - (a.v3.vx * a.v3.vx));
	a.v4.vx = -retRandX(r);
	a.v4.vy = 0.0f;
	a.v5.vx = -retRandX(r);
	a.v5.vy = -(sqrt((r * r) - (a.v5.vx * a.v5.vx)));
	a.v6.vx = 0.0f;
	a.v6.vy = -retRandX(r);
	a.v7.vx = retRandX(r);
	a.v7.vy = -(sqrt((r * r) - (a.v7.vx * a.v7.vx)));
	return a;
}

int calcHit(float radius)
{
	/**
	* Calculates the hit points of the asteroid based on its size.............
	*/
	int hit_points = 0;
	if (radius >= 1.0f && radius < 1.2f)
	{
		hit_points = 1;
	}
	else if (radius >= 1.2f && radius < 1.4f)
	{
		hit_points = 2;
	}
	else
	{
		hit_points = 3;
	}
	return hit_points;
}

void add_asteroid(int n)
{
	if ((astr_length + n) < 800 && (numOnScreenAstr() + n) <= 4)//Limits for generating asteroids both per screen and the memory...............
	{
		int i = 0;
		while (i < n)
		{
			astr[astr_length].transform = calcAstrPos(astr[astr_length].transform);
			astr[astr_length].radius = (float)((10 + (rand() % 6)) / 10.0f);
			astr[astr_length].velocity = (float)(5 + (rand() % 11));
			astr[astr_length].status = ALIVE;
			astr[astr_length].hit_points = calcHit(astr[astr_length].radius);
			astr[astr_length].angle = calcAngle(astr[astr_length].transform, pl_ship.transform);
			astr[astr_length].unit = calcUniVec(astr[astr_length].angle, astr[astr_length].unit);
			astr[astr_length] = calcAstrShape(astr[astr_length]);
			astr_length++;
			i++;
		}
	}
}

int numOnScreenAstr()
{
	/**
	* To Calculate the total number of Asteroids within the arena................
	*/
	int count = 0;
	for (int i = 0; i < astr_length; i++)
	{
		if (astr[i].status == ALIVE)
		{
			count++;
		}
	}
	return count;
}

void asteroid_attack(int n)
{
	/**
	* To set waves of Asteroid Attacks per time stamps.........
	*/
	if (aatime.wave_timer == n)
	{
		aatime.wave =  1 + (aatime.wave % 2); 
		/*
		*SETTING UP WAVE PARAMETERS (MAX ASTEROIDS PER WAVE IS 2,
	    *but onscreen it may vary depending on the asteroid starting location and velocity...............
		*/
		add_asteroid(aatime.wave);
		aatime.wave_timer = 0;
	}
	else
		aatime.wave_timer++;
}

void asteroid_rotation()
{
	for (int i = 0; i < astr_length; i++)
	{
		if (astr[i].status == ALIVE)
			astr[i].rotation += (150 * aatime.dt);
	}
}

vector Wbound(float radi, vector v)
{
	const float maxY = as_ar.h_border_limit + 1.0f;
	const float maxX = radi + 1.0f;
	const float minX = as_ar.w_border_limit + 1.0f;

	float x = minX + (rand() % (int)(maxX - minX));
	float y = rand() % (int)maxY;

	v.vx = x;
	v.vy = y;
	return v;
}

vector Hbound(float radi, vector v)
{
	const float maxX = as_ar.w_border_limit + 1.0f;
	const float maxY = radi + 1.0f;
	const float minY = as_ar.h_border_limit + 1.0f;

	float x = rand() % (int)maxX;
	float y = minY + (rand() % (int)(maxY - minY));

	v.vx = x;
	v.vy = y;
	return v;
}

vector calcAstrPos(vector v)
{
	/**
	* Generates the asteroid starting coordinate points outside the window frame................
	*/
	const float outer_radi = sqrt((as_ar.h_border_limit * as_ar.h_border_limit) + (as_ar.w_border_limit * as_ar.w_border_limit));
	//Radius for the outer screen bounding circle....

	const int range = 1 + rand() % 2;
	if (range == 1)
		v = Wbound(outer_radi, v);//launching from the width sides.....
	else
		v = Hbound(outer_radi, v);//launching from the height sides...........

	const int random = 1 + rand() % 4;

	if (random == 1)
	{
		v.vx *= 1;
		v.vy *= 1;
	}
	else if (random == 2)
	{
		v.vx *= -1;
		v.vy *= -1;
	}
	else if (random == 3)
	{
		v.vx *= -1;
		v.vy *= 1;
	}
	else
	{
		v.vx *= 1;
		v.vy *= -1;
	}

	return v;
}

/*Section 8************OTHER BASIC FUNCTIONS Including callbacks and game refreshing functions*********************/
float calcDeltaTime()
{
	float cur_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
	float dt = cur_time - g_last_time;
	g_last_time = cur_time;
	return dt;
}

void reset_ship()
{
	pl_ship.speedDown = pl_ship.speedUp = keyboard.toggle_weapon = false;
	pl_ship.transform.vx = pl_ship.transform.vy = pl_ship.velocity = pl_ship.angle = 0.0f;
	pl_ship.status = ALIVE;
	pl_ship.unit = calcUniVec(pl_ship.angle, pl_ship.unit);
	player.score = nearAstr = 0;
	aatime.mstimer = aatime.mtimer = aatime.offset = aatime.stimer = aatime.acTimer = aatime.decTimer = 0;
	bullet_length = 0;
	init_bullets();//RESETING ALL THE BULLET ELEMENTS BACK TO ITS DEFAULT VALUES
}

void end_app()
{
	exit(EXIT_SUCCESS);
}


void continue_game()
{
	if (pl_ship.status == DEAD)
	{

		if (keyboard.reset == true)
		{
			reset_ship();
			keyboard.reset = false;
		}
	}
}

void run_game()
{
	if (keyboard.start == true)
	{
		aatime.dt = calcDeltaTime();//calculating the delta time frame.....

		if (pl_ship.status == ALIVE)
		{
			asteroid_attack(4000);
			ship_controller();
			detect_collide_arena();
			detect_collide_astrship();
			calcGameTime();
		}

		continue_game();//Where you reset the spaceship........
		gun_activate();
		transform_astroid();
		asteroid_rotation();
		detect_collide_bullarena();
		detect_collide_astrbull();
		detect_collide_astrbounce();
		detect_collide_astrarena();
		nearAstr = calcNearAstr();//Tracks the nearest asteroid (index)..............
	}
}

void on_idle()
{
	run_game();
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
		as_ar.w_border_limit = (half_world_size-0.01);
		as_ar.h_border_limit = (half_world_size * aspect) - 0.01f;
	}
	else
	{
		float aspect = (float)w / (float)h;
		glOrtho(-half_world_size * aspect, half_world_size * aspect, -half_world_size, half_world_size, -half_world_size, half_world_size);
		as_ar.w_border_limit = (half_world_size * aspect) - 0.01f;
		as_ar.h_border_limit = (half_world_size - 0.01);
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void init_game(int* argcp, char** argv, window_t* mainwinp)
{
	// GLUT & OpenGL setup
	glutInit(argcp, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	// Display related setup
	glutInitWindowPosition(mainwinp->x_pos, mainwinp->y_pos);
	glutInitWindowSize(mainwinp->width, mainwinp->height);
	glutCreateWindow("<ASTEROID ARCADIUM>");

	if (mainwinp->is_fullscreen == true)
	{
		glutFullScreen();
	}

	glutDisplayFunc(on_display);
	glutReshapeFunc(on_reshape);
	glutKeyboardFunc(on_keyboard);
	glutKeyboardUpFunc(on_release);
	// Define the idle function
	glutIdleFunc(on_idle);
	g_last_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
}

void play_game()
{
	glutMainLoop();
}

void load_config(int* argcp, char** argv, window_t* mainwin_p)
{
	mainwin_p->width = 1024;
	mainwin_p->height = 768;
	mainwin_p->is_fullscreen = true;
}