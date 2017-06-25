/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_TUNING_H
#define GAME_TUNING_H
#undef GAME_TUNING_H // this file will be included several times

// physics tuning
MACRO_TUNING_PARAM(GroundControlSpeed, ground_control_speed, 10.0f)
MACRO_TUNING_PARAM(GroundControlAccel, ground_control_accel, 100.0f / TicksPerSecond)
MACRO_TUNING_PARAM(GroundFriction, ground_friction, 0.5f)
MACRO_TUNING_PARAM(GroundJumpImpulse, ground_jump_impulse, 13.2f)
MACRO_TUNING_PARAM(AirJumpImpulse, air_jump_impulse, 12.0f)
MACRO_TUNING_PARAM(AirControlSpeed, air_control_speed, 250.0f / TicksPerSecond)
MACRO_TUNING_PARAM(AirControlAccel, air_control_accel, 1.5f)
MACRO_TUNING_PARAM(AirFriction, air_friction, 0.95f)
MACRO_TUNING_PARAM(HookLength, hook_length, 380.0f)
MACRO_TUNING_PARAM(HookFireSpeed, hook_fire_speed, 80.0f)
MACRO_TUNING_PARAM(HookDragAccel, hook_drag_accel, 3.0f)
MACRO_TUNING_PARAM(HookDragSpeed, hook_drag_speed, 15.0f)
MACRO_TUNING_PARAM(Gravity, gravity, 0.5f)

MACRO_TUNING_PARAM(VelrampStart, velramp_start, 550)
MACRO_TUNING_PARAM(VelrampRange, velramp_range, 2000)
MACRO_TUNING_PARAM(VelrampCurvature, velramp_curvature, 1.4f)

// weapon tuning
MACRO_TUNING_PARAM(GunCurvature, gun_curvature, 1.25f)
MACRO_TUNING_PARAM(GunSpeed, gun_speed, 2200.0f)
MACRO_TUNING_PARAM(GunLifetime, gun_lifetime, 2.0f)

MACRO_TUNING_PARAM(ShotgunCurvature, shotgun_curvature, 1.25f)
MACRO_TUNING_PARAM(ShotgunSpeed, shotgun_speed, 2750.0f)
MACRO_TUNING_PARAM(ShotgunSpeeddiff, shotgun_speeddiff, 0.8f)
MACRO_TUNING_PARAM(ShotgunLifetime, shotgun_lifetime, 0.20f)

MACRO_TUNING_PARAM(GrenadeCurvature, grenade_curvature, 7.0f)
MACRO_TUNING_PARAM(GrenadeSpeed, grenade_speed, 1000.0f)
MACRO_TUNING_PARAM(GrenadeLifetime, grenade_lifetime, 2.0f)

MACRO_TUNING_PARAM(LaserReach, laser_reach, 800.0f)
MACRO_TUNING_PARAM(LaserBounceDelay, laser_bounce_delay, 150)
MACRO_TUNING_PARAM(LaserBounceNum, laser_bounce_num, 1)
MACRO_TUNING_PARAM(LaserBounceCost, laser_bounce_cost, 0)
MACRO_TUNING_PARAM(LaserDamage, laser_damage, 5)

MACRO_TUNING_PARAM(PlayerCollision, player_collision, 1)
MACRO_TUNING_PARAM(PlayerHooking, player_hooking, 1)

// TeeMo
MACRO_TUNING_PARAM(PhantomGrenadeRegentime, phantom_grenade_regentime, 4000)
MACRO_TUNING_PARAM(PhantomGrenadeMaxAmmo, phantom_grenade_max_ammo, 6)
MACRO_TUNING_PARAM(PhantomGrenadeLifetime, phantom_grenade_lifetime, 0.7f)
MACRO_TUNING_PARAM(PhantomGrenadeFireDelay, phantom_grenade_fire_delay, 550.0f)
MACRO_TUNING_PARAM(AirGrenadeLifetime, air_grenade_lifetime, 15.0f)
MACRO_TUNING_PARAM(AirGrenadeFireDelay, air_grenade_fire_delay, 550.0f)
MACRO_TUNING_PARAM(LaserGrenadeLifetime, laser_grenade_lifetime, 1.7f)
MACRO_TUNING_PARAM(LaserGrenadeRegentime, laser_grenade_regentime, 2500)
MACRO_TUNING_PARAM(LaserGrenadeMaxAmmo, laser_grenade_max_ammo, 4)
MACRO_TUNING_PARAM(LaserGrenadeReach, laser_grenade_reach, 400.0f)
MACRO_TUNING_PARAM(LaserGrenadeFireDelay, laser_grenade_fire_delay, 550.0f)
MACRO_TUNING_PARAM(ExtraBombGrenadeLifetime, extra_bomb_grenade_lifetime, 1.7f)
MACRO_TUNING_PARAM(ExtraBombGrenadeRegentime, extra_bomb_grenade_regentime, 400)
MACRO_TUNING_PARAM(ExtraBombGrenadeMaxAmmo, extra_bomb_grenade_max_ammo, 4)
MACRO_TUNING_PARAM(ExtraBombGrenadeFireDelay, extra_bomb_grenade_fire_delay, 50.0f)
MACRO_TUNING_PARAM(DragonGrenadeLifetime, dragon_grenade_lifetime, 2.0f)
MACRO_TUNING_PARAM(DragonGrenadeRegentime, dragon_grenade_regentime, 3500)
MACRO_TUNING_PARAM(DragonGrenadeMaxAmmo, dragon_grenade_max_ammo, 3)
MACRO_TUNING_PARAM(DragonGrenadeFireDelay, dragon_grenade_fire_delay, 550.0f)
MACRO_TUNING_PARAM(DragonGrenadeExplosionDelay, dragon_grenade_explosion_delay, 150.0f)
MACRO_TUNING_PARAM(PlasmaRifleLifetime, plasma_lifetime, 0.7f)
MACRO_TUNING_PARAM(PlasmaRifleSpeed, plasma_speed, 150.0f)
MACRO_TUNING_PARAM(PlasmaRifleRegentime, plasma_rifle_regentime, 500)
MACRO_TUNING_PARAM(PlasmaRifleMaxAmmo, plasma_rifle_max_ammo, 4)
MACRO_TUNING_PARAM(PlasmaRifleFireDelay, plasma_rifle_fire_delay, 200.0f)
MACRO_TUNING_PARAM(LaserTurretRadius, turret_radius, 225.0f)
MACRO_TUNING_PARAM(LaserTurretFireDelay, turret_fire_delay, 2000.0f)
MACRO_TUNING_PARAM(LaserTurretBuildDelay, turret_build_delay, 400.0f)
MACRO_TUNING_PARAM(LaserTurretNum, turret_max_builds, 3)
MACRO_TUNING_PARAM(LaserTurretHealth, turret_health, 15)
MACRO_TUNING_PARAM(LaserHealerReach, laser_healer_reach, 150.0f)
MACRO_TUNING_PARAM(LaserHealerSpeed, laser_healer_speed, 400.0f)
MACRO_TUNING_PARAM(LaserRepairReach, laser_repair_reach, 150.0f)
MACRO_TUNING_PARAM(LaserRepairSpeed, laser_repair_speed, 400.0f)

MACRO_TUNING_PARAM(FlyBonusKills, fly_bonus_kills, 6)
MACRO_TUNING_PARAM(FlyBonusLifetime, fly_bonus_lifetime, 240000.0f)
MACRO_TUNING_PARAM(InvisibleBonusKills, invisible_bonus_kills, 12)
MACRO_TUNING_PARAM(InvisibleBonusLifetime, invisible_bonus_lifetime, 240000.0f)
MACRO_TUNING_PARAM(HookerBonusKills, hooker_bonus_kills, 22)
MACRO_TUNING_PARAM(HookerBonusLifetime, hooker_bonus_lifetime, 240000.0f)
MACRO_TUNING_PARAM(ExtraBombBonusKills, extra_bomb_bonus_kills, 34)
MACRO_TUNING_PARAM(GodBonusKills, god_bonus_kills, 42)
MACRO_TUNING_PARAM(GodBonusLifetime, god_bonus_lifetime, 80000.0f)

MACRO_TUNING_PARAM(MaxEngineers, max_engineers, 2)
MACRO_TUNING_PARAM(MaxGenerals, max_general, -1)
MACRO_TUNING_PARAM(MaxSpies, max_spies, 1)
MACRO_TUNING_PARAM(MaxMedics, max_medics, 4)
#endif
