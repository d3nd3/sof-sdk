
#include "q_Typedef.h"

//
// qfiles.h: quake file formats
// This file must be identical in the quake and utils directories
//

/*
========================================================================

The .pak files are just a linear collapse of a directory tree

========================================================================
*/

#define IDPAKHEADER		(('K'<<24)+('C'<<16)+('A'<<8)+'P')

typedef struct
{
	char	name[56];
	int		filepos, filelen;
} dpackfile_t;

typedef struct
{
	int		ident;		// == IDPAKHEADER
	int		dirofs;
	int		dirlen;
} dpackheader_t;

#define	MAX_FILES_IN_PACK	4096


/*
========================================================================

PCX files are used for as many images as possible

========================================================================
*/

typedef struct
{
    char	manufacturer;
    char	version;
    char	encoding;
    char	bits_per_pixel;
    unsigned short	xmin,ymin,xmax,ymax;
    unsigned short	hres,vres;
    unsigned char	palette[48];
    char	reserved;
    char	color_planes;
    unsigned short	bytes_per_line;
    unsigned short	palette_type;
    char	filler[58];
    unsigned char	data;			// unbounded
} pcx_t;


/*
========================================================================

.MD2 compressed triangle model file format

========================================================================
*/
#define IDJOINTEDALIASHEADER	(('2'<<24)+('J'<<16)+('D'<<8)+'I')

/*
========================================================================

.MD2 triangle model file format

========================================================================
*/

#define IDALIASHEADER		(('2'<<24)+('P'<<16)+('D'<<8)+'I')
#define ALIAS_VERSION	8

#define	MAX_TRIANGLES	2048
#define MAX_VERTS		2048
#define MAX_FRAMES		512
#define MAX_MD2SKINS	64
#define	MAX_SKINNAME	64

typedef struct
{
	short	s;
	short	t;
} dstvert_t;

typedef struct 
{
	short	index_xyz[3];
	short	index_st[3];
} dtriangle_t;

typedef struct
{
	byte	v[3];			// scaled byte to fit in frame mins/maxs
	byte	lightnormalindex;
} dtrivertx_t;

#define DTRIVERTX_V0   0
#define DTRIVERTX_V1   1
#define DTRIVERTX_V2   2
#define DTRIVERTX_LNI  3
#define DTRIVERTX_SIZE 4

typedef struct
{
	float		scale[3];	// multiply byte verts by this
	float		translate[3];	// then add this
	char		name[16];	// frame name from grabbing
	dtrivertx_t	verts[1];	// variable sized
} daliasframe_t;


// the glcmd format:
// a positive integer starts a tristrip command, followed by that many
// vertex structures.
// a negative integer starts a trifan command, followed by -x vertexes
// a zero indicates the end of the command list.
// a vertex consists of a floating point s, a floating point t,
// and an integer vertex index.


typedef struct
{
	int			ident;
	int			version;

	int			skinwidth;
	int			skinheight;
	int			framesize;		// byte size of each frame

	int			num_skins;
	int			num_xyz;
	int			num_st;			// greater than num_xyz for seams
	int			num_tris;
	int			num_glcmds;		// dwords in strip/fan command list
	int			num_frames;

	int			ofs_skins;		// each skin is a MAX_SKINNAME string
	int			ofs_st;			// byte offset from start for stverts
	int			ofs_tris;		// offset for dtriangles
	int			ofs_frames;		// offset for first frame
	int			ofs_glcmds;	
	int			ofs_end;		// end of file

} dmdl_t;

/*
========================================================================

.BK file format

========================================================================
*/

#define IDBOOKHEADER	(('K'<<24)+('O'<<16)+('O'<<8)+'B')
#define BOOK_VERSION	2

typedef struct bookframe_s
{
	int			x;
	int			y;
	int			w;
	int			h;
	char		name[MAX_SKINNAME];		// name of gfx file
} bookframe_t;

typedef struct bookheader_s
{
	unsigned int	ident;
	unsigned int	version;
	int				num_segments;
	int				total_w;
	int				total_h;
} bookheader_t;

typedef struct book_s
{
	bookheader_t bheader;
	bookframe_t	bframes[MAX_MD2SKINS];
} book_t;

/*
========================================================================

.SP2 sprite file format

========================================================================
*/

#define IDSPRITEHEADER	(('2'<<24)+('S'<<16)+('D'<<8)+'I')
		// little-endian "IDS2"
#define SPRITE_VERSION	2

typedef struct
{
	int		width, height;
	int		origin_x, origin_y;		// raster coordinates inside pic
	char	name[MAX_SKINNAME];		// name of pcx file
} dsprframe_t;

typedef struct {
	int			ident;
	int			version;
	int			numframes;
	dsprframe_t	frames[1];			// variable sized
} dsprite_t;

/*
==============================================================================

  .M8 texture file format

==============================================================================
*/

typedef struct palette_s
{
	union
	{
		struct
		{
			byte r,g,b;
		};
	};
} palette_t;

#define MIP_VERSION		2
#define PAL_SIZE		256
#define	MIPLEVELS		16

typedef struct miptex_s
{
	int			version;
	char		name[32];
	unsigned	width[MIPLEVELS], height[MIPLEVELS];
	unsigned	offsets[MIPLEVELS];		// four mip maps stored
	char		animname[32];			// next frame in animation chain
	palette_t	palette[PAL_SIZE];
	int			flags;
	int			contents;
	int			value;
} miptex_t;


#define MIP32_VERSION	4

// SOF Specific
#define MIP32_NOMIP_FLAG2			0x00000001
#define MIP32_DETAILER_FLAG2		0x00000002
#define MIP32_PARENTAL_FLAG2		0x00000004
#define MIP32_SPHERICAL_FLAG2		0x00000008

typedef struct miptex32_s
{
	int			version;
	char		name[128];
	char		altname[128];			// texture substitution
	char		animname[128];			// next frame in animation chain
	char		damagename[128];		// image that should be shown when damaged
	unsigned	width[MIPLEVELS], height[MIPLEVELS];
	unsigned	offsets[MIPLEVELS];		
	int			flags;
	int			contents;
	int			value;
	float		scale_x, scale_y;
	int			mip_scale;

	// detail texturing info
	char		dt_name[128];		// detailed texture name
	float		dt_scale_x, dt_scale_y;
	float		dt_u, dt_v;
	float		dt_alpha;
	int			dt_src_blend_mode, dt_dst_blend_mode;

	int			flags2;
	float		damage_health;

	int			unused[18];				// future expansion to maintain compatibility with h2
} miptex32_t;



/*
==============================================================================

  .BSP file format

==============================================================================
*/

#define IDBSPHEADER	(('P'<<24)+('S'<<16)+('B'<<8)+'I')
		// little-endian "IBSP"

#define BSPVERSION	38


// upper design bounds
// leaffaces, leafbrushes, planes, and verts are still bounded by
// 16 bit short limits
#define	MAX_MAP_MODELS		1024
#define	MAX_MAP_BRUSHES		8192
#define	MAX_MAP_ENTITIES	2048
#define	MAX_MAP_ENTSTRING	0x40000
#define	MAX_MAP_TEXINFO		8192

#define	MAX_MAP_AREAS		256
#define	MAX_MAP_AREAPORTALS	1024
#define	MAX_MAP_PLANES		65536
#define	MAX_MAP_NODES		65536
#define	MAX_MAP_BRUSHSIDES	65536
#define	MAX_MAP_LEAFS		65536
#define	MAX_MAP_VERTS		65536
#define	MAX_MAP_FACES		65536
#define	MAX_MAP_LEAFFACES	65536
#define	MAX_MAP_LEAFBRUSHES 65536
#define	MAX_MAP_PORTALS		65536
#define	MAX_MAP_EDGES		128000
#define	MAX_MAP_SURFEDGES	256000
#define	MAX_MAP_LIGHTING	0x200000
#define	MAX_MAP_VISIBILITY	0x180000

// key / value pair sizes

#define	MAX_KEY		32
#define	MAX_VALUE	1024

//=============================================================================

typedef struct
{
	int		fileofs, filelen;
} lump_t;

#define	LUMP_ENTITIES		0
#define	LUMP_PLANES			1
#define	LUMP_VERTEXES		2
#define	LUMP_VISIBILITY		3
#define	LUMP_NODES			4
#define	LUMP_TEXINFO		5
#define	LUMP_FACES			6
#define	LUMP_LIGHTING		7
#define	LUMP_LEAFS			8
#define	LUMP_LEAFFACES		9
#define	LUMP_LEAFBRUSHES	10
#define	LUMP_EDGES			11
#define	LUMP_SURFEDGES		12
#define	LUMP_MODELS			13
#define	LUMP_BRUSHES		14
#define	LUMP_BRUSHSIDES		15
#define	LUMP_POP			16
#define	LUMP_AREAS			17
#define	LUMP_AREAPORTALS	18
#define	HEADER_LUMPS		19

typedef struct
{
	int			ident;
	int			version;	
	lump_t		lumps[HEADER_LUMPS];
} dheader_t;

typedef struct
{
	float		mins[3], maxs[3];
	float		origin[3];		// for sounds or lights
	int			headnode;
	int			firstface, numfaces;	// submodels just draw faces
										// without walking the bsp tree
} dmodel_t;


typedef struct
{
	float	point[3];
} dvertex_t;


// 0-2 are axial planes
#define	PLANE_X			0
#define	PLANE_Y			1
#define	PLANE_Z			2

// 3-5 are non-axial planes snapped to the nearest
#define	PLANE_ANYX		3
#define	PLANE_ANYY		4
#define	PLANE_ANYZ		5

// planes (x&~1) and (x&~1)+1 are allways opposites

typedef struct
{
	float	normal[3];
	float	dist;
	int		type;		// PLANE_X - PLANE_ANYZ ?remove? trivial to regenerate
} dplane_t;


// contents flags are seperate bits
// a given brush can contribute multiple content bits
// multiple brushes can be in a single leaf

// these definitions also need to be in q_shared.h!

// lower bits are stronger, and will eat weaker brushes completely
#define	CONTENTS_SOLID			0x00000001		// an eye is never valid in a solid
#define	CONTENTS_WINDOW			0x00000002		// translucent, but not watery
#define	CONTENTS_PUSHPULL		0x00000004
#define	CONTENTS_LAVA			0x00000008
#define	CONTENTS_SLIME			0x00000010
#define	CONTENTS_WATER			0x00000020
#define	CONTENTS_MIST			0x00000040		// 64
#define	LAST_VISIBLE_CONTENTS	64				// this one worries me a bit JKH

// remaining contents are non-visible, and don't eat brushes

#define	CONTENTS_AREAPORTAL		0x00008000

#define	CONTENTS_PLAYERCLIP		0x00010000
#define	CONTENTS_MONSTERCLIP	0x00020000

// currents can be added to any other contents, and may be mixed
#define	CONTENTS_CURRENT_0		0x00040000
#define	CONTENTS_CURRENT_90		0x00080000
#define	CONTENTS_CURRENT_180	0x00100000
#define	CONTENTS_CURRENT_270	0x00200000
#define	CONTENTS_CURRENT_UP		0x00400000
#define	CONTENTS_CURRENT_DOWN	0x00800000

#define	CONTENTS_ORIGIN			0x01000000	// removed before bsping an entity

#define	CONTENTS_MONSTER		0x02000000	// should never be on a brush, only in game
#define	CONTENTS_DEADMONSTER	0x04000000
#define	CONTENTS_DETAIL			0x08000000	// brushes to be added after vis leafs
#define	CONTENTS_TRANSLUCENT	0x10000000	// auto set if any surface has trans
#define	CONTENTS_LADDER			0x20000000



#define	SURF_LIGHT				0x00000001		// value will hold the light strength

#define	SURF_SLICK				0x00000002		// effects game physics

#define	SURF_SKY				0x00000004		// don't draw, but add to skybox
#define	SURF_WARP				0x00000008		// turbulent water warp
#define	SURF_TRANS33			0x00000010
#define	SURF_TRANS66			0x00000020
#define	SURF_FLOWING			0x00000040	// scroll towards angle
#define	SURF_NODRAW				0x00000080	// don't bother referencing the texture

#define	SURF_HINT				0x00000100	// make a primary bsp splitter
#define	SURF_SKIP				0x00000200	// completely ignore, allowing non-closed brushes

#define	SURF_ALPHA_TEXTURE		0x00000800	// texture has alpha in it, and should show through in bsp process
#define	SURF_ANIMSPEED			0x00001000		// value will hold the anim speed in fps

#define SURF_UNDULATE			0x00002000	// rock surface up and down...
#define SURF_SKYREFLECT			0x00004000	// liquid will somewhat reflect the sky - not quite finished....

#define SURF_TYPE_GRAVEL		0x00000000
#define SURF_TYPE_METAL			0x01000000
#define SURF_TYPE_STONE			0x02000000
#define SURF_TYPE_WOOD			0x03000000
#define	SURF_MATERIAL			0xFF000000



typedef struct
{
	int			planenum;
	int			children[2];	// negative numbers are -(leafs+1), not nodes
	short		mins[3];		// for frustom culling
	short		maxs[3];
	unsigned short	firstface;
	unsigned short	numfaces;	// counting both sides
} dnode_t;


typedef struct texinfo_s
{
	float		vecs[2][4];		// [s/t][xyz offset]
	int			flags;			// miptex flags + overrides
	int			value;			// light emission, etc
	char		texture[32];	// texture name (textures/*.wal)
	int			nexttexinfo;	// for animations, -1 = end of chain
} texinfo_t;


// note that edge 0 is never used, because negative edge nums are used for
// counterclockwise use of the edge in a face
typedef struct
{
	unsigned short	v[2];		// vertex numbers
} dedge_t;

typedef struct
{
	int				contents;			// OR of all brushes (not needed?)

	short			cluster;
	short			area;

	short			mins[3];			// for frustum culling
	short			maxs[3];

	unsigned short	firstleafface;
	unsigned short	numleaffaces;

	unsigned short	firstleafbrush;
	unsigned short	numleafbrushes;
} dleaf_t;

typedef struct
{
	unsigned short	planenum;		// facing out of the leaf
	short	texinfo;
} dbrushside_t;

typedef struct
{
	int			firstside;
	int			numsides;
	int			contents;
} dbrush_t;

#define	ANGLE_UP	-1
#define	ANGLE_DOWN	-2


// the visibility lump consists of a header with a count, then
// byte offsets for the PVS and PHS of each cluster, then the raw
// compressed bit vectors
#define	DVIS_PVS	0
#define	DVIS_PHS	1
typedef struct
{
	int			numclusters;
	int			bitofs[8][2];	// bitofs[numclusters][2]
} dvis_t;

// each area has a list of portals that lead into other areas
// when portals are closed, other areas may not be visible or
// hearable even if the vis info says that it should be
typedef struct
{
	int		portalnum;
	int		otherarea;
} dareaportal_t;


typedef struct
{
	int		numareaportals;
	int		firstareaportal;
} darea_t;

typedef struct
{
	char	*name;
	int		value;
} materialtype_t;

enum
{
	MATERIAL_GRAVEL,
	MATERIAL_METAL,
	MATERIAL_STONE,
	MATERIAL_WOOD,
};

materialtype_t	*materialtypes;

void QFile_ReadMaterialTypes(char* filename);

