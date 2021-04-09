#include "qdata.h"
#include <windows.h>
#include <gl/gl.h>

extern char		*g_outputDir;

char		out_dir[1024];
char		mip_prefix[1024];		// directory to dump the textures in

unsigned		*longimage;
int				longimagewidth, longimageheight;

unsigned total_x = 0;
unsigned total_y = 0;
unsigned total_textures = 0;

#define MAX_IMAGE_SIZE 1024

unsigned		bufferl[MAX_IMAGE_SIZE*MAX_IMAGE_SIZE];
unsigned		scaled[MAX_IMAGE_SIZE*MAX_IMAGE_SIZE];
unsigned		out[MAX_IMAGE_SIZE*MAX_IMAGE_SIZE];

// ********************************************************************
// **  Mip Map Pre-Processing Routines
// ********************************************************************

void GL_ResampleTexture (unsigned *in, int inwidth, int inheight, unsigned *out,  int outwidth, int outheight)
{
	int		i, j;
	unsigned	*inrow, *inrow2;
	unsigned	frac, in_width_lots_of_out_widths;
	unsigned	p1[1024], p2[1024];
	byte		*pix1, *pix2, *pix3, *pix4;

	// represent 1 output_pixel in terms of 16 bit integer
	in_width_lots_of_out_widths = inwidth*0x10000/outwidth;

	// GO TO 1 QUARTER OF THIS  X/4
	frac = in_width_lots_of_out_widths>>2;
	for (i=0 ; i<outwidth ; i++)
	{
		// MULTIPLY BY 4 CANCELS OUT THE DIVIDE?
		p1[i] = 4*(frac>>16);
		// ADD ENTIRE WIDTHS WORTH
		frac += in_width_lots_of_out_widths;
	}
	// GO TO 3 QUARTERS OF THIS 3 * X/4
	frac = 3*(in_width_lots_of_out_widths>>2);
	for (i=0 ; i<outwidth ; i++)
	{
		// MULTIPLY BY 4 CANCELS OUT THE DIVIDE?
		p2[i] = 4*(frac>>16);
		frac += in_width_lots_of_out_widths;
	}

	for (i=0 ; i<outheight ; i++, out += outwidth)
	{
		inrow = in + inwidth*(int)((i+0.25)*inheight/outheight);
		inrow2 = in + inwidth*(int)((i+0.75)*inheight/outheight);
		frac = in_width_lots_of_out_widths >> 1;
		for (j=0 ; j<outwidth ; j++)
		{
			pix1 = (byte *)inrow + p1[j];
			pix2 = (byte *)inrow + p2[j];
			pix3 = (byte *)inrow2 + p1[j];
			pix4 = (byte *)inrow2 + p2[j];
			((byte *)(out+j))[0] = (pix1[0] + pix2[0] + pix3[0] + pix4[0])>>2;
			((byte *)(out+j))[1] = (pix1[1] + pix2[1] + pix3[1] + pix4[1])>>2;
			((byte *)(out+j))[2] = (pix1[2] + pix2[2] + pix3[2] + pix4[2])>>2;
			((byte *)(out+j))[3] = (pix1[3] + pix2[3] + pix3[3] + pix4[3])>>2;
		}
	}
}

byte *PixelAddr(byte *in, int width, int height, int x, int y)
{
	return in+((y%height)*width+(x%width))*4;
}

void GL_MipMap (byte *out, byte *in, int width, int height)
{
	int		i, j, k, l;
	byte	*src;
	byte	*dest;
	int		newwidth,newheight;
	int		temp[4];

	newwidth=width>>1;
	if (newwidth<1)
		newwidth=1;
	newheight=height>>1;
	if (newheight<1)
		newheight=1;

	for (i=0;i<newheight;i++)
	{
		for (j=0;j<newwidth;j++)
		{
			for (k=0;k<4;k++)
				temp[k]=0;
			for (l=0;l<4;l++)
			{
				src=PixelAddr(in,width,height,2*j+(l&1),2*i+(l>>1));
				for (k=0;k<4;k++)
				{
					temp[k]+=(int)src[k];
				}
			}						
			dest=PixelAddr(out,newwidth,newheight,j,i);
			for (k=0;k<4;k++)
				dest[k]=(byte)((temp[k]+2)>>2);
		}
	}
}

miptex32_t *CreateMip32(unsigned *data, unsigned width, unsigned height, int *FinalSize, qboolean mip)
{
	int				scaled_width, scaled_height;
	int				miplevel;
	miptex32_t		*mp;
	byte			*pos;
	int				size;
	paletteRGBA_t	*test;

	for (scaled_width = 1 ; scaled_width < width ; scaled_width<<=1)
		;
	if (1 && scaled_width > width && 1)
		scaled_width >>= 1;
	for (scaled_height = 1 ; scaled_height < height ; scaled_height<<=1)
		;
	if (1 && scaled_height > height && 1)
		scaled_height >>= 1;

	// don't ever bother with >256 textures
	if (scaled_width > MAX_IMAGE_SIZE)
		scaled_width = MAX_IMAGE_SIZE;
	if (scaled_height > MAX_IMAGE_SIZE)
		scaled_height = MAX_IMAGE_SIZE;

	if (scaled_width < 1)
		scaled_width = 1;
	if (scaled_height < 1)
		scaled_height = 1;

	size = sizeof(*mp) + (scaled_width*scaled_height*3*4);
	mp = (miptex32_t *)SafeMalloc(size, "CreateMip");
	memset(mp,0,size);

	mp->version = MIP32_VERSION;

	size = width*height;
	test = (paletteRGBA_t *)data;
	while(size)
	{
		if (test->a != 255)
		{
			mp->flags |= LittleLong(SURF_ALPHA_TEXTURE);
			break;
		}

		size--;
		test++;
	}

	if (scaled_width == width && scaled_height == height)
	{
		memcpy (scaled, data, width*height*4);
	}
	else
		GL_ResampleTexture (data, width, height, scaled, scaled_width, scaled_height);

	pos = (byte *)(mp + 1);
	miplevel = 0;


	while ((!miplevel || scaled_width > 1 || scaled_height > 1) && (miplevel <= MIPLEVELS-1))
	{
		if (miplevel > 0)
		{
			GL_MipMap((byte *)out, (byte *)scaled, scaled_width, scaled_height);
			scaled_width >>= 1;
			scaled_height >>= 1;
			if (scaled_width < 1)
				scaled_width = 1;
			if (scaled_height < 1)
				scaled_height = 1;
		}
		else
		{
			memcpy(out, scaled, MAX_IMAGE_SIZE * MAX_IMAGE_SIZE * 4);
		}


		mp->width[miplevel] = scaled_width;
		mp->height[miplevel] = scaled_height;
		mp->offsets[miplevel] = pos - ((byte *)(mp));
		memcpy(pos, out, scaled_width * scaled_height * 4);
		memcpy(scaled, out, MAX_IMAGE_SIZE * MAX_IMAGE_SIZE * 4);
		pos += scaled_width * scaled_height * 4;

		// If no mipping specified, and texture size is always valid, break out
		if(!mip && (scaled_width <= 256) && (scaled_height <= 256))
		{
			break;
		}

		miplevel++;
	}

	*FinalSize = pos - ((byte *)(mp));

	return mp;
}

/*
=============================================================================

MIPTEX GRABBING

=============================================================================
*/

typedef enum
{
	pt_contents,
	pt_flags,
	pt_animvalue,
	pt_altnamevalue,
	pt_damagenamevalue,
	pt_flagvalue,
	pt_materialvalue,
	pt_scale,
	pt_mip,
	pt_detail,
	pt_gl,
	pt_nomip,
	pt_detailer,
	pt_parental,
	pt_spherical,
	pt_sphericaler,
} parmtype_t;

typedef struct
{
	char	*name;
	int		flags;
	parmtype_t	type;
} mipparm_t;

mipparm_t	mipparms[] =
{
	// utility content attributes
	{"pushpull",CONTENTS_PUSHPULL, pt_contents},
	{"water",	CONTENTS_WATER, pt_contents},
	{"slime",	CONTENTS_SLIME, pt_contents},		// mildly damaging
	{"lava",	CONTENTS_LAVA, pt_contents},		// very damaging
	{"window",	CONTENTS_WINDOW, pt_contents},	// solid, but doesn't eat internal textures
	{"mist",	CONTENTS_MIST, pt_contents},	// non-solid window
	{"origin",	CONTENTS_ORIGIN, pt_contents},	// center of rotating brushes
	{"playerclip",	CONTENTS_PLAYERCLIP, pt_contents},
	{"monsterclip",	CONTENTS_MONSTERCLIP, pt_contents},

	// utility surface attributes
	{"hint",	SURF_HINT, pt_flags},
	{"skip",	SURF_SKIP, pt_flags},
	{"light",	SURF_LIGHT, pt_flagvalue},		// value is the light quantity

	{"animspeed",SURF_ANIMSPEED, pt_flagvalue},		// value will hold the anim speed in fps

	// texture chaining
	{"anim",	0,			pt_animvalue},		// animname is the next animation
	{"alt",		0,			pt_altnamevalue},	// altname is the alternate texture
	{"damage",	0,			pt_damagenamevalue},	// damagename is the damage texture
	{"scale",	0,			pt_scale},		// next two values are for scale
	{"mip",		0,			pt_mip},		
	{"detail",	0,			pt_detail},		
	{"spherical", 0,		pt_spherical},

	{"GL_ZERO",					GL_ZERO,				pt_gl},
	{"GL_ONE",					GL_ONE,					pt_gl},
	{"GL_SRC_COLOR",			GL_SRC_COLOR,			pt_gl},
	{"GL_ONE_MINUS_SRC_COLOR",	GL_ONE_MINUS_SRC_COLOR,	pt_gl},
	{"GL_DST_COLOR",			GL_DST_COLOR,			pt_gl},
	{"GL_ONE_MINUS_DST_COLOR",	GL_ONE_MINUS_DST_COLOR,	pt_gl},
	{"GL_SRC_ALPHA",			GL_SRC_ALPHA,			pt_gl},
	{"GL_ONE_MINUS_SRC_ALPHA",	GL_ONE_MINUS_SRC_ALPHA,	pt_gl},
	{"GL_DST_ALPHA",			GL_DST_ALPHA,			pt_gl},
	{"GL_ONE_MINUS_DST_ALPHA",	GL_ONE_MINUS_DST_ALPHA,	pt_gl},
	{"GL_SRC_ALPHA_SATURATE",	GL_SRC_ALPHA_SATURATE,	pt_gl},

	// server attributes
	{"slick",	SURF_SLICK, pt_flags},

	// drawing attributes
	{"sky",		SURF_SKY, pt_flags},
	{"warping",	SURF_WARP, pt_flags},		// only valid with 64x64 textures
	{"trans33",	SURF_TRANS33, pt_flags},	// translucent should allso set fullbright
	{"trans66",	SURF_TRANS66, pt_flags},
	{"flowing",	SURF_FLOWING, pt_flags},	// flow direction towards angle 0
	{"nodraw",	SURF_NODRAW, pt_flags},	// for clip textures and trigger textures
	{"alpha",	SURF_ALPHA_TEXTURE, pt_flags},
	{"undulate",	SURF_UNDULATE, pt_flags},		// rock surface up and down...
	{"skyreflect",	SURF_SKYREFLECT, pt_flags},		// liquid will somewhat reflect the sky - not quite finished....

	{"material", SURF_MATERIAL, pt_materialvalue},
	{"metal",	SURF_TYPE_METAL, pt_flags},
	{"stone",	SURF_TYPE_STONE, pt_flags},
	{"wood",	SURF_TYPE_WOOD, pt_flags},

	{"m_nomip", 0, pt_nomip},
	{"m_detail", 0, pt_detailer},
	{"m_spherical", 0, pt_sphericaler},
	{"m_parental", 0, pt_parental},

	{NULL, 0, pt_contents}
};

void CopyStringRemoveExt(char *dst, char *src, char *ext)
{
	strcpy(dst, src);
	strlwr(dst);
	strlwr(ext);
	// If extension - remove it
	if(strstr(dst, ext))
	{
		dst[strlen(dst) - 4] = 0;
	}
}

/*
==============
Cmd_Mip

$mip filename x y width height <OPTIONS>
must be multiples of sixteen
SURF_WINDOW
==============
*/

void Cmd_Mip (void)
{
	int             xl,yl,xh,yh,w,h;
	int				flags, value, contents;
	mipparm_t		*mp;
	char			lumpname[128];
	char			altname[128];
	char			animname[128];
	char			damagename[128];
	float			damage_health;
	materialtype_t	*mat;
	char			filename[1024];
	unsigned        *destl, *sourcel;
	int             linedelta, x, y;
	int				size;
	miptex32_t		*qtex32;
	float			scale_x, scale_y;
	int				mip_scale;
	// detail texturing
	char			dt_name[128];
	float			dt_scale_x, dt_scale_y;
	float			dt_u, dt_v;
	float			dt_alpha;
	int				dt_src_blend_mode, dt_dst_blend_mode;
	int				flags2;


	GetScriptToken (false);
	CopyStringRemoveExt(lumpname, token, ".m32");
	
	GetScriptToken (false);
	xl = atoi (token);
	GetScriptToken (false);
	yl = atoi (token);
	GetScriptToken (false);
	w = atoi (token);
	GetScriptToken (false);
	h = atoi (token);

	total_x += w;
	total_y += h;
	total_textures++;

	if ( (w & 7) || (h & 7) )
		Error ("line %i: miptex sizes must be multiples of 8", scriptline);

	flags = 0;
	flags2 = 0;
	contents = 0;
	value = 0;
	mip_scale = 0;

	altname[0] = animname[0] = damagename[0] = 0;

	damage_health = 0.0;
	scale_x = scale_y = 0.5;

	// detail texturing
	dt_name[0] = 0;
	dt_scale_x = dt_scale_y = 0.0;
	dt_u = dt_v = 0.0;
	dt_alpha = 0.0;
	dt_src_blend_mode = dt_dst_blend_mode = 0;

	// get optional flags and values
	while (ScriptTokenAvailable ())
	{
		GetScriptToken (false);
	
		for (mp=mipparms ; mp->name ; mp++)
		{
			if (!strcmp(mp->name, token))
			{
				switch (mp->type)
				{
					case pt_animvalue:
						GetScriptToken (false);	// specify the next animation frame
						CopyStringRemoveExt(animname, token, ".m32");
						break;
					case pt_altnamevalue:
						GetScriptToken (false);	// specify the alternate texture
						CopyStringRemoveExt(altname, token, ".m32");
						break;
					case pt_damagenamevalue:
						GetScriptToken (false);		// specify the damage texture
						CopyStringRemoveExt(damagename, token, ".m32");
						GetScriptToken (false);		// specify the damage health
						damage_health = atof(token);
						break;
					case pt_flags:
						flags |= mp->flags;
						break;
					case pt_contents:
						contents |= mp->flags;
						break;
					case pt_flagvalue:
						flags |= mp->flags;
						GetScriptToken (false);	// specify the light value
						value = atoi(token);
						break;
					case pt_materialvalue:
						GetScriptToken(false);
						for (mat=materialtypes ; mat->name ; mat++)
						{
							if (!strcmp(mat->name, token))
							{
								// assumes SURF_MATERIAL is in top 8 bits
								flags = (flags & 0x0FFFFFF) | (mat->value << 24);
								break;
							}
						}
						break;
					case pt_scale:
						GetScriptToken (false);	// specify the x scale
						scale_x = atof(token);
						GetScriptToken (false);	// specify the y scale
						scale_y = atof(token);
						break;

					case pt_mip:
						mip_scale = 1;
						break;

					case pt_detailer:
						flags2 |= MIP32_DETAILER_FLAG2;
						break;

					case pt_sphericaler:
						flags2 |= MIP32_SPHERICAL_FLAG2;
						break;

					case pt_nomip:
						flags2 |= MIP32_NOMIP_FLAG2;
						break;

					case pt_parental:
						flags2 |= MIP32_PARENTAL_FLAG2;
						break;

					case pt_spherical:
						GetScriptToken(false);
						CopyStringRemoveExt(dt_name, token, ".m32");
						break;

					case pt_detail:
						GetScriptToken(false);
						CopyStringRemoveExt(dt_name, token, ".m32");
						GetScriptToken(false);
						dt_scale_x = atof(token);
						GetScriptToken(false);
						dt_scale_y = atof(token);
						GetScriptToken(false);
						dt_u = atof(token);
						GetScriptToken(false);
						dt_v = atof(token);
						GetScriptToken(false);
						dt_alpha = atof(token);
						GetScriptToken(false);
						for (mp=mipparms ; mp->name ; mp++)
						{
							if (!strcmp(mp->name, token))
							{
								if (mp->type == pt_gl)
								{
									dt_src_blend_mode = mp->flags;
									break;
								}
							}
						}
						if (!mp->name)
						{
							Error ("line %i: invalid gl blend mode %s", scriptline, token);
						}
						GetScriptToken (false);
						for (mp=mipparms ; mp->name ; mp++)
						{
							if (!strcmp(mp->name, token))
							{
								if (mp->type == pt_gl)
								{
									dt_dst_blend_mode = mp->flags;
									break;
								}
							}
						}
						if (!mp->name)
						{
							Error ("line %i: invalid gl blend mode %s", scriptline, token);
						}
						break;
				}
				break;
			}
		}
		if (!mp->name)
			Error ("line %i: unknown parm %s", scriptline, token);
	}

	if (g_release)
	{
		return;	// textures are only released by $maps
	}

	xh = xl+w;
	yh = yl+h;
	if (xh*yh > MAX_IMAGE_SIZE*MAX_IMAGE_SIZE)
	{
		Error("line %i image %s: image is too big!", scriptline, lumpname);
	}
		
	if (xl >= longimagewidth || xh > longimagewidth ||
		yl >= longimageheight || yh > longimageheight)
	{
		Error ("line %i image %s: bad clip dimmensions (%d,%d) (%d,%d) > image (%d,%d)", scriptline, lumpname, xl,yl,w,h,longimagewidth,longimageheight);
	}

	sourcel = longimage + (yl*longimagewidth) + xl;
	destl = bufferl;
	linedelta = (longimagewidth - w);

	for (y=yl ; y<yh ; y++)
	{
		for (x=xl ; x<xh ; x++)
		{
			*destl++ = *sourcel++;	// RGBA
		}
		sourcel += linedelta;
	}

	qtex32 = CreateMip32(bufferl, w, h, &size, !(flags2 & MIP32_NOMIP_FLAG2));

	qtex32->flags |= LittleLong(flags);
	qtex32->flags2 |= LittleLong(flags2);
	qtex32->contents = LittleLong(contents);
	qtex32->value = LittleLong(value);
	qtex32->scale_x = scale_x;
	qtex32->scale_y = scale_y;
	qtex32->mip_scale = mip_scale;
	sprintf (qtex32->name, "%s/%s", mip_prefix, lumpname);
	if (animname[0])
	{
		sprintf (qtex32->animname, "%s/%s", mip_prefix, animname);
	}
	if (altname[0])
	{
		sprintf (qtex32->altname, "%s/%s", mip_prefix, altname);
	}
	if (damagename[0])
	{
		sprintf (qtex32->damagename, "%s/%s", mip_prefix, damagename);
		qtex32->damage_health = damage_health;
	}
	if (dt_name[0] && ((flags2 & (MIP32_DETAILER_FLAG2|MIP32_SPHERICAL_FLAG2)) == 0))
	{
		sprintf (qtex32->dt_name, "%s/%s", mip_prefix, dt_name);
		qtex32->dt_scale_x = dt_scale_x;
		qtex32->dt_scale_y = dt_scale_y;
		qtex32->dt_u = dt_u;
		qtex32->dt_v = dt_v;
		qtex32->dt_alpha = dt_alpha;
		qtex32->dt_src_blend_mode = dt_src_blend_mode;
		qtex32->dt_dst_blend_mode = dt_dst_blend_mode;
	}
	
//
// write it out
//
	sprintf (filename, "%s/%s/%s.m32", out_dir, mip_prefix, lumpname);
	if(qtex32->flags & (SURF_ALPHA_TEXTURE))
		printf ("writing %s with ALPHA\n", filename);
	else
		printf ("writing %s\n", filename);
	SaveFile (filename, (byte *)qtex32, size);

	free (qtex32);
}

/*
===============
Cmd_Mipdir
===============
*/
void Cmd_Mipdir (void)
{
	char	filename[1024];

	GetScriptToken (false);
	strcpy (mip_prefix, token);
	// create the directory if needed
	sprintf (filename, "%stextures", g_outputDir);
	Q_mkdir (filename); 
	strcpy(out_dir, filename);
	sprintf (filename, "%stextures/%s", g_outputDir, mip_prefix);
	Q_mkdir (filename); 
}

/*
===============
Cmd_picdir
===============
*/
void Cmd_Picdir (void)
{
	char	filename[1024];

	GetScriptToken (false);
	strcpy (mip_prefix, token);
	// create the directory if needed
	sprintf (filename, "%s", g_outputDir);
	Q_mkdir (filename); 
	strcpy(out_dir, filename);
	sprintf (filename, "%s/%s", g_outputDir, mip_prefix);
	Q_mkdir (filename); 
}

/*
===============
Cmd_Load
===============
*/
void Cmd_Load (void)
{
	char	*name;

	GetScriptToken (false);

	if (g_release)
		return;

	name = ExpandPathAndArchive(token);

	// load the image
	printf ("loading %s\n", name);
	if(!LoadAnyImage (name, &longimage, &longimagewidth, &longimageheight))
	{
		Error ("Texture must be true colour");
	}
}

