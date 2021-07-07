#include "Miscellaneous.h"
#include "SDK.h"
#include "Assorted.h"
#include "Aimbot.h"

float Since_Last_Used;

std::vector<const char*>ClanTagInfo;

static void TriggerBot(CBaseEntity* pLocal, CUserCmd* cmd) noexcept {

	if (!Menu::Get.Misc.Triggerbot.TB_Enabled
		|| IsButtonPressed(IN_ATTACK)
		|| (!IsVirtualKeyPressed(Menu::Get.Keys.Triggerbot) && Menu::Get.Misc.Triggerbot.TB_Usage))
		return;

	if (auto ActiveWapon = pLocal->GetActiveWeapon(); ActiveWapon && !ActiveWapon->IsKnife() && !ActiveWapon->IsGrenade()) {

		if (auto weapon_data = ActiveWapon->GetWpnData(); weapon_data) {

			ActiveWapon->UpdateAccuracyPenalty();

			auto seed = cmd->random_seed & 0xFF;

			RandomSeed(seed + 1);

			const auto rand1 = RandomFloat(0.f, M_PI_F * 2.f);

			const auto rand2 = RandomFloat(0.f, ActiveWapon->GetInaccuracy());

			const auto v37 = cosf(rand1) * rand2;
			const auto v38 = sinf(rand1) * rand2;

			Vector  spreads[16];

			auto vec_start = pLocal->GetEyePosition() + pLocal->GetVelocity() * Globals->interval_per_tick;

			auto viewangles = cmd->viewangles;

			viewangles -= pLocal->GetPunchAngle() * 2.f;

			for (auto i = 0; i < weapon_data->m_iBullets; i++)
			{
				const auto rand3 = RandomFloat(0.0, M_PI_F * 2.f);

				const auto rand4 = RandomFloat(0.0, ActiveWapon->GetSpread());

				spreads[i].x = cosf(rand3) * rand4;
				spreads[i].y = sinf(rand3) * rand4;

				const auto final_spread_x = spreads[i].x + v37;
				const auto final_spread_y = spreads[i].y + v38;
				Vector vec_dir{};

				Vector forward, right, up;

				Math::AngleVectors(viewangles, &forward, &right, &up);

				vec_dir = forward + (right * final_spread_x) + (up * final_spread_y);
				
				VectorNormalize(vec_dir);

				trace_t tr;

				constexpr float flMaxRange = 8000;

				auto vec_end = vec_start + vec_dir * flMaxRange; // max bullet range is 10000 units

				static auto DidHitNonWorldEntity = reinterpret_cast<bool(__thiscall*)(void*)>(
					CallableFromRelative(Tools::FindPattern("client.dll", "E8 ? ? ? ? 84 C0 75 77")));


				auto iWeaponID = ActiveWapon->GetWeaponID();
				auto iDamage = weapon_data->m_iDamage;
				auto iBulletType = weapon_data->iAmmoType;
				auto flDistance = weapon_data->m_flRange;
				auto flRangeModifier = weapon_data->m_flRangeModifier;
				auto iPenetration = weapon_data->m_iPenetration;

				if (ActiveWapon->GetWeaponMode() == 1)
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

				float flPenetrationPower = 0;		// thickness of a wall that this bullet can penetrate
				float flPenetrationDistance = 0;	// distance at which the bullet is capable of penetrating a wall
				float flDamageModifier = 0.5;		// default modification of bullets power after they go through a wall.
				float flPenetrationModifier = 1.f;

				GetBulletTypeParameters(iBulletType, flPenetrationPower, flPenetrationDistance);
				static auto PhysProps = (IPhysicsSurfaceProps*)Interfaces::GetInterface("vphysics.dll", "VPhysicsSurfaceProps001");

				int Loop = 0; while (Loop++ < 15 && fCurrentDamage > 0)
				{
					Vector vecEnd = vec_start + vec_dir * flDistance;

					trace_t tr;

					UTIL_TraceLine(vec_start, vecEnd, CS_MASK_SHOOT | CONTENTS_HITBOX, pLocal, 0, &tr);
					{
						CTraceFilter filter;
						filter.pSkip = pLocal;
						UTIL_ClipTraceToPlayers(vec_start, vecEnd + vec_dir * 40.0f, CS_MASK_SHOOT | CONTENTS_HITBOX, &filter, &tr);
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

					auto pEntity = reinterpret_cast<CBaseEntity*>(tr.m_pEnt);
					if (pEntity && pEntity->IsPlayer() && pEntity->IsEnemy(pLocal))
					{
						if (auto Damage = ScaleDamage(fCurrentDamage, tr.hitgroup, pEntity->GetArmorValue(),
							pEntity->HasHelmet(), weapon_data->m_flArmorRatio); Damage > 0.1f)
						{
							/*UTIL_TraceLine(vec_start, vec_end, 0x4600400B, pLocal, 0, &tr);

							if (!tr.startsolid && !DidHitNonWorldEntity(&tr))
							{
								Ray_t ray;

								CTraceFilter filter;

								filter.pSkip = pLocal;

								ray.Init(vec_start, vec_end);

								constexpr float rayExtension = 40.0f;

								UTIL_ClipTraceToPlayers(vec_start, vec_end + vec_dir * rayExtension, 0x4600400B, &filter, &tr);
							}*/

							//if (tr.fraction == 1.0f)
								//continue; // we didn't hit anything, stop tracing shoot

							//if (tr.startsolid || !DidHitNonWorldEntity(&tr)) continue;

							auto can_continue = false;

							if (Menu::Get.Misc.Triggerbot.Filter[0] && tr.hitgroup == HITGROUP_HEAD)
								can_continue = true;

							if (Menu::Get.Misc.Triggerbot.Filter[1] && tr.hitgroup == HITGROUP_CHEST)
								can_continue = true;

							if (Menu::Get.Misc.Triggerbot.Filter[2] && tr.hitgroup == HITGROUP_STOMACH)
								can_continue = true;

							if (Menu::Get.Misc.Triggerbot.Filter[3] && (tr.hitgroup == HITGROUP_LEFTARM || tr.hitgroup == HITGROUP_RIGHTARM))
								can_continue = true;

							if (Menu::Get.Misc.Triggerbot.Filter[4] && (tr.hitgroup == HITGROUP_LEFTLEG || tr.hitgroup == HITGROUP_RIGHTLEG))
								can_continue = true;

							if (!can_continue)
								continue;

							matrix3x4 matrix[128];

							if (!pEntity->SetupBones(matrix, 128, 0x00000100, Globals->curtime))
								continue;


							//Get our model studio stuff
							auto model = pEntity->GetModel();
							if (!model)
								continue;


							auto pStudioHdr = ModelInfo->GetStudiomodel(model);
							if (!pStudioHdr)
								continue;


							auto pHitboxSet = pStudioHdr->GetHitboxSet(pEntity->GetHitboxSet());//0 for testing, use netvar later
							if (!pHitboxSet)
								continue;

							for (int i = 0; i < pHitboxSet->numhitboxes; i++)
							{
								auto box = pHitboxSet->GetHitbox(i);
								if (!box)
									continue;

								for (auto CustomPoint : MultipointVectors)
								{
									Vector delta;

									auto NewPoint = CustomPoint + vec_end;

									delta.VectorSubtract(&NewPoint, &vec_start);

									if (IntersectRayWithOBB(vec_start, delta, matrix[box->bone], box->bbmin, box->bbmax))
									{
										SendButton(IN_ATTACK);

										return;
									}
								}
							}

							return;
						}
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
					if (!TraceToExit(tr.endpos, vec_dir, penetrationEnd, 24, 128))
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

					vec_start = exitTr.endpos;
					flDistance = (flDistance - flCurrentDistance) * 0.5;

					// reduce damage power each time we hit something other than a grate
					fCurrentDamage *= flDamageModifier;

					// reduce penetration counter
					iPenetration--;
				}
			}
		}
	}
}

void KnifeBot(CBaseEntity* pLocal, CUserCmd* cmd, CBaseCombatWeapon* ActiveWeapon) noexcept
{
	static auto SwingOrStab = reinterpret_cast<bool(__thiscall*)(void*, bool)>(Tools::FindPattern("client.dll","80 7B 08 00 B8 ? ? ? ? BE ? ? ? ?") - 0x3B);

	if (Menu::Get.Misc.KnifeBot.KB_Usage || IsVirtualKeyPressed(Menu::Get.Keys.KnifeBot))
	{
		if (Menu::Get.Misc.KnifeBot.KB_Enabled)
		{
			if (!SwingOrStab(ActiveWeapon, Menu::Get.Misc.KnifeBot.Method == 1)) return;

			auto m_pTraceHitEnt = *reinterpret_cast<CBaseEntity**>(std::uint32_t(ActiveWeapon) + 2484);

			if (m_pTraceHitEnt && !m_pTraceHitEnt->IsDormant() && m_pTraceHitEnt->IsPlayer() && m_pTraceHitEnt->IsAlive() && m_pTraceHitEnt->IsEnemy(pLocal))
			{
				bool bFirstSwing = (ActiveWeapon->GetNextPrimaryAttack() + 0.4) < Globals->curtime;

				if (Menu::Get.Misc.KnifeBot.Method == 0 || (Menu::Get.Misc.KnifeBot.Method == 2 && m_pTraceHitEnt->GetHealth() <= (bFirstSwing ? 20 : 15)))
				{
					cmd->buttons |= IN_ATTACK;
				}
				else
				{
					if (Menu::Get.Misc.KnifeBot.Method == 2)
					{
						if (!SwingOrStab(ActiveWeapon, true)) return;
					}

					cmd->buttons |= IN_ATTACK2;
				}
			}
		}
	}
}

__forceinline float get_ideal_rotation(float speed) {
	// 15.f is the ideal angle of rotation
	float factor = RAD2DEG(std::atan2(15.f, speed));
	// clamp it from 0 to 45 for maximum movement
	return std::clamp(factor, 0.f, 45.f) * Globals->interval_per_tick;
}

__forceinline void RotateMovement(CUserCmd* cmd, float yaw_to_rotate_towards)
{
	const float rot = DEG2RAD(cmd->viewangles.y - yaw_to_rotate_towards);

	const float new_forward = (std::cos(rot) * cmd->forwardmove) - (std::sin(rot) * cmd->sidemove);

	const float new_side = (std::sin(rot) * cmd->forwardmove) + (std::cos(rot) * cmd->sidemove);

	cmd->forwardmove = new_forward;

	cmd->sidemove = new_side;
}

void AutoStrafe(CBaseEntity* pLocal, CUserCmd* cmd) noexcept
{
	if (Menu::Get.Misc.Autostrafe && IsVirtualKeyPressed(Menu::Get.Keys.Autostrafe))
	{
		const auto IN_MOVE = IsButtonPressed(IN_FORWARD | IN_BACK | IN_MOVELEFT | IN_MOVERIGHT);

		if (IN_MOVE
			|| pLocal->IsDead()
			|| pLocal->OnGround()
			|| pLocal->GetMoveType() NotEquals MOVETYPE_WALK)
			return;

		static bool side_switch = false;

		side_switch = !side_switch;

		cmd->forwardmove = 0.f;

		cmd->sidemove = side_switch ? 450.f : -450.f;

		QAngle velocity_angle;

		Vector velocity = pLocal->GetVelocity();

		Math::VectorAngles(velocity, velocity_angle);

		float velocity_yaw = velocity_angle.y;

		float ideal_rotation = get_ideal_rotation(velocity.Length2D()) / Globals->interval_per_tick;

		if (!side_switch)
			ideal_rotation *= -1.f;

		float delta = velocity_yaw - cmd->viewangles.y;

		Math::NormalizeAngle(delta);

		float ideal_yaw_rot = std::fabsf(delta) < 5.f ? ideal_rotation + velocity_yaw : ideal_rotation + cmd->viewangles.y;

		RotateMovement(cmd, ideal_yaw_rot);

		/*if (pLocal->GetVelocity().Length() < 50.0f)
		{
			cmd->forwardmove = 400;
		}
		else
		{
			cmd->forwardmove = 0;
		}

		QAngle Controlled_Creature_Desired_Move_View_Angles = cmd->viewangles;

		QAngle Controlled_Creature_Velocity_Angles;

		Math::VectorAngles(pLocal->GetVelocity(), Controlled_Creature_Velocity_Angles);

		float Controlled_Creature_Velocity_Angle_Y_Difference = remainderf(cmd->viewangles.y - Controlled_Creature_Velocity_Angles.y, 360);

		if (Controlled_Creature_Velocity_Angle_Y_Difference != 0)
		{
			if (Controlled_Creature_Velocity_Angle_Y_Difference > 0)
			{
				cmd->sidemove = -400;
			}
			else
			{
				cmd->sidemove = 400;
			}

			Controlled_Creature_Desired_Move_View_Angles.y = remainderf(Controlled_Creature_Desired_Move_View_Angles.y - Controlled_Creature_Velocity_Angle_Y_Difference, 360);

			Math::CorrectMovement(Controlled_Creature_Desired_Move_View_Angles, cmd, cmd->forwardmove, cmd->sidemove);
		}
		else if (cmd->mousedx > 1)
		{
			cmd->sidemove = 400;
		}
		else if (cmd->mousedx < -1)
		{
			cmd->sidemove = -400;
		}*/
	}
}

void CircleStrafe(CBaseEntity* pLocal, CUserCmd* cmd) noexcept
{
	Ray_t Ray;

	trace_t Trace_t;

	CTraceFilter Filter;

	static Vector Dir;

	static int Angle = 0;

	if (Menu::Get.Misc.CircleStrafe && pLocal->InAir())
	{
		if (IsVirtualKeyPressed(Menu::Get.Keys.CircleStrafe))
		{
			if (cmd->viewangles.y - Angle > 360) Angle -= 360;

			Math::AngleVectors(Vector(0, Angle, 0), Dir);

			Dir *= 8218;

			Ray.Init(pLocal->GetEyePosition(), pLocal->GetVecOrigin() + Dir);

			Trace->TraceRay(Ray, MASK_SHOT, &Filter, &Trace_t);

			auto temp = 3.4f / ((Trace_t.endpos - pLocal->GetVecOrigin()).Length() / 100.f);

			if (temp < 3.4f) temp = 3.4f;

			Angle += temp;

			cmd->sidemove = -450.f;

			cmd->viewangles.y = Angle;
		}
		else
		{
			Angle = cmd->viewangles.y;
		}
	}
}

void StrafeOptimizer(CBaseEntity* pLocal, CUserCmd* cmd)
{
	static auto m_yaw = Cvar->FindVar("m_yaw");

	static auto sensitivity = Cvar->FindVar("sensitivity");

	static float Previous_View_Angles_Y = cmd->viewangles.y;

	if (constexpr static auto& StrafeOptimizer = Menu::Get.Misc.StrafeOptimizer; StrafeOptimizer.Enabled)
	{
		if (cmd->forwardmove == 0 && IsButtonPressed(IN_JUMP)) if (auto Velocity = pLocal->GetVelocity(); Velocity.Length2D() > StrafeOptimizer.Required_Speed)
		{
			Prediction->Update(ClientState->GetDeltaTick(), ClientState->GetDeltaTick() > 0, ClientState->last_command_ack, ClientState->lastoutgoingcommand + ClientState->chokedcommands);

			float Mouse_Yaw_Factor = m_yaw->GetFloat();

			float Mouse_Sensitivity = sensitivity->GetFloat();

			float Mouse_Yaw_Step = Mouse_Sensitivity * Mouse_Yaw_Factor;

			if (cmd->sidemove < 0)
			{
				if (Previous_View_Angles_Y - cmd->viewangles[1] < 0)
				{
					float Strafe_Angle = remainderf(cmd->viewangles[1] - atan2f(Velocity[1], Velocity[0]) * 180 / M_PI, 360) * StrafeOptimizer.Desired_Gain / 100;

					if (Strafe_Angle < -Mouse_Yaw_Step)
					{
						if (Strafe_Angle < -StrafeOptimizer.Greatest_Possible_Strafe_Angle)
						{
							Strafe_Angle = -StrafeOptimizer.Greatest_Possible_Strafe_Angle;
						}

						Previous_View_Angles_Y = cmd->viewangles[1];

						cmd->viewangles[1] = remainderf(cmd->viewangles[1] - Mouse_Yaw_Step * roundf(Strafe_Angle / Mouse_Yaw_Step), 360);

						cmd->mousedx = (__int16)(Mouse_Sensitivity * ceilf(remainderf(Previous_View_Angles_Y - cmd->viewangles[1], 360) / sqrtf(Mouse_Yaw_Step)));

						Engine->SetViewAngles(cmd->viewangles);
					}
				}
			}
			else
			{
				if (cmd->sidemove > 0)
				{
					if (Previous_View_Angles_Y - cmd->viewangles[1] > 0)
					{
						float Strafe_Angle = remainderf(cmd->viewangles[1] - atan2f(Velocity[1], Velocity[0]) * 180 / M_PI, 360) * StrafeOptimizer.Desired_Gain / 100;

						if (Strafe_Angle > Mouse_Yaw_Step)
						{
							if (Strafe_Angle > StrafeOptimizer.Greatest_Possible_Strafe_Angle)
							{
								Strafe_Angle = StrafeOptimizer.Greatest_Possible_Strafe_Angle;
							}

							Previous_View_Angles_Y = cmd->viewangles[1];

							cmd->viewangles[1] = remainderf(cmd->viewangles[1] - Mouse_Yaw_Step * roundf(Strafe_Angle / Mouse_Yaw_Step), 360);

							cmd->mousedx = (__int16)(Mouse_Sensitivity * ceilf(remainderf(Previous_View_Angles_Y - cmd->viewangles[1], 360) / sqrtf(Mouse_Yaw_Step)));

							Engine->SetViewAngles(cmd->viewangles);
						}
					}
				}
			}
		}

		Previous_View_Angles_Y = cmd->viewangles[1];
	}
}

static const bool NoRecoil(CBaseEntity* pLocal, QAngle& Angle) noexcept {

	if (Menu::Get.Misc.NoRecoil)
	{
		Angle -= pLocal->GetPunchAngle() * 2.0f;

		Math::ClampAngles(Angle);

		return true;
	}

	return false;
}

const bool NoSpread(CBaseEntity* pLocal, CUserCmd* cmd, QAngle& Angle) noexcept {

	static float Random[4]{ 0.f, 0.f, 0.f, 0.f };

	if (const auto ActiveWeapon = pLocal->GetActiveWeapon(); Menu::Get.Misc.NoSpread && IsButtonPressed(IN_ATTACK) && ActiveWeapon)
	{
		ActiveWeapon->UpdateAccuracyPenalty();

		RandomSeed((cmd->random_seed & 0xFF) + 1);

		Random[0] = RandomFloat(0.f, M_PI_F * 2.f);

		Random[1] = RandomFloat(0.f, ActiveWeapon->GetInaccuracy());

		Random[2] = RandomFloat(0.f, M_PI_F * 2.f);

		Random[3] = RandomFloat(0.f, ActiveWeapon->GetSpread());

		const auto v37 = cos(Random[0]) * Random[1];

		const auto v38 = sin(Random[0]) * Random[1];

		const auto spread_x = cos(Random[2]) * Random[3] + v37;

		const auto spread_y = sin(Random[2]) * Random[3] + v38;

		Vector IN_FORWARD, vecRight, vecUp;

		Math::AngleVectors(Angle, &IN_FORWARD, &vecRight, &vecUp);

		auto vecDir = IN_FORWARD + vecRight * spread_x * -1.f + vecUp * spread_y * -1.f;

		VectorNormalize(vecDir);

		Math::VectorAngles(-vecDir, Angle);

		Math::ClampAngles(Angle);

		return true;
	}

	return false;
}

void Bullet_Accuracy(CBaseEntity* pLocal, CUserCmd* cmd, bool& bSendPacket) noexcept {

	if (const auto ActiveWeapon = pLocal->GetActiveWeapon(); ActiveWeapon) {

		if (const float ServerTime = pLocal->GetTickBase() * Globals->interval_per_tick; ServerTime > ActiveWeapon->GetNextPrimaryAttack() && ServerTime > pLocal->GetNextAttack()) {

			const auto SideMove_Backup = cmd->sidemove; const auto ViewAngles_Backup = cmd->viewangles; const auto ForwardMove_Backup = cmd->forwardmove;
			
			if (ActiveWeapon->GetClip1()) {

				if (IsButtonPressed(IN_ATTACK))
				{
					if (!Aimbot::NoRecoilWasCalled)
						Aimbot::NoRecoilWasCalled = NoRecoil(pLocal, cmd->viewangles);

					if (!Aimbot::NoSpreadWasCalled)
						Aimbot::NoSpreadWasCalled = NoSpread(pLocal, cmd, cmd->viewangles);

					if (Aimbot::NoRecoilWasCalled ||
						Aimbot::NoSpreadWasCalled) {

						Aimbot::NoRecoilWasCalled = Aimbot::NoSpreadWasCalled = bSendPacket = false;

						Math::CorrectMovement(ViewAngles_Backup, cmd, ForwardMove_Backup, SideMove_Backup);
					}
				}
			}
			else if (ActiveWeapon->IsFullAuto())
			{
				ReleaseButton(IN_ATTACK);
			}
		}
	}
}

void ClanTagSpammer(CBaseEntity* pLocal) noexcept
{
	static int Stealer_Last_Used_Entity_Index{ 0 };

	static int Changer_Last_Used_Clan_Index{ 0 };

	if (Globals->curtime > Menu::Get.Misc.ClanTag.Delay + Since_Last_Used)
	{
		if (pLocal->GetTeamNum() > 1)
		{
			if (Menu::Get.Misc.ClanTag.Stealer)
			{
				CBaseEntity* Entity = GetEntity(Stealer_Last_Used_Entity_Index);

				if (Entity && Entity NotEquals pLocal && Entity->IsPlayer())
				{
					if (auto ClanTag = PlayerResource->GetClanTag(Stealer_Last_Used_Entity_Index); ClanTag)
					{
						// Check if there is one valid char

						for (unsigned int i = 0; i < 10; i++)
						{
							if (ClanTag[i])
							{
								Since_Last_Used = Globals->curtime;

								SetClanTag(ClanTag);

								Stealer_Last_Used_Entity_Index++;

								if (Stealer_Last_Used_Entity_Index >= Engine->GetMaxClients())
								{
									Stealer_Last_Used_Entity_Index = 0;
								}

								return;
							}
						}
					}
				}

				Stealer_Last_Used_Entity_Index++;

				if (Stealer_Last_Used_Entity_Index >= Engine->GetMaxClients())
				{
					Stealer_Last_Used_Entity_Index = 0;
				}
			}

			if (Menu::Get.Misc.ClanTag.Changer)
			{
				if (Changer_Last_Used_Clan_Index < ClanTagInfo.size() && Changer_Last_Used_Clan_Index >= 0)
				{
					SetClanTag(ClanTagInfo[Changer_Last_Used_Clan_Index]);

					Since_Last_Used = Globals->curtime;

					Changer_Last_Used_Clan_Index++;
				}
				else if (Changer_Last_Used_Clan_Index)
				{
					Changer_Last_Used_Clan_Index = 0;
				}
			}
		}
	}
}

void FastLadderClimb(CBaseEntity* pLocal, CUserCmd* cmd, bool& bSendPacket) noexcept
{
	if (Menu::Get.Misc.FastLadder && pLocal->GetMoveType() == MOVETYPE_LADDER)
	{
		if (IsButtonPressed(IN_FORWARD) && !IsButtonPressed(IN_MOVELEFT | IN_MOVERIGHT) && cmd->mousedy < 2 && cmd->mousedy > -2)
		{
			float Pitch{ 89.0f }; Vector Angles;

			Engine->GetViewAngles(Angles); if (Angles.x < 30.F) { Pitch = -89.0f; }

			if ((Angles.y > 135 && Angles.y < 180) || (Angles.y < -135 && Angles.y > -180.0f))
			{
				cmd->viewangles.y = -90.64f;
			}
			else if ((Angles.y < 135 && Angles.y > 90) || (Angles.y < 90 && Angles.y > 45))
			{
				cmd->viewangles.y = 179.150f;
			}
			else if ((Angles.y > -90 && Angles.y < -45) || (Angles.y < -90 && Angles.y > -135))
			{
				cmd->viewangles.y = -1.20f;
			}
			else if ((Angles.y < -0 && Angles.y > -45) || (Angles.y < 45 && Angles.y > 0))
			{
				cmd->viewangles.y = 89.20f;
			}

			bSendPacket = false;

			cmd->viewangles.x = Pitch;

			cmd->buttons |= IN_MOVERIGHT;

			cmd->sidemove = cmd->forwardmove = 0.f;
		}
	}
}


void FastRun(CBaseEntity* pLocal, CUserCmd* cmd) noexcept {

	static bool Run = false;

	if (pLocal->GetMoveType() != MOVETYPE_WALK) {

		return;
	}

	if (Menu::Get.Misc.Fastrun && (pLocal->OnGround() || pLocal->GetWaterLevel() >= 2) && (!Menu::Get.Misc.Bunnyhop || !IsVirtualKeyPressed(Menu::Get.Keys.Bunnyhop)))
	{
		if ((IsButtonPressed(IN_FORWARD) && IsButtonPressed(IN_MOVELEFT)) || (IsButtonPressed(IN_BACK) && IsButtonPressed(IN_MOVERIGHT)))
		{
			cmd->sidemove += Run ? -205.6002f : 205.6002f;

			cmd->forwardmove += Run ? -205.6002f : 205.6002f;
		}
		else if ((IsButtonPressed(IN_FORWARD) && IsButtonPressed(IN_MOVERIGHT)) || (IsButtonPressed(IN_BACK) && IsButtonPressed(IN_MOVELEFT)))
		{
			cmd->sidemove += Run ? -205.6002f : 205.6002f;

			cmd->forwardmove += Run ? 205.6002f : -205.6002f;
		}
		else if (IsButtonPressed(IN_FORWARD | IN_BACK))
		{
			cmd->sidemove += Run ? -205.6002f : 205.6002f;
		}
		else if (IsButtonPressed(IN_MOVELEFT | IN_MOVERIGHT))
		{
			cmd->forwardmove += Run ? 205.6002f : -205.6002f;
		}

		Run = !Run;
	}
}

void Faststop(CBaseEntity* pLocal, CUserCmd* cmd) noexcept {

	if (pLocal->GetMoveType() != MOVETYPE_WALK) {

		return;
	}

	if (Menu::Get.Misc.Faststop && pLocal->OnGround() ) {

		constexpr auto Value = 0.217812;

		const auto CurrentOrigin{ pLocal->GetAbsOrigin() }; static auto OldOrigin = CurrentOrigin;

		Vector Direction, FORWARD;

		bool InMove(IsButtonPressed(IN_BACK
			| IN_FORWARD
			| IN_MOVELEFT
			| IN_MOVERIGHT));

		if (!InMove && (!Menu::Get.Misc.Bunnyhop || !IsVirtualKeyPressed(Menu::Get.Keys.Bunnyhop))) {

			Vector PlayerSpeed = CurrentOrigin - OldOrigin; PlayerSpeed *= 350;

			Math::VectorToAngles(PlayerSpeed, Direction);

			Direction.y = cmd->viewangles.y - Direction.y;

			Math::AngleToVectors(Direction, FORWARD);

			if (float Speed = PlayerSpeed.Length2D(); Speed > 0.1f)
			{
				Vector Left_and_Back = (FORWARD + -Value) * -Speed;

				Vector Right_and_Forward = (FORWARD + Value) * -Speed;

				cmd->sidemove += +Left_and_Back.y; cmd->forwardmove -= -Left_and_Back.x;

				cmd->sidemove -= -Right_and_Forward.y; cmd->forwardmove += +Right_and_Forward.x;
			}
		}

		OldOrigin = CurrentOrigin;
	}
}

float Global_Jumps{ 0 }, Perfect_Jumps{ 0 };

void BunnyHop(CBaseEntity* pLocal, CUserCmd* cmd) noexcept
{
	class ClientModeShared
	{
	public:
		bool IsChatPanelOutOfFocus(void)
		{
			static auto CHudChat = Get_vFunction<uintptr_t(__thiscall*)(void*)>(this, 19)(this);

			if (CHudChat)
			{
				return *(float*)(CHudChat + 0xFC) == 0;
			}

			return false;
		}
	};

#define IsPlayingMode (((ClientModeShared*)ClientMode)->IsChatPanelOutOfFocus() && !Engine->Con_IsVisible())

	static int Next_Jump_Tick{ 0 }, Next_Release_Tick{ 0 }, Random_Forward_ticks{ 0 };

	static auto Last_Logged_Time{ Globals->curtime };

	static bool FirstJump{ true }, WasPassed{ false };

	if (Menu::Get.Misc.Bunnyhop && pLocal->GetMoveType() Equals MOVETYPE_WALK)
	{
		if (IsVirtualKeyPressed(Menu::Get.Keys.Bunnyhop) && IsPlayingMode)
		{
			if (pLocal->GetWaterLevel() >= 2) { SendButton(IN_JUMP); return; }

			const auto PerfectRate{ (Perfect_Jumps / Global_Jumps) * 100.f };

			if (!FirstJump && Was_On_Ground_Backup && Globals->curtime < Last_Logged_Time + 0.0375 && PerfectRate > static_cast<float>(Menu::Get.Misc.Bunnyhop_Perfect_Rate))
			{
				WasPassed = true;
			}
			else if (Was_On_Ground_Backup)
			{
				if (!WasPassed)
				{
					Perfect_Jumps++;
				}
				else
				{
					WasPassed = false;
				}

				Global_Jumps++;

				SendButton(IN_JUMP);

				FirstJump = Next_Release_Tick = 0;

				Random_Forward_ticks = rand() % (5 - 3 + 1) + 3;
			}
			else if (!pLocal->OnGround())
			{
				if (cmd->tick_count >= Next_Jump_Tick && cmd->tick_count < Next_Release_Tick)
				{
					SendButton(IN_JUMP);
				}
				else
				{
					if (cmd->tick_count >= Next_Release_Tick)
					{
						Next_Jump_Tick = cmd->tick_count + Random_Forward_ticks;

						Next_Release_Tick = cmd->tick_count + (Random_Forward_ticks * 2);
					}

					ReleaseButton(IN_JUMP);
				}

				Last_Logged_Time = Globals->curtime;
			}
		}
		else if (!FirstJump)
		{
			FirstJump = true;
		}
	}
}

void EdgeJump(CBaseEntity* pLocal, CUserCmd* cmd) noexcept
{
	if (!Menu::Get.Misc.Edgejump
		|| pLocal->OnGround()
		|| pLocal->GetMoveType() != MOVETYPE_WALK
		|| !IsVirtualKeyPressed(Menu::Get.Keys.Edgejump)
		|| !Was_On_Ground_Backup)
		return;

	cmd->buttons |= IN_JUMP;
}

bool Toggled_On{ false };

void Auto_Kevlar(CBaseEntity* pLocal) noexcept {

	if (!Toggled_On
		&& Menu::Get.Misc.AutoKevlar
		&& pLocal->GetAccount() >= 1000
		&& pLocal->GetArmorValue() <= 15
		&& pLocal->GetTeamNum() == Counter_Terrorist) {

		Toggled_On = !Toggled_On;

		Engine->ClientCmd_Unrestricted("sm_kevlar");
	}

	if (Toggled_On && pLocal->GetArmorValue() > 15)
	{
		Toggled_On = !Toggled_On;
	}
}

static void AntiAfkKick(CUserCmd* cmd) noexcept {

	static unsigned short Counter{ 0 };

	if (Menu::Get.Misc.AntiAFKkick && Counter++ >= 100) {

		Counter = NULL;

		SendButton(1 << 26);
	}
}

static void MultiFunc(CBaseEntity* pLocal, CUserCmd* cmd) noexcept
{
	if (const auto ActiveWeapon = pLocal->GetActiveWeapon(); ActiveWeapon) 
	{
		const auto IsKnife = ActiveWeapon->IsKnife();

		const auto IsGrenade = ActiveWeapon->IsGrenade();

		const float ServerTime = pLocal->GetTickBase() * Globals->interval_per_tick;

		if (ServerTime > ActiveWeapon->GetNextPrimaryAttack() && ServerTime > pLocal->GetNextAttack())
		{
			if (IsKnife)
			{
				KnifeBot(pLocal, cmd, ActiveWeapon);
			}
			else
			{
				if (!IsGrenade)
				{
					TriggerBot(pLocal, cmd);
				}
			}
		}
		else if (Menu::Get.Misc.AutoPisol && !ActiveWeapon->IsFullAuto())
		{
			if (IsButtonPressed(IN_ATTACK) && pLocal->GetShotsFired() > 0)
			{
				ReleaseButton(IN_ATTACK);
			}
		}
	}
}

void Miscellaneous::Run(CBaseEntity* pLocal, CUserCmd* cmd, bool& bSendPacket) noexcept {

	AntiAfkKick(cmd);

	if (pLocal->IsAlive()) {

		FastRun(pLocal, cmd);

		Faststop(pLocal, cmd);

		MultiFunc(pLocal, cmd);

		ClanTagSpammer(pLocal);

		AutoStrafe(pLocal, cmd);

		CircleStrafe(pLocal, cmd);

		FastLadderClimb(pLocal, cmd, bSendPacket);
	}
}

void Miscellaneous::EnginePrediction(CBaseEntity* pLocal, CUserCmd* cmd) noexcept
{
	class IGameMovement
	{
	public:
		virtual			~IGameMovement(void) {}

		// Process the current movement command
		virtual void	ProcessMovement(CBaseEntity* pPlayer, CMoveData* pMove) = 0;
		virtual void	StartTrackPredictionErrors(CBaseEntity* pPlayer) = 0;
		virtual void	FinishTrackPredictionErrors(CBaseEntity* pPlayer) = 0;
		virtual void	DiffPrint(char const* fmt, ...) = 0;

		// Allows other parts of the engine to find out the normal and ducked player bbox sizes
		virtual Vector const& GetPlayerMins(bool ducked) const = 0;
		virtual Vector const& GetPlayerMaxs(bool ducked) const = 0;
		virtual Vector const& GetPlayerViewOffset(bool ducked) const = 0;
	};

	static auto g_pMoveData = *reinterpret_cast<CMoveData**>(
		Tools::FindPattern("client.dll", "FF 35 ? ? ? ? 57") + 2);
	static auto m_nPredictionRandomSeed = *reinterpret_cast<int**>(
		Tools::FindPattern("client.dll", "A3 ? ? ? ? 5D C3 55 8B EC 8B 45 08") + 1);
	static auto m_pPredictionPlayer = *reinterpret_cast<CBaseEntity***>(
		Tools::FindPattern("client.dll", "89 3D ? ? ? ? F3 0F 2A 87") + 2);

	auto LocalPlayer_Address = reinterpret_cast<std::uintptr_t>(pLocal);

	*reinterpret_cast<CUserCmd**>(LocalPlayer_Address + 0x101C) = cmd;

	*m_nPredictionRandomSeed = cmd->random_seed;

	*m_pPredictionPlayer = pLocal;

	if (*reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(Prediction) + 0xB))
		Globals->frametime = 0.0;
	else
		Globals->frametime = Globals->interval_per_tick;

	((IGameMovement*)GameMovement)->StartTrackPredictionErrors(pLocal);

	if (cmd->impulse)
	{
		*reinterpret_cast<DWORD*>(LocalPlayer_Address + 0x10C4) = cmd->impulse;
	}

	pLocal->UpdateButtonState(cmd->buttons);

	pLocal->SetLocalViewAngles(cmd->viewangles);

	Prediction->SetupMove(pLocal, cmd, MoveHelper, g_pMoveData);

	((IGameMovement*)GameMovement)->ProcessMovement(pLocal, g_pMoveData);

	Prediction->FinishMove(pLocal, cmd, g_pMoveData);

	((IGameMovement*)GameMovement)->FinishTrackPredictionErrors(pLocal);

	*reinterpret_cast<CUserCmd**>(LocalPlayer_Address + 0x101C) = nullptr;

	*m_nPredictionRandomSeed = -1;

	*m_pPredictionPlayer = nullptr;
}
