// by navewindre
// github.com/navewindre

#include "../typedef.h"

struct COLOR {
  COLOR()  = default;
  COLOR( float r1, float g1, float b1, float a1 ) : r( r1 ), g( g1 ), b( b1 ), a( a1 ) {}
  
  float r;
  float g;
  float b;
  float a;
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
	CAI_BaseNPC = 0,
	CAK47,
	CBaseAnimating,
	CBaseAnimatingOverlay,
	CBaseAttributableItem,
	CBaseButton,
	CBaseCombatCharacter,
	CBaseCombatWeapon,
	CBaseCSGrenade,
	CBaseCSGrenadeProjectile,
	CBaseDoor,
	CBaseEntity,
	CBaseFlex,
	CBaseGrenade,
	CBaseParticleEntity,
	CBasePlayer,
	CBasePropDoor,
	CBaseTeamObjectiveResource,
	CBaseTempEntity,
	CBaseToggle,
	CBaseTrigger,
	CBaseViewModel,
	CBaseVPhysicsTrigger,
	CBaseWeaponWorldModel,
	CBeam,
	CBeamSpotlight,
	CBoneFollower,
	CBRC4Target,
	CBreachCharge,
	CBreachChargeProjectile,
	CBreakableProp,
	CBreakableSurface,
	CC4,
	CCascadeLight,
	CChicken,
	CColorCorrection,
	CColorCorrectionVolume,
	CCSGameRulesProxy,
	CCSPlayer,
	CCSPlayerResource,
	CCSRagdoll,
	CCSTeam,
	CDangerZone,
	CDangerZoneController,
	CDEagle,
	CDecoyGrenade,
	CDecoyProjectile,
	CDrone,
	CDronegun,
	CDynamicLight,
	CDynamicProp,
	CEconEntity,
	CEconWearable,
	CEmbers,
	CEntityDissolve,
	CEntityFlame,
	CEntityFreezing,
	CEntityParticleTrail,
	CEnvAmbientLight,
	CEnvDetailController,
	CEnvDOFController,
	CEnvGasCanister,
	CEnvParticleScript,
	CEnvProjectedTexture,
	CEnvQuadraticBeam,
	CEnvScreenEffect,
	CEnvScreenOverlay,
	CEnvTonemapController,
	CEnvWind,
	CFEPlayerDecal,
	CFireCrackerBlast,
	CFireSmoke,
	CFireTrail,
	CFish,
	CFists,
	CFlashbang,
	CFogController,
	CFootstepControl,
	CFunc_Dust,
	CFunc_LOD,
	CFuncAreaPortalWindow,
	CFuncBrush,
	CFuncConveyor,
	CFuncLadder,
	CFuncMonitor,
	CFuncMoveLinear,
	CFuncOccluder,
	CFuncReflectiveGlass,
	CFuncRotating,
	CFuncSmokeVolume,
	CFuncTrackTrain,
	CGameRulesProxy,
	CGrassBurn,
	CHandleTest,
	CHEGrenade,
	CHostage,
	CHostageCarriableProp,
	CIncendiaryGrenade,
	CInferno,
	CInfoLadderDismount,
	CInfoMapRegion,
	CInfoOverlayAccessor,
	CItem_Healthshot,
	CItemCash,
	CItemDogtags,
	CKnife,
	CKnifeGG,
	CLightGlow,
	CMaterialModifyControl,
	CMelee,
	CMolotovGrenade,
	CMolotovProjectile,
	CMovieDisplay,
	CParadropChopper,
	CParticleFire,
	CParticlePerformanceMonitor,
	CParticleSystem,
	CPhysBox,
	CPhysBoxMultiplayer,
	CPhysicsProp,
	CPhysicsPropMultiplayer,
	CPhysMagnet,
	CPhysPropAmmoBox,
	CPhysPropLootCrate,
	CPhysPropRadarJammer,
	CPhysPropWeaponUpgrade,
	CPlantedC4,
	CPlasma,
	CPlayerResource,
	CPointCamera,
	CPointCommentaryNode,
	CPointWorldText,
	CPoseController,
	CPostProcessController,
	CPrecipitation,
	CPrecipitationBlocker,
	CPredictedViewModel,
	CProp_Hallucination,
	CPropCounter,
	CPropDoorRotating,
	CPropJeep,
	CPropVehicleDriveable,
	CRagdollManager,
	CRagdollProp,
	CRagdollPropAttached,
	CRopeKeyframe,
	CSCAR17,
	CSceneEntity,
	CSensorGrenade,
	CSensorGrenadeProjectile,
	CShadowControl,
	CSlideshowDisplay,
	CSmokeGrenade,
	CSmokeGrenadeProjectile,
	CSmokeStack,
	CSpatialEntity,
	CSpotlightEnd,
	CSprite,
	CSpriteOriented,
	CSpriteTrail,
	CStatueProp,
	CSteamJet,
	CSun,
	CSunlightShadowControl,
	CSurvivalSpawnChopper,
	CTablet,
	CTeam,
	CTeamplayRoundBasedRulesProxy,
	CTEArmorRicochet,
	CTEBaseBeam,
	CTEBeamEntPoint,
	CTEBeamEnts,
	CTEBeamFollow,
	CTEBeamLaser,
	CTEBeamPoints,
	CTEBeamRing,
	CTEBeamRingPoint,
	CTEBeamSpline,
	CTEBloodSprite,
	CTEBloodStream,
	CTEBreakModel,
	CTEBSPDecal,
	CTEBubbles,
	CTEBubbleTrail,
	CTEClientProjectile,
	CTEDecal,
	CTEDust,
	CTEDynamicLight,
	CTEEffectDispatch,
	CTEEnergySplash,
	CTEExplosion,
	CTEFireBullets,
	CTEFizz,
	CTEFootprintDecal,
	CTEFoundryHelpers,
	CTEGaussExplosion,
	CTEGlowSprite,
	CTEImpact,
	CTEKillPlayerAttachments,
	CTELargeFunnel,
	CTEMetalSparks,
	CTEMuzzleFlash,
	CTEParticleSystem,
	CTEPhysicsProp,
	CTEPlantBomb,
	CTEPlayerAnimEvent,
	CTEPlayerDecal,
	CTEProjectedDecal,
	CTERadioIcon,
	CTEShatterSurface,
	CTEShowLine,
	CTesla,
	CTESmoke,
	CTESparks,
	CTESprite,
	CTESpriteSpray,
	CTest_ProxyToggle_Networkable,
	CTestTraceline,
	CTEWorldDecal,
	CTriggerPlayerMovement,
	CTriggerSoundOperator,
	CVGuiScreen,
	CVoteController,
	CWaterBullet,
	CWaterLODControl,
	CWeaponAug,
	CWeaponAWP,
	CWeaponBaseItem,
	CWeaponBizon,
	CWeaponCSBase,
	CWeaponCSBaseGun,
	CWeaponCycler,
	CWeaponElite,
	CWeaponFamas,
	CWeaponFiveSeven,
	CWeaponG3SG1,
	CWeaponGalil,
	CWeaponGalilAR,
	CWeaponGlock,
	CWeaponHKP2000,
	CWeaponM249,
	CWeaponM3,
	CWeaponM4A1,
	CWeaponMAC10,
	CWeaponMag7,
	CWeaponMP5Navy,
	CWeaponMP7,
	CWeaponMP9,
	CWeaponNegev,
	CWeaponNOVA,
	CWeaponP228,
	CWeaponP250,
	CWeaponP90,
	CWeaponSawedoff,
	CWeaponSCAR20,
	CWeaponScout,
	CWeaponSG550,
	CWeaponSG552,
	CWeaponSG556,
	CWeaponSSG08,
	CWeaponTaser,
	CWeaponTec9,
	CWeaponTMP,
	CWeaponUMP45,
	CWeaponUSP,
	CWeaponXM1014,
	CWorld,
	CWorldVguiText,
	DustTrail,
	MovieExplosion,
	ParticleSmokeGrenade,
	RocketTrail,
	SmokeTrail,
	SporeExplosion,
	SporeTrail,
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
