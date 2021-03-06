#ifndef __W_TYPES_H
#define __W_TYPES_H

typedef enum
{
	AMMO_NONE = 0,
	AMMO_KNIFE,
	AMMO_44,
	AMMO_9MM,
	AMMO_SHELLS,
	AMMO_556,
	AMMO_ROCKET,
	AMMO_MWAVE,
	AMMO_FTHROWER,
	AMMO_SLUG,
	AMMO_MACHPIS9MM,
	MAXAMMOS
} ammos_t;

#define SFW_BESTWEAPONPOSSIBLE -1

typedef enum
{
	SFW_EMPTYSLOT = 0,
	SFW_KNIFE,
	SFW_PISTOL2,
	SFW_PISTOL1,
	SFW_MACHINEPISTOL,
	SFW_ASSAULTRIFLE,
	SFW_SNIPER,
	SFW_AUTOSHOTGUN,
	SFW_SHOTGUN,
	SFW_MACHINEGUN,
	SFW_ROCKET,
	SFW_MICROWAVEPULSE,
	SFW_FLAMEGUN,
	SFW_HURTHAND,
	SFW_THROWHAND,
	SFW_NUM_WEAPONS
} weapons_t;

typedef enum
{
	SFE_EMPTYSLOT = 0,
	SFE_FLASHPACK,
//	SFE_NEURAL_GRENADE,
	SFE_C4,
	SFE_LIGHT_GOGGLES,
	SFE_CLAYMORE,
	SFE_MEDKIT,
	SFE_GRENADE,
	SFE_CTFFLAG,
	SFE_NUMITEMS
} equipment_t;

/*
typedef enum
{
	CTF_FLAG = 0,
} ctf_t;
*/

typedef enum
{
	SFC_CASH = 0,
} cash_t;

typedef enum
{
	SFH_SMALL_HEALTH = 0,
	SFH_LARGE_HEALTH,
} health_t;

#endif // __W_TYPES_H
