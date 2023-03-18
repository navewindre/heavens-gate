// by navewindre
// github.com/navewindre

#pragma once
#include "../vec3.h"

struct COLOR {
  COLOR()  = default;
  COLOR( F32 r1, F32 g1, F32 b1, F32 a1 ) : r( r1 ), g( g1 ), b( b1 ), a( a1 ) {}
  
  F32 r;
  F32 g;
  F32 b;
  F32 a;
};

struct GLOW_OBJECT {
  U8          pad00[ 4 ];
  U32   			ent;				//0000
  COLOR				color;
  U8				  pad01[ 8 ];
  F32				  bloom_amt;
  U8			    pad_02[ 4 ];
  bool				rwo;	//0024
  bool				rwuo;	//0025
  bool				full_bloom;
  char        pad_002B[1]; //0x002B
  I32         fullbloom_stencil; //0x002C
  I32         unk; //0x0030
  I32         splitscreen_slot; //0x0034
};

struct GLOW_OBJ_MANAGER {
  GLOW_OBJECT*	objects;
  U32		max;
  U32		unk02;
  U32		count;
  U32		data_ptr_back;
  U32		first_free_slot;
  U32		unk1;
  U32		unk2;
  U32		unk3;
  U32		unk4;
  U32		unk5;
};

struct RECV_PROP;
struct RECV_TABLE {
  RECV_PROP*  props;
  I32         prop_count;
  void*       decoder;
  const char* table_name;

  bool initialized;
  bool in_main_list;
};

struct RECV_PROP {
  const char* varname;
  I32         recv_type;
  I32         flags;
  I32         buffer_size;
  bool        inside_array;
  void*       extra_data;

  RECV_PROP*  array_prop;
  void*       array_length_proxy;

  void*       proxy_fn;
  void*       dt_proxy_fn;

  RECV_TABLE* table;
  I32         offset;

  I32 element_stride;
  I32 elements;

  const char* parent_array_name;
};

class CSGO_CLIENT_CLASS {
public:
  void* create_fn;
  void* create_event_fn;
  const char* network_name;
  void* recv;
  CSGO_CLIENT_CLASS* next;
  U32 index;
};

enum CSGO_CLASS_ID {
  CTestTraceline = 0x000000E0,
  CTEWorldDecal = 0x000000E1,
  CTESpriteSpray = 0x000000DE,
  CTESprite = 0x000000DD,
  CTESparks = 0x000000DC,
  CTESmoke = 0x000000DB,
  CTEShowLine = 0x000000D9,
  CTEProjectedDecal = 0x000000D6,
  CFEPlayerDecal = 0x00000047,
  CTEPlayerDecal = 0x000000D5,
  CTEPhysicsProp = 0x000000D2,
  CTEParticleSystem = 0x000000D1,
  CTEMuzzleFlash = 0x000000D0,
  CTELargeFunnel = 0x000000CE,
  CTEKillPlayerAttachments = 0x000000CD,
  CTEImpact = 0x000000CC,
  CTEGlowSprite = 0x000000CB,
  CTEShatterSurface = 0x000000D8,
  CTEFootprintDecal = 0x000000C8,
  CTEFizz = 0x000000C7,
  CTEExplosion = 0x000000C5,
  CTEEnergySplash = 0x000000C4,
  CTEEffectDispatch = 0x000000C3,
  CTEDynamicLight = 0x000000C2,
  CTEDecal = 0x000000C0,
  CTEClientProjectile = 0x000000BF,
  CTEBubbleTrail = 0x000000BE,
  CTEBubbles = 0x000000BD,
  CTEBSPDecal = 0x000000BC,
  CTEBreakModel = 0x000000BB,
  CTEBloodStream = 0x000000BA,
  CTEBloodSprite = 0x000000B9,
  CTEBeamSpline = 0x000000B8,
  CTEBeamRingPoint = 0x000000B7,
  CTEBeamRing = 0x000000B6,
  CTEBeamPoints = 0x000000B5,
  CTEBeamLaser = 0x000000B4,
  CTEBeamFollow = 0x000000B3,
  CTEBeamEnts = 0x000000B2,
  CTEBeamEntPoint = 0x000000B1,
  CTEBaseBeam = 0x000000B0,
  CTEArmorRicochet = 0x000000AF,
  CTEMetalSparks = 0x000000CF,
  CSteamJet = 0x000000A8,
  CSmokeStack = 0x0000009E,
  DustTrail = 0x00000115,
  CFireTrail = 0x0000004A,
  SporeTrail = 0x0000011B,
  SporeExplosion = 0x0000011A,
  RocketTrail = 0x00000118,
  SmokeTrail = 0x00000119,
  CPropVehicleDriveable = 0x00000091,
  ParticleSmokeGrenade = 0x00000117,
  CParticleFire = 0x00000075,
  MovieExplosion = 0x00000116,
  CTEGaussExplosion = 0x000000CA,
  CEnvQuadraticBeam = 0x00000042,
  CEmbers = 0x00000037,
  CEnvWind = 0x00000046,
  CPrecipitation = 0x0000008A,
  CPrecipitationBlocker = 0x0000008B,
  CBaseTempEntity = 0x00000012,
  NextBotCombatCharacter = 0x00000000,
  CEconWearable = 0x00000036,
  CBaseAttributableItem = 0x00000004,
  CEconEntity = 0x00000035,
  CWeaponZoneRepulsor = 0x00000112,
  CWeaponXM1014 = 0x00000111,
  CWeaponTaser = 0x0000010C,
  CTablet = 0x000000AC,
  CSnowball = 0x0000009F,
  CSmokeGrenade = 0x0000009C,
  CWeaponShield = 0x0000010A,
  CWeaponSG552 = 0x00000108,
  CSensorGrenade = 0x00000098,
  CWeaponSawedoff = 0x00000104,
  CWeaponNOVA = 0x00000100,
  CIncendiaryGrenade = 0x00000063,
  CMolotovGrenade = 0x00000071,
  CMelee = 0x00000070,
  CWeaponM3 = 0x000000F8,
  CKnifeGG = 0x0000006C,
  CKnife = 0x0000006B,
  CHEGrenade = 0x00000060,
  CFlashbang = 0x0000004D,
  CFists = 0x0000004C,
  CWeaponElite = 0x000000EF,
  CDecoyGrenade = 0x0000002F,
  CDEagle = 0x0000002E,
  CWeaponUSP = 0x00000110,
  CWeaponM249 = 0x000000F7,
  CWeaponUMP45 = 0x0000010F,
  CWeaponTMP = 0x0000010E,
  CWeaponTec9 = 0x0000010D,
  CWeaponSSG08 = 0x0000010B,
  CWeaponSG556 = 0x00000109,
  CWeaponSG550 = 0x00000107,
  CWeaponScout = 0x00000106,
  CWeaponSCAR20 = 0x00000105,
  CSCAR17 = 0x00000096,
  CWeaponP90 = 0x00000103,
  CWeaponP250 = 0x00000102,
  CWeaponP228 = 0x00000101,
  CWeaponNegev = 0x000000FF,
  CWeaponMP9 = 0x000000FE,
  CWeaponMP7 = 0x000000FD,
  CWeaponMP5Navy = 0x000000FC,
  CWeaponMag7 = 0x000000FB,
  CWeaponMAC10 = 0x000000FA,
  CWeaponM4A1 = 0x000000F9,
  CWeaponHKP2000 = 0x000000F6,
  CWeaponGlock = 0x000000F5,
  CWeaponGalilAR = 0x000000F4,
  CWeaponGalil = 0x000000F3,
  CWeaponG3SG1 = 0x000000F2,
  CWeaponFiveSeven = 0x000000F1,
  CWeaponFamas = 0x000000F0,
  CWeaponBizon = 0x000000EB,
  CWeaponAWP = 0x000000E9,
  CWeaponAug = 0x000000E8,
  CAK47 = 0x00000001,
  CWeaponCSBaseGun = 0x000000ED,
  CWeaponCSBase = 0x000000EC,
  CC4 = 0x00000022,
  CBumpMine = 0x00000020,
  CBumpMineProjectile = 0x00000021,
  CBreachCharge = 0x0000001C,
  CBreachChargeProjectile = 0x0000001D,
  CWeaponBaseItem = 0x000000EA,
  CBaseCSGrenade = 0x00000008,
  CSnowballProjectile = 0x000000A1,
  CSnowballPile = 0x000000A0,
  CSmokeGrenadeProjectile = 0x0000009D,
  CSensorGrenadeProjectile = 0x00000099,
  CMolotovProjectile = 0x00000072,
  CItem_Healthshot = 0x00000068,
  CItemDogtags = 0x0000006A,
  CDecoyProjectile = 0x00000030,
  CPhysPropRadarJammer = 0x0000007F,
  CPhysPropWeaponUpgrade = 0x00000080,
  CPhysPropAmmoBox = 0x0000007D,
  CPhysPropLootCrate = 0x0000007E,
  CItemCash = 0x00000069,
  CEnvGasCanister = 0x0000003F,
  CDronegun = 0x00000032,
  CParadropChopper = 0x00000074,
  CSurvivalSpawnChopper = 0x000000AB,
  CBRC4Target = 0x0000001B,
  CInfoMapRegion = 0x00000066,
  CFireCrackerBlast = 0x00000048,
  CInferno = 0x00000064,
  CChicken = 0x00000024,
  CDrone = 0x00000031,
  CFootstepControl = 0x0000004F,
  CCSGameRulesProxy = 0x00000027,
  CWeaponCubemap = 0x00000000,
  CWeaponCycler = 0x000000EE,
  CTEPlantBomb = 0x000000D3,
  CTEFireBullets = 0x000000C6,
  CTERadioIcon = 0x000000D7,
  CPlantedC4 = 0x00000081,
  CCSTeam = 0x0000002B,
  CCSPlayerResource = 0x00000029,
  CCSPlayer = 0x00000028,
  CPlayerPing = 0x00000083,
  CCSRagdoll = 0x0000002A,
  CTEPlayerAnimEvent = 0x000000D4,
  CHostage = 0x00000061,
  CHostageCarriableProp = 0x00000062,
  CBaseCSGrenadeProjectile = 0x00000009,
  CHandleTest = 0x0000005F,
  CTeamplayRoundBasedRulesProxy = 0x000000AE,
  CSpriteTrail = 0x000000A6,
  CSpriteOriented = 0x000000A5,
  CSprite = 0x000000A4,
  CRagdollPropAttached = 0x00000094,
  CRagdollProp = 0x00000093,
  CPropCounter = 0x0000008E,
  CPredictedViewModel = 0x0000008C,
  CPoseController = 0x00000088,
  CGrassBurn = 0x0000005E,
  CGameRulesProxy = 0x0000005D,
  CInfoLadderDismount = 0x00000065,
  CFuncLadder = 0x00000055,
  CTEFoundryHelpers = 0x000000C9,
  CEnvDetailController = 0x0000003D,
  CDangerZone = 0x0000002C,
  CDangerZoneController = 0x0000002D,
  CWorldVguiText = 0x00000114,
  CWorld = 0x00000113,
  CWaterLODControl = 0x000000E7,
  CWaterBullet = 0x000000E6,
  CMapVetoPickController = 0x0000006E,
  CVoteController = 0x000000E5,
  CVGuiScreen = 0x000000E4,
  CPropJeep = 0x00000090,
  CPropVehicleChoreoGeneric = 0x00000000,
  CTriggerSoundOperator = 0x000000E3,
  CBaseVPhysicsTrigger = 0x00000016,
  CTriggerPlayerMovement = 0x000000E2,
  CBaseTrigger = 0x00000014,
  CTest_ProxyToggle_Networkable = 0x000000DF,
  CTesla = 0x000000DA,
  CBaseTeamObjectiveResource = 0x00000011,
  CTeam = 0x000000AD,
  CSunlightShadowControl = 0x000000AA,
  CSun = 0x000000A9,
  CParticlePerformanceMonitor = 0x00000076,
  CSpotlightEnd = 0x000000A3,
  CSpatialEntity = 0x000000A2,
  CSlideshowDisplay = 0x0000009B,
  CShadowControl = 0x0000009A,
  CSceneEntity = 0x00000097,
  CRopeKeyframe = 0x00000095,
  CRagdollManager = 0x00000092,
  CPhysicsPropMultiplayer = 0x0000007B,
  CPhysBoxMultiplayer = 0x00000079,
  CPropDoorRotating = 0x0000008F,
  CBasePropDoor = 0x00000010,
  CDynamicProp = 0x00000034,
  CProp_Hallucination = 0x0000008D,
  CPostProcessController = 0x00000089,
  CPointWorldText = 0x00000087,
  CPointCommentaryNode = 0x00000086,
  CPointCamera = 0x00000085,
  CPlayerResource = 0x00000084,
  CPlasma = 0x00000082,
  CPhysMagnet = 0x0000007C,
  CPhysicsProp = 0x0000007A,
  CStatueProp = 0x000000A7,
  CPhysBox = 0x00000078,
  CParticleSystem = 0x00000077,
  CMovieDisplay = 0x00000073,
  CMaterialModifyControl = 0x0000006F,
  CLightGlow = 0x0000006D,
  CItemAssaultSuitUseable = 0x00000000,
  CItem = 0x00000000,
  CInfoOverlayAccessor = 0x00000067,
  CFuncTrackTrain = 0x0000005C,
  CFuncSmokeVolume = 0x0000005B,
  CFuncRotating = 0x0000005A,
  CFuncReflectiveGlass = 0x00000059,
  CFuncOccluder = 0x00000058,
  CFuncMoveLinear = 0x00000057,
  CFuncMonitor = 0x00000056,
  CFunc_LOD = 0x00000051,
  CTEDust = 0x000000C1,
  CFunc_Dust = 0x00000050,
  CFuncConveyor = 0x00000054,
  CFuncBrush = 0x00000053,
  CBreakableSurface = 0x0000001F,
  CFuncAreaPortalWindow = 0x00000052,
  CFish = 0x0000004B,
  CFireSmoke = 0x00000049,
  CEnvTonemapController = 0x00000045,
  CEnvScreenEffect = 0x00000043,
  CEnvScreenOverlay = 0x00000044,
  CEnvProjectedTexture = 0x00000041,
  CEnvParticleScript = 0x00000040,
  CFogController = 0x0000004E,
  CEnvDOFController = 0x0000003E,
  CCascadeLight = 0x00000023,
  CEnvAmbientLight = 0x0000003C,
  CEntityParticleTrail = 0x0000003B,
  CEntityFreezing = 0x0000003A,
  CEntityFlame = 0x00000039,
  CEntityDissolve = 0x00000038,
  CDynamicLight = 0x00000033,
  CColorCorrectionVolume = 0x00000026,
  CColorCorrection = 0x00000025,
  CBreakableProp = 0x0000001E,
  CBeamSpotlight = 0x00000019,
  CBaseButton = 0x00000005,
  CBaseToggle = 0x00000013,
  CBasePlayer = 0x0000000F,
  CBaseFlex = 0x0000000C,
  CBaseEntity = 0x0000000B,
  CBaseDoor = 0x0000000A,
  CBaseCombatCharacter = 0x00000006,
  CBaseAnimatingOverlay = 0x00000003,
  CBoneFollower = 0x0000001A,
  CBaseAnimating = 0x00000002,
  CAI_BaseNPC = 0x00000000,
  CBeam = 0x00000018,
  CBaseViewModel = 0x00000015,
  CBaseParticleEntity = 0x0000000E,
  CBaseGrenade = 0x0000000D,
  CBaseCombatWeapon = 0x00000007,
  CBaseWeaponWorldModel = 0x00000017,
};

struct CSGO_ANIM_STATE
{
private:
    U32 unk000; //0x0000
    U32 unk001; //0x0004
    char pad_0[4][4]; //0x0008
public:
    float unk_time; //0x0018 wheeee
private:
    float point_four; //0x001C always 0.4
    float point_two; //0x0020 always 0.2
    U32 pad_1; //0x0024
public:
    float walk_amt; //0x0028 Resets to 0 when movement stops
    float stop_amt; //0x002C Resets to 0 when full run starts (bw/fw)
private:
    float point_two2; //0x0030 always 0.2
    float point_four2; //0x0034 always 0.4
    float unk_float_but_special; //0x0038
    float unk_float2; //0x003C Resets to 0 when movement stops
    float unk_float3; //0x0040 Resets to 0 when movement starts
    float unk_float4; //0x0044 static? 0.3
    float unk_float5; //0x0048 static? 0.3
    float unk_float6; //0x004C 0.0 <-> 1.0 (to 1. when moving)
    U32 unk_U32; //0x0050 static?  0x23E
    char pad_2[2][4]; //0x0054
    void *curr_weapon_0; //0x005C current weapon
    void *static_something; //0x0060
    void *curr_weapon_1; //0x0064 current weapon
    void *curr_weapon_2; //0x0068 current weapon
    float unk_time1; //0x006C same as +0x18
    U32 unk_time2; //0x0070 increases with time but its an int
    U32 what; //0x0074
    float look_dir[3]; //0x0078
    float hell_yaw; //0x0084
    float velocity[3]; //0x0088
    float uppies; //0x0094
    float i_have_no_idea; //0x0098
    float unk_float_the_sixth; //0x009C
    float N00000304; //0x00A0
    float jump_something0; //0x00A4
    float jump_something1; //0x00A8
    U32 delaware; //0x00AC
    float origin_something[3]; //0x00B0
    float position_something[3]; //0x00BC
    float inspector_vector[3]; //0x00C8
    float you_vector_go_catch_it[3]; //0x00D4
    float wow_three_floats[3]; //0x00E0
    float i_cant_believe_its_not_an_array[3]; //0x00EC
    float fuel_prices[3]; //0x00F8
    float wow_the_point_FLOATS_get_it; //0x0104
    U8 onGround; //0x0108
    U8 hitGroundAnim; //0x0109
    U16 u_thought; //0x010A
    char pad_010C[4]; //0x010C
    float N00000387[3]; //0x0110
    float N0000038A; //0x011C
    char pad_0120[160]; //0x0120
}; //Size: 0x01C0

class CSGO_NETCHANNEL {
  
};

struct CSGO_EVENT_INFO {
  I16 class_id;
  F32 fire_delay;
  const void* send_table;
  CSGO_CLIENT_CLASS* client_class;
  void* data;
  I32 packed_bits;
  I32 flags;
private:
  U8 pad[16];
};

class CSGO_CLIENTSTATE {
  char pad[156];
public:
  CSGO_NETCHANNEL* netchannel;
  I32 challenge;
private:
  U8 pad1[4];
public:
  F64 connect_time;
  I32 retry_number;
private:
  U8 pad2[84];
public:
  I32 signon_state;
private:
  U8 pad3[4];
public:
  F64 next_cmd_time;
  I32 server_count;
  I32 current_sequence;
private:
  U8 pad4[8];
public:
  float clock_offsets[16];
  I32 cur_clock_offset;
  I32 server_tick;
  I32 client_tick;
  I32 delta_tick;
private:
  U32 pad5;
public:
  char level_name[260];
  char level_name_short[40];
private:
  U8 pad7[212];
public:
  I32 maxclients;
private:
  U8 pad8[18836];
public:
  I32 old_tickcount;
  F32 tick_remainder;
  F32 frame_time;
  I32 last_outgoing_command;
  I32 choked_commands;
  I32 last_command_ack;
  I32 last_server_tick;
  I32 command_ack;
  I32 sound_sequence;
  I32 last_progress_percent;
  bool is_hltv;
private:
  U8 pad9[75];
public:
  VEC3 viewangles;
private:
  U8 pad10[204];
public:
  CSGO_EVENT_INFO* events;
};