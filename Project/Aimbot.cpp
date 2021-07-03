#include "Miscellaneous.h"
#include "Aimbot.h"
#include "SDK.h"
#include <deque>

#include "Assorted.h"

void GetBulletTypeParameters(int iBulletType, float& fPenetrationPower, float& flPenetrationDistance)
{
	static auto function = reinterpret_cast<void(__stdcall*)(int, float&, float&)>(
		CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? F3 0F 10 43 ? 33 FF")));
	function(iBulletType, fPenetrationPower, flPenetrationDistance);
}

//Yes i love copy pasting from ida (55 8B EC 8B 45 08 83 C0 BD)
void GetMaterialParameters(int iMaterial, float& flPenetrationModifier, float& flDamageModifier)
{
	switch (iMaterial)
	{
	case 67:
		flPenetrationModifier = 0.40000001;
		flDamageModifier = 0.25;
		break;
	case 68:
	case 77:
		flPenetrationModifier = 0.5;
		flDamageModifier = 0.30000001;
		break;
	case 71:
		flPenetrationModifier = 1.0;
		flDamageModifier = 0.99000001;
		break;
	case 80:
		flPenetrationModifier = 0.40000001;
		flDamageModifier = 0.44999999;
		break;
	case 84:
		flPenetrationModifier = 0.64999998;
		flDamageModifier = 0.30000001;
		break;
	case 86:
		flPenetrationModifier = 0.5;
		flDamageModifier = 0.44999999;
		break;
	case 87:
		flPenetrationModifier = 1.0;
		flDamageModifier = 0.60000002;
		break;
	default:
		flPenetrationModifier = 1.0;
		flDamageModifier = 0.5;
		break;
	}
}

bool TraceToExit(Vector& start, Vector& dir, Vector& end, float flStepSize, float flMaxDistance)
{
	float flDistance = 0;
	Vector last = start;

	while (flDistance <= flMaxDistance)
	{
		flDistance += flStepSize;

		end = start + (dir * flDistance);

		if ((Trace->GetPointContents(end) & MASK_SOLID) == 0)
		{
			// found first free point
			return true;
		}
	}

	return false;
}

bool IsArmored(int hitgroup, int armor_value, bool has_helmet)
{
	bool bApplyArmor = false;

	if (armor_value > 0)
	{
		switch (hitgroup)
		{
		case HITGROUP_GENERIC:
		case HITGROUP_CHEST:
		case HITGROUP_STOMACH:
		case HITGROUP_LEFTARM:
		case HITGROUP_RIGHTARM:
			bApplyArmor = true;
			break;
		case HITGROUP_HEAD:
			if (has_helmet)
			{
				bApplyArmor = true;
			}
			break;
		default:
			break;
		}
	}

	return bApplyArmor;
}

float ScaleDamage(float damage, int hitgroup, int armor_value, bool has_helmet, float weapon_armor_ratio)
{
	auto new_dmg = damage;
	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		new_dmg *= 4.f;
		break;
	case HITGROUP_STOMACH:
		new_dmg *= 1.25f;
		break;
	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		new_dmg *= 0.75f;
		break;
	default:
		break;
	}

	auto armor_bonus = 0.5f;
	auto armor_ratio = weapon_armor_ratio * 0.5f;
	if (armor_value && IsArmored(hitgroup, armor_value, has_helmet))
	{
		auto flNew = new_dmg * armor_ratio;
		auto flArmor = (new_dmg - flNew) * armor_bonus;

		if (flArmor > armor_value)
		{
			flArmor = armor_value;
			flArmor *= (1 / armor_bonus);
			flNew = new_dmg - flArmor;
		}

		new_dmg = flNew;
	}

	return new_dmg;
}


bool CanPenetratePoint(CBaseEntity* pLocal, CBaseEntity* target_player, Vector shootAngles)
{
	static auto PhysProps = (IPhysicsSurfaceProps*)Interfaces::GetInterface("vphysics.dll", "VPhysicsSurfaceProps001");
	assert(PhysProps);
	auto weapon = pLocal->GetActiveWeapon();
	if (!weapon)
		return false;

	auto weapon_data = weapon->GetWpnData();
	if (!weapon_data)
		return false;

	auto iWeaponID = weapon->GetWeaponID();
	auto iDamage = weapon_data->m_iDamage;
	auto iBulletType = weapon_data->iAmmoType;
	auto flDistance = weapon_data->m_flRange;
	auto flRangeModifier = weapon_data->m_flRangeModifier;
	auto iPenetration = weapon_data->m_iPenetration;

	if (weapon->GetWeaponMode() == Secondary_Mode)
	{
		if (iWeaponID == WEAPON_GLOCK)
		{
			iDamage = 18;	// reduced power for burst shots
			flRangeModifier = 0.9f;
		}
		else if (iWeaponID == WEAPON_M4A1)
			flRangeModifier = 0.95f; // slower bullets in silenced mode
		else if (iWeaponID == WEAPON_USP)
			iDamage = 30; // reduced damage in silenced mode
	}

	float fCurrentDamage = iDamage;   // damage of the bullet at it's current trajectory
	float flCurrentDistance = 0.0;  //distance that the bullet has traveled so far

	Vector vecDir, vecRight, vecUp;
	Math::AngleVectors(shootAngles, &vecDir, &vecRight, &vecUp);

	float flPenetrationPower = 0;		// thickness of a wall that this bullet can penetrate
	float flPenetrationDistance = 0;	// distance at which the bullet is capable of penetrating a wall
	float flDamageModifier = 0.5;		// default modification of bullets power after they go through a wall.
	float flPenetrationModifier = 1.f;

	GetBulletTypeParameters(iBulletType, flPenetrationPower, flPenetrationDistance);

	auto vecSrc = pLocal->GetEyePosition();

	int Loop = 0; while (Loop++ < 15 && fCurrentDamage > 0)
	{
		Vector vecEnd = vecSrc + vecDir * flDistance;

		trace_t tr;

		UTIL_TraceLine(vecSrc, vecEnd, CS_MASK_SHOOT | CONTENTS_HITBOX, pLocal, 0, &tr);
		{
			CTraceFilter filter;
			filter.pSkip = pLocal;
			UTIL_ClipTraceToPlayers(vecSrc, vecEnd + vecDir * 40.0f, CS_MASK_SHOOT | CONTENTS_HITBOX, &filter, &tr);
		}

		if (tr.fraction == 1.0f)
			break;

		/************* MATERIAL DETECTION ***********/
		surfacedata_t* pSurfaceData = PhysProps->GetSurfaceData(tr.surface.surfaceProps);
		int iEnterMaterial = pSurfaceData->game.material;

		GetMaterialParameters(iEnterMaterial, flPenetrationModifier, flDamageModifier);

		bool hitGrate = tr.contents & CONTENTS_GRATE;

		// since some railings in de_inferno are CONTENTS_GRATE but CHAR_TEX_CONCRETE, we'll trust the
		// CONTENTS_GRATE and use a high damage modifier.
		if (hitGrate)
		{
			// If we're a concrete grate (TOOLS/TOOLSINVISIBLE texture) allow more penetrating power.
			flPenetrationModifier = 1.0f;
			flDamageModifier = 0.99f;
		}

		//calculate the damage based on the distance the bullet travelled.
		flCurrentDistance += tr.fraction * flDistance;
		fCurrentDamage *= pow(flRangeModifier, (flCurrentDistance / 500));

		auto player = reinterpret_cast<CBaseEntity*>(tr.m_pEnt);
		if (player == target_player && tr.hitgroup != HITGROUP_GENERIC && tr.hitgroup != HITGROUP_GEAR)
		{
			return ScaleDamage(fCurrentDamage, tr.hitgroup, player->GetArmorValue(),
				player->HasHelmet(), weapon_data->m_flArmorRatio) > 0.1f;
		}

		// check if we reach penetration distance, no more penetrations after that
		if (flCurrentDistance > flPenetrationDistance && iPenetration > 0)
			iPenetration = 0;

		// check if bullet can penetarte another entity
		if (iPenetration == 0 && !hitGrate)
			break; // no, stop

		// If we hit a grate with iPenetration == 0, stop on the next thing we hit
		if (iPenetration < 0)
			break;

		Vector penetrationEnd;

		// try to penetrate object, maximum penetration is 128 inch
		if (!TraceToExit(tr.endpos, vecDir, penetrationEnd, 24, 128))
			break;

		// find exact penetration exit
		trace_t exitTr;
		UTIL_TraceLine(penetrationEnd, tr.endpos, CS_MASK_SHOOT | CONTENTS_HITBOX, NULL, 0, &exitTr);

		if (exitTr.m_pEnt != tr.m_pEnt && exitTr.m_pEnt != NULL)
		{
			// something was blocking, trace again
			UTIL_TraceLine(penetrationEnd, tr.endpos, CS_MASK_SHOOT | CONTENTS_HITBOX, exitTr.m_pEnt, 0, &exitTr);
		}

		// get material at exit point
		pSurfaceData = PhysProps->GetSurfaceData(exitTr.surface.surfaceProps);
		int iExitMaterial = pSurfaceData->game.material;

		hitGrate = hitGrate && (exitTr.contents & CONTENTS_GRATE);

		// if enter & exit point is wood or metal we assume this is 
		// a hollow crate or barrel and give a penetration bonus
		if (iEnterMaterial == iExitMaterial)
		{
			if (iExitMaterial == CHAR_TEX_WOOD ||
				iExitMaterial == CHAR_TEX_METAL)
			{
				flPenetrationModifier *= 2;
			}
		}

		float flTraceDistance = (exitTr.endpos - tr.endpos).Length();

		// check if bullet has enough power to penetrate this distance for this material
		if (flTraceDistance > (flPenetrationPower * flPenetrationModifier))
			break; // bullet hasn't enough power to penetrate this distance

		// penetration was successful

		//setup new start end parameters for successive trace

		flPenetrationPower -= flTraceDistance / flPenetrationModifier;
		flCurrentDistance += flTraceDistance;

		vecSrc = exitTr.endpos;
		flDistance = (flDistance - flCurrentDistance) * 0.5;

		// reduce damage power each time we hit something other than a grate
		fCurrentDamage *= flDamageModifier;

		// reduce penetration counter
		iPenetration--;
	}

	return false;
}

int Aimbot::LoopCount{ 0 };

CBaseEntity* Aimbot::BestTarget{ nullptr };

bool Aimbot::NoSpreadWasCalled{ false }, Aimbot::NoRecoilWasCalled{ false };

Vector Predicted_Position(CBaseEntity* Entity, bool pLocal) noexcept
{
	if (pLocal)
	{
		return Entity->GetEyePosition() + (Entity->GetVelocity() * Globals->interval_per_tick);
	}
	else
	{
		return Entity->GetHitboxPosition(Menu::Get.Aimbot.Ragebot.HitBox) + (Entity->GetVelocity() * Globals->interval_per_tick);
	}
}

void SmoothedAngles(QAngle& AimPoint, CUserCmd* cmd) noexcept
{
	static auto CurrentAngles{ cmd->viewangles };

	Engine->GetViewAngles(CurrentAngles);

	Vector Angles_Delta{ AimPoint - CurrentAngles };

	Math::ClampAngles(Angles_Delta);

	Angles_Delta /= Menu::Get.Aimbot.Ragebot.SmoothVal;

	AimPoint = Angles_Delta + CurrentAngles;

	Math::ClampAngles(AimPoint);
}

void Aimbot::Run(CBaseEntity* pLocal, CUserCmd* cmd, bool& bSendPacket) noexcept
{
	if (Menu::Get.Aimbot.Ragebot.Enabled)
	{
		auto* ActiveWeapon = pLocal->GetActiveWeapon();

		CBaseEntity* BestEntity = GetClosestEnemy(pLocal);

		static Vector Aim_Angles, ViewAngles_Backup, Diffrences;

		const auto ServerTime = pLocal->GetTickBase() * Globals->interval_per_tick;

		static float LastTickCount = Globals->curtime;

		if ((Globals->curtime >= LastTickCount + (float(Menu::Get.Aimbot.Ragebot.Timeout) / 1000.f) || Menu::Get.Aimbot.Ragebot.Silent) && ActiveWeapon && ActiveWeapon->GetClip1() && !ActiveWeapon->IsKnife())
		{
			if (BestEntity && Menu::Get.Aimbot.Ragebot.AutoCrouch)
			{
				SendButton(IN_DUCK);
			}

			//if (ServerTime > ActiveWeapon->GetNextPrimaryAttack() && ServerTime > pLocal->GetNextAttack())
			{
				if (BestEntity && (IsButtonPressed(IN_ATTACK) || !Menu::Get.Aimbot.Ragebot.Usage))
				{
					ViewAngles_Backup = cmd->viewangles;

					auto Aim_HitBox_Pos = (Menu::Get.Aimbot.Ragebot.Predicted == false ? BestEntity->GetHitboxPosition(Menu::Get.Aimbot.Ragebot.HitBox) : Predicted_Position(BestEntity));

					Diffrences = (Menu::Get.Aimbot.Ragebot.Predicted == false ? pLocal->GetEyePosition() : Predicted_Position(pLocal, true)) - Aim_HitBox_Pos;

					Math::VectorAngles(Diffrences, Aim_Angles);

					QAngle Aim_Angles_Backup{ Aim_Angles };

					if (Menu::Get.Misc.NoRecoil && (IsButtonPressed(IN_ATTACK) || Menu::Get.Aimbot.Ragebot.AutoShoot))
					{
						Aim_Angles -= pLocal->GetPunchAngle() * 2.f;

						NoRecoilWasCalled = true;
					}

					Math::ClampAngles(Aim_Angles);

					if (Menu::Get.Misc.NoSpread)
					{
						const bool NoSpread(CBaseEntity * pLocal, CUserCmd * cmd, QAngle & Angle) noexcept;

						NoSpreadWasCalled = NoSpread(pLocal, cmd, Aim_Angles);
					}

					if (!Menu::Get.Aimbot.Ragebot.Silent)
					{
						SmoothedAngles(Aim_Angles, cmd);

						SmoothedAngles(Aim_Angles_Backup, cmd);
					}

					Aim_Angles.z = 0.0f;

					switch (Menu::Get.Aimbot.Ragebot.Silent)
					{
					case 0:

						Aim_Angles_Backup.z = 0.F;

						LastTickCount = Globals->curtime;

						Engine->SetViewAngles(Aim_Angles_Backup);

						break;

					case 2:

						bSendPacket = false;

						break;
					}

					if (Menu::Get.Aimbot.Ragebot.Silent == 1)
					{
						LoopCount = std::clamp(14 - ClientState->chokedcommands, 0, 14);
					}

					cmd->viewangles = Aim_Angles;

					Math::CorrectMovement(ViewAngles_Backup, cmd, cmd->forwardmove, cmd->sidemove);

					if (Menu::Get.Aimbot.Ragebot.AutoShoot)
					{
						//if (Menu::Get.Aimbot.Ragebot.Silent)
						{
							SendButton(IN_ATTACK);
						}

						if (!ActiveWeapon->IsFullAuto())
						{
							if (pLocal->GetShotsFired() > 0 || ServerTime <= ActiveWeapon->GetNextPrimaryAttack())
							{
								ReleaseButton(IN_ATTACK);
							}
						}
					}

					if (Menu::Get.Aimbot.Ragebot.AutoStop)
					{
						cmd->forwardmove = cmd->sidemove = 0.f;

						// Removing button flags for better Acurrey while using faststop

						ReleaseButton(IN_BACK | IN_FORWARD | IN_MOVELEFT | IN_MOVERIGHT);
					}
				}
			}
		}
	}
}

CBaseEntity* Aimbot::GetClosestEnemy(CBaseEntity* pLocal) noexcept
{
	CBaseEntity* Best_Entity = nullptr;

	float Best_Distance = FLT_MAX;

	float BestFOV = Best_Distance;

	static auto CurrentViewAngles{ QAngle{0,0,0} };

	Engine->GetViewAngles(CurrentViewAngles);

	LoopTroughPlayers()
	{
		auto Entity = GetEntity(i);

		if (Entity
			&& Entity->IsAlive()
			&& Entity->IsPlayer()
			&& !Entity->IsDormant()
			&& Entity NotEquals pLocal
			&& (Entity->IsEnemy(pLocal) || Menu::Get.Aimbot.Ragebot.FriendlyFire)) {

			auto Aim_HitboxPos = Entity->GetHitboxPosition(Menu::Get.Aimbot.Ragebot.HitBox);

			auto delta = pLocal->GetEyePosition() - Aim_HitboxPos;

			static Vector angles;

			Math::VectorAngles(delta, angles);

			Math::ClampAngles(angles);

			if (CanPenetratePoint( pLocal, Entity, angles))
			{
				delta = angles - CurrentViewAngles;

				delta.y = std::remainderf(delta.y, 360.f);

				auto FOV = delta.Length2D();

				if (FOV <= static_cast<float>(Menu::Get.Aimbot.Ragebot.FOV) / 2.f)
				{
					if (!Menu::Get.Aimbot.Ragebot.TargetSelection)
					{
						if (float Current_Distance = Entity->GetAbsOrigin().DistTo(pLocal->GetAbsOrigin()); Current_Distance < Best_Distance)
						{
							Best_Distance = Current_Distance;

							Best_Entity = Entity;
						}
					}
					else
					{
						if (FOV < BestFOV)
						{
							BestFOV = FOV;

							Best_Entity = Entity;
						}
					}
				}
			}
		}
	}

	return Best_Entity;
}