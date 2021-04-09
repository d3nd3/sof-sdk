// qdata.h


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>

#include "cmdlib.h"
#include "scriplib.h"
#include "mathlib.h"
#include "lbmlib.h"
#include "threads.h"
#include "qfiles.h"

void Cmd_Load (void);

void Cmd_Grab (void);
void Cmd_Mip (void);

void Cmd_File (void);
void Cmd_Dir (void);
void Cmd_StartWad (void);
void Cmd_EndWad (void);
void Cmd_Mipdir (void);
void Cmd_Picdir (void);

void Cmd_TextureMix (void);
void Cmd_TMixdir ();

void ReleaseFile (char *filename);

extern	unsigned	*longimage;
extern	int			longimagewidth, longimageheight;

extern	qboolean	g_release;			// don't grab, copy output data to new tree
extern	char		g_releasedir[1024];	// c:\quake2\baseq2, etc

extern	qboolean	g_pak;				// if true, copy to pak instead of release
extern	char		g_only[256];		// if set, only grab this cd

extern	char		g_materialFile[256];

extern	unsigned	total_x;
extern	unsigned	total_y;
extern	unsigned	total_textures;

miptex32_t *CreateMip32(unsigned *data, unsigned width, unsigned height, int *FinalSize, qboolean mip);
