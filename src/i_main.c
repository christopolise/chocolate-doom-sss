//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	Main program, simply calls D_DoomMain high level loop.
//

#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

#include "SDL.h"

#include "doomtype.h"
#include "i_system.h"
#include "m_argv.h"
#include "m_misc.h"

char *banana = "pickle";
void *video_stream_mem;
void *input_stream_mem;
void *output_stream_mem;

int shmid;
int shmid_input;
int shmid_output;

//
// D_DoomMain()
// Not a globally visible function, just included for source reference,
// calls all startup code, parses command line options.
//

void D_DoomMain(void);

int main(int argc, char **argv)
{
    // save arguments

    myargc = argc;
    myargv = malloc(argc * sizeof(char *));
    assert(myargv != NULL);

    // Video output shared memory buffer
    shmid = shmget((key_t)666, 320 * 200, 0666 | IPC_CREAT);
    video_stream_mem = shmat(shmid, NULL, 0);

    // Game input shared memory buffer
    shmid_input = shmget((key_t)667, 320 * 200, 0666 | IPC_CREAT);
    input_stream_mem = shmat(shmid_input, NULL, 0);

    // Game output shared memory buffer
    shmid_output = shmget((key_t)668, 320 * 200, 0666 | IPC_CREAT);
    output_stream_mem = shmat(shmid_output, NULL, 0);
    printf("Processes attached "
           "at:\n----------------------\nVideo:\t%p\nInput:\t%p\nOutput:\t%p\n",
           video_stream_mem, input_stream_mem, output_stream_mem);

    for (int i = 0; i < argc; i++)
    {
        myargv[i] = M_StringDuplicate(argv[i]);
    }

    //!
    // Print the program version and exit.
    //
    if (M_ParmExists("-version") || M_ParmExists("--version"))
    {
        puts(PACKAGE_STRING);
        exit(0);
    }

#if defined(_WIN32)
    // compose a proper command line from loose file paths passed as arguments
    // to allow for loading WADs and DEHACKED patches by drag-and-drop
    M_AddLooseFiles();
#endif

    M_FindResponseFile();
    M_SetExeDir();

#ifdef SDL_HINT_NO_SIGNAL_HANDLERS
    SDL_SetHint(SDL_HINT_NO_SIGNAL_HANDLERS, "1");
#endif

    // start doom

    D_DoomMain();

    return 0;
}
