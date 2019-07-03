#include <stdio.h>
#include <SDL_joystick.h>
#include <unistd.h>
//#include <SDL_gamecontroller.h>
#include "SDL.h"
#define MAX_CONTROLLERS 10

//Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 4000;

SDL_GameController *ControllerHandles[MAX_CONTROLLERS];

int xbox_init()
{
    //Initialization flag
    int success = 1;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = 0;
    }
	int ControllerIndex = 0;
    //Check for joysticks
    if( SDL_NumJoysticks() < 1 )
    {
        printf( "Warning: No joysticks connected!\n" );
    }
    else
    {
    	printf("Success! Number of joysticks: %d\n", SDL_NumJoysticks());
    	int MaxJoysticks = SDL_NumJoysticks();
		if (!SDL_IsGameController(ControllerIndex))
	    {
	        printf("Unexpected error\n");
	    }
	    ControllerHandles[ControllerIndex] = SDL_GameControllerOpen(ControllerIndex);
    }
    //int IsAPressed = SDL_GameControllerGetButton(Controller, SDL_CONTROLLER_BUTTON_A);
    //for (int ControllerIndex = 0;
    // ControllerIndex < MAX_CONTROLLERS;
    // ++ControllerIndex)
	int StickLX = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX);
	int StickLY = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY);
	//int StickRX = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTX);
	//int StickRY = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_RIGHTY);
	printf("%d %d\n", StickLX, StickLY);

	return 0;
}

int xbox_main()
{
	//printf("Main\n");
    //Event handler
    SDL_Event e;
    int x_value;
    int y_value;

    //Handle events on queue
    while( SDL_PollEvent( &e ) != 0 )
    {
    	if (e.jaxis.axis == 0)
    	{
    		x_value = e.jaxis.value;
    	}
    	if (e.jaxis.axis == 1)
    	{
    		y_value = e.jaxis.value;
    	}
        printf("%d %d\n", x_value, y_value);
    }
}

int main()
{
	int test1 = xbox_init();
	while (1)
	{
		int main_section = xbox_main();
	}
	return 0;
}