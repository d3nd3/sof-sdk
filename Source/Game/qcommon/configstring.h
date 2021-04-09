#pragma once

//
// config strings are a general means of communication from
// the server to all connected clients.
// Each config string can be at most MAX_QPATH characters.
//
#define	CS_NAME				0
#define	CS_CDTRACK			1
#define	CS_SKY				2
#define	CS_SKYAXIS			3		// %f %f %f format
#define	CS_SKYROTATE		4
#define CS_SKYCOLOR			5
#define	CS_MAPCHECKSUM		6		// checksum string

#define CS_CTF_BLUE_STAT	8
#define CS_CTF_RED_STAT		9
#define CS_CTF_BLUE_TEAM   	10
#define CS_CTF_RED_TEAM	   	11
#define CS_AMBSET			12
#define CS_MUSICSET			13
#define CS_TERRAINNAME		14
#define CS_SCREENEFFECT		15
#define CS_DEBRISPRECACHE	16

#define	CS_MAXCLIENTS		31

#define	CS_SHOWNAMES		32
#define	CS_SHOWTEAMS		33
#define	CS_SHOWINFOINDARK	34

#define	CS_MODELS			35

#define	CS_SOUNDS			(CS_MODELS+MAX_MODELS) // 291 :: 0x123
#define CS_EFFECTS			(CS_SOUNDS+MAX_SOUNDS) // 647 :: 0x‭287‬
#define	CS_IMAGES			(CS_EFFECTS+MAX_EFPACKS) // 903 :: 0x‭387‬
#define	CS_LIGHTS			(CS_IMAGES+MAX_IMAGES) // 1159 :: 0x‭487‬
#define	CS_PLAYERSKINS		(CS_LIGHTS+MAX_LIGHTSTYLES) // 1415 :: 0x‭587‬
#define	CS_PLAYERICONS		(CS_PLAYERSKINS+MAX_CLIENTS) // 1447 :: 0x‭5A7‬
#define CS_STRING_PACKAGES	(CS_PLAYERICONS+MAX_PLAYERICONS) // 1463 :: 0x‭5B7‬
#define CS_WELCOME			(CS_STRING_PACKAGES+MAX_STRING_PACKAGES) // 1493 :: 0x‭5D5‬
#define CS_GHOULFILES		(CS_WELCOME + 4) // 1497 :: 0x‭5D9‬
#define CS_CONTROL_FLAGS	(CS_GHOULFILES+MAX_GHOULFILES) // 3545 :: 0x‭DD9‬
#define	MAX_CONFIGSTRINGS	(CS_CONTROL_FLAGS+10) // 3555 :: 0x‭DE3‬

// end