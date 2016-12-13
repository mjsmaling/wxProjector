#include "../include/globals.h"
#include <stdlib.h>

// Project app.
wxProjectorApp *g_project_app = 0;

// Songs maanger.
//wxSongsManager *g_songs_manager = 0;

// App Paths.
wxAppPaths *g_app_paths = 0;

// The show window. Should only be one instance of this.
wxShowWindow *g_show_window = 0;

int generate_random_number(int min, int max)
{
    return generate_random_number() * (max - min) + min;
}

float generate_random_number(void)
{
    return ((float) rand()) / ((float) RAND_MAX);
}
