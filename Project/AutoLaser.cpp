#include "ClientEntityListener.h"
#include "AutoLaser.h"
#include "Assorted.h"
#include <fstream>

std::vector<Entities> EntitiesListener;

auto Last_LocalPlayer_Roll_Position{ 0.f };

#define GAMEMOVEMENT_JUMP_TIME				510.0f
#define GAMEMOVEMENT_DUCK_TIME				1000.0f

void Update_LocalPlayer_Roll(bool OnGround,const float& MyRoll) noexcept
{
	if (OnGround)
	{
		if (MyRoll != Last_LocalPlayer_Roll_Position)
		{
			Last_LocalPlayer_Roll_Position = MyRoll;
		}
	}
}

void Set_Action_Or_Release_Tick
(
	Vector EntityVelocity,
	Vector EntityOrigin,
	Vector LocalOrigin,
	Entities& EntityInfo,
	int Tickcount,
	bool JumpLaser,
	Vector Maxs,
	int CreationTick = 0,
	bool ActionTick = false
)
{
	bool Yaw = MakeItPositive(EntityVelocity.y) > MakeItPositive(EntityVelocity.x);

	auto DeltaOrigin = (EntityOrigin - LocalOrigin).Lengthed();

	auto DeltaSpeed = (EntityVelocity * TICK_INTERVAL).Lengthed();

	std::int32_t Iteration = 0;

	if (!ActionTick)
	{
		if (Yaw)
		{
			Iteration = (DeltaOrigin.y / DeltaSpeed.y);
		}
		else
		{
			Iteration = (DeltaOrigin.x / DeltaSpeed.x);
		}


		EntityInfo.ReleaseTick = Tickcount + Iteration + 1;
	}
	else
	{
#define ExtraTime 1.75f

#define GetPing(Ping) Ping * 1000.f

		static float Latency, AdditionTime; Latency = AdditionTime = NULL;

		if (auto NetChan = ClientState->m_NetChannel; NetChan) Latency += NetChan->GetLatency(FLOW_INCOMING) + NetChan->GetLatency(FLOW_OUTGOING);

		AdditionTime += (JumpLaser ? GAMEMOVEMENT_JUMP_TIME : GAMEMOVEMENT_DUCK_TIME * ExtraTime) + GetPing(Latency);

		if (Yaw)
		{
			Iteration = (DeltaOrigin.y / DeltaSpeed.y) - DeltaSpeed.y;
		}
		else
		{
			Iteration = (DeltaOrigin.x / DeltaSpeed.x) - DeltaSpeed.x;
		}

		AdditionTime += Maxs[Yaw] * 3.5f;

		EntityInfo.Action_Tick = (unsigned long)(Tickcount + Iteration - std::round(TICKS_TO_TIME(AdditionTime))) - (Globals->tickcount - (CreationTick + EntityInfo.IgnoreTicks));
	}
}

struct CCurvedList
{
	enum
	{
		LeftUp,
		LeftDown
	};

	struct Content
	{
		int ModenIndex; unsigned char ID;
	};

	const char* Map; std::vector<Content>pContent;
};

std::vector<CCurvedList>CurvedList{};

void FindInCurvedList(int ModelIndex,
	Vector EntityVelocity,
	Vector EntityOrigin,
	Vector LocalOrigin,
	Entities& Entity,
	bool& JumpLaser)
{
	auto MapName = ClientState->GetLevelNameShort();

	for (auto& Curved : CurvedList)
	{
		if (FindStringCS(MapName, Curved.Map))
		{
			for (auto& pContent : Curved.pContent)
			{
				if (ModelIndex == pContent.ModenIndex)
				{
					if (bool Yaw = MakeItPositive(EntityVelocity.y) > MakeItPositive(EntityVelocity.x); Yaw)
					{
						if (Entity.Origin.y < LocalOrigin.y)
						{
							if (pContent.ID == CCurvedList::LeftDown)
							{
								if (LocalOrigin.x <= EntityOrigin.x + -73)
								{
									JumpLaser = false;
								}
							}
							else
							{
								if (LocalOrigin.x >= EntityOrigin.x + 73)
								{
									JumpLaser = false;
								}
							}
						}
						else if (Entity.Origin.y > LocalOrigin.y)
						{
							if (pContent.ID == CCurvedList::LeftDown)
							{
								if (LocalOrigin.x >= EntityOrigin.x + 73)
								{
									JumpLaser = false;
								}
							}
							else
							{
								if (LocalOrigin.x <= EntityOrigin.x + -73)
								{
									JumpLaser = false;
								}
							}
						}
					}
					else
					{
						if (Entity.Origin.x < LocalOrigin.x)
						{
							if (pContent.ID == CCurvedList::LeftUp)
							{
								if (LocalOrigin.y <= EntityOrigin.y + -73)
								{
									JumpLaser = false;
								}
							}
							else
							{
								if (LocalOrigin.y >= EntityOrigin.y + 73)
								{
									JumpLaser = false;
								}
							}
						}
						else if (Entity.Origin.x > LocalOrigin.x)
						{
							if (pContent.ID == CCurvedList::LeftUp)
							{
								if (LocalOrigin.y >= EntityOrigin.y + 73)
								{
									JumpLaser = false;
								}
							}
							else
							{
								if (LocalOrigin.y <= EntityOrigin.y + -73)
								{
									JumpLaser = false;
								}
							}
						}
					}

					return;
				}
			}
		}
	}
}

class CViewVectors
{
public:
	CViewVectors() {}

	CViewVectors(
		Vector vView,
		Vector vHullMin,
		Vector vHullMax,
		Vector vDuckHullMin,
		Vector vDuckHullMax,
		Vector vDuckView,
		Vector vObsHullMin,
		Vector vObsHullMax,
		Vector vDeadViewHeight)
	{
		m_vView = vView;
		m_vHullMin = vHullMin;
		m_vHullMax = vHullMax;
		m_vDuckHullMin = vDuckHullMin;
		m_vDuckHullMax = vDuckHullMax;
		m_vDuckView = vDuckView;
		m_vObsHullMin = vObsHullMin;
		m_vObsHullMax = vObsHullMax;
		m_vDeadViewHeight = vDeadViewHeight;
	}

	// Height above entity position where the viewer's eye is.
	Vector m_vView;

	Vector m_vHullMin;
	Vector m_vHullMax;

	Vector m_vDuckHullMin;
	Vector m_vDuckHullMax;
	Vector m_vDuckView;

	Vector m_vObsHullMin;
	Vector m_vObsHullMax;

	Vector m_vDeadViewHeight;
};

void AutoLaser::Run(CBaseEntity* pLocal, CUserCmd* cmd, bool& bSendPacket) noexcept
{
	[[maybe_unused]] static const auto Once = []() noexcept
	{
		CurvedList.push_back({ "ze_ffvii_cosmo_traning_v2_1" });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 8, CCurvedList::LeftDown });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 17, CCurvedList::LeftUp });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 13, CCurvedList::LeftDown });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 12, CCurvedList::LeftUp });

		CurvedList.push_back({ "ze_ffvii_mako_reactor_v5_3" });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 235, CCurvedList::LeftDown });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 273, CCurvedList::LeftUp });

		CurvedList.push_back({ "ze_ffvii_cosmo_canyon_v5fix" });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 326, CCurvedList::LeftDown });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 325, CCurvedList::LeftUp });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 364, CCurvedList::LeftDown });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 363,CCurvedList::LeftUp });

		CurvedList.push_back({ "ze_mountain_escape_bbo_v1" });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 391, CCurvedList::LeftUp });

		CurvedList.push_back({ "ze_otakuroom_v5_6fs" });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 37, CCurvedList::LeftUp });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 36, CCurvedList::LeftDown });

		CurvedList.push_back({ "ze_666_crazy_escape_b3s" });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 19, CCurvedList::LeftUp });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 27, CCurvedList::LeftUp });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 21, CCurvedList::LeftDown });

		CurvedList[CurvedList.size() - 1].pContent.push_back({ 28, CCurvedList::LeftDown });

		return 1;
	}();


	static auto g_pGameRules = **reinterpret_cast<void***>(Tools::FindPattern("client.dll", "8B EC 8B 0D ? ? ? ? 85 C9 74 07") + 4);

	static auto GetViewVectors = reinterpret_cast<CViewVectors * (__thiscall*)(void*)>(Tools::FindPattern("client.dll", "DB 41 24") + 0x10);

	static auto& m_vView = GetViewVectors(g_pGameRules)->m_vView;

	static auto& m_vDuckView = GetViewVectors(g_pGameRules)->m_vDuckView;

	if (Menu::Get.General.AutoLaser && pLocal->IsAlive() && pLocal->GetTeamNum() == Counter_Terrorist) {

		bool OnGround = pLocal->OnGround();

		Vector LocalOrigin = pLocal->GetAbsOrigin(), MyVelocity = pLocal->GetVelocity(), Mins = pLocal->GetMins(), Maxs = pLocal->GetMaxs(); Update_LocalPlayer_Roll(OnGround, LocalOrigin.z);

		for (auto Iteration = NULL; Iteration < EntitiesListener.size(); Iteration++)
		{
			auto& EntityInfo = EntitiesListener[Iteration]; auto Entity = GetEntity(EntityInfo.userid);

			if (!Entity || Entity->IsDormant() || (Entity->GetMoveType() != MOVETYPE_NONE && Entity->GetMoveType() != MOVETYPE_PUSH) || Entity->IsPlayer())
			{
				EntitiesListener.erase(EntitiesListener.begin() + Iteration);
			}
			else if (!EntityInfo.Init)
			{
				auto pParent = EntityList->GetClientEntityFromHandle(PVOID(Entity->GetNetworkMoveParent()));

				if (EntityInfo.Origin = (pParent ? pParent->GetAbsOrigin() : Entity->GetAbsOrigin()); EntityInfo.Origin.z)
				{
					EntityInfo.Init = !EntityInfo.Init;

					EntityInfo.LocalPlayerOrigin = LocalOrigin;

					// TODO Try to find your way to define lasers

					// the method below works but not for all maps!

					/*IMaterial* pMaterial = nullptr; ModelInfo->GetModelMaterials(Entity->GetModel(), 1, &pMaterial);

					static auto white = MatSystemOther->FindMaterial("cs_havana/white", NULL); // for Dreamin

					static auto bluelaser1 = MatSystemOther->FindMaterial("sprites/bluelaser1", NULL); //for Mako, get pParent

					//for mako v5 [Hint: get parent and they aren't killed after usage also use LoopTroughEntities for it]

					if (pMaterial) ConMsg("%s\n", pMaterial->GetName());

					if (pMaterial != blueblacklargebeam && pMaterial != white)
					{
						EntitiesListener.erase(EntitiesListener.begin() + Iteration);

						continue;
					}*/

					auto EntityMaxs = Entity->GetMaxs(); auto EntityMins = Entity->GetMins();

					if (EntityMaxs.DistToThis(EntityMins, YAW) < 250.f && EntityMaxs.DistToThis(EntityMins, PITCH) < 250.f)
					{
						EntitiesListener.erase(EntitiesListener.begin() + Iteration);

						continue;
					}
				}
			}
			else
			{
				auto pParent = EntityList->GetClientEntityFromHandle(PVOID(Entity->GetNetworkMoveParent())); //Get Parent info instead

				auto EntityOrigin = (pParent ? pParent->GetAbsOrigin() : Entity->GetAbsOrigin());

				auto EntityVelocity = (pParent ? pParent->EstimateAbsVelocity() : Entity->EstimateAbsVelocity());

				if (auto Speed = EntityVelocity.Length2D(); Speed > 705.f && EntityVelocity.z <= 1.f
					/*&& (Globals->tickcount - Entity->GetCreationTick()) >= 15/*Anti-Cheat bypass ?*//*true*/) // Don't remove this check!
				{
					auto EntityMaxs = Entity->GetMaxs(); auto EntityMins = Entity->GetMins();

					if (EntityInfo.Origin.z + EntityMaxs.z < Last_LocalPlayer_Roll_Position
						|| EntityInfo.Origin.z + EntityMins.z > Last_LocalPlayer_Roll_Position + m_vView.z
						|| (EntityInfo.Origin.DistTo(EntityInfo.LocalPlayerOrigin) < EntityInfo.LocalPlayerOrigin.DistTo(EntityOrigin)
							|| (EntityInfo.ReleaseTick != 0 && cmd->tick_count > EntityInfo.ReleaseTick))) {

						EntitiesListener.erase(EntitiesListener.begin() + Iteration);

						continue;
					}

					auto SumedVelocity = EntityVelocity.y + EntityVelocity.x;

					bool JumpLaser = EntityOrigin.z + EntityMins.z <= Last_LocalPlayer_Roll_Position + m_vDuckView.z;

					FindInCurvedList((pParent ? pParent->GetModelIndex() : Entity->GetModelIndex()), EntityVelocity, EntityOrigin, LocalOrigin, EntityInfo, JumpLaser);

					// we don't want to take an action if the laser isn't gonna hit us with it's big model

					if (bool Yaw = MakeItPositive(EntityVelocity.y) > MakeItPositive(EntityVelocity.x); Yaw)
					{
						if (LocalOrigin.x + Mins.x > EntityOrigin.x + EntityMaxs.x) continue;

						if (LocalOrigin.x + Maxs.x < EntityOrigin.x + EntityMins.x) continue;
					}
					else
					{
						if ((LocalOrigin.y + Maxs.y) < (EntityOrigin.y + EntityMins.y)) continue;

						if ((LocalOrigin.y + Mins.y) > (EntityOrigin.y + EntityMaxs.y)) continue;
					}

					auto AdditionValue = EntityVelocity.InSameDirection(MyVelocity) ?  EntityMins : EntityMaxs;

					if (EntityInfo.Jumped || EntityInfo.Ducked)
					{
						bSendPacket = true;

						SendButton(IN_DUCK);

						Engine->GetViewAngles(cmd->viewangles);

						continue;
					}

					/*if (!EntityInfo.Action_Tick)*/ Set_Action_Or_Release_Tick(
						EntityVelocity,
						EntityInfo.Origin,
						LocalOrigin,
						EntityInfo,
						cmd->tick_count,
						JumpLaser,
						AdditionValue,
						Entity->GetCreationTick(),
						true);

					const auto Jump_Condations{
					JumpLaser
					&& !EntityInfo.Jumped
					&& (Menu::Get.General.Filter == NULL || Menu::Get.General.Filter == 2) };

					const auto Duck_Condations{
						!JumpLaser
						&& !EntityInfo.Ducked
						&& Menu::Get.General.Filter };

					if (cmd->tick_count >= EntityInfo.Action_Tick)
					{
						if (Jump_Condations)
						{
							EntityInfo.Jumped = !EntityInfo.Jumped;

							if (OnGround) SendButton(IN_JUMP | IN_DUCK);
						}
						else if (Duck_Condations)
						{
							SendButton(IN_DUCK);

							EntityInfo.Ducked = !EntityInfo.Ducked;
						}

						if (Jump_Condations || Duck_Condations)
						{
							Set_Action_Or_Release_Tick(EntityVelocity,
								EntityOrigin,
								LocalOrigin,
								EntityInfo,
								cmd->tick_count,
								JumpLaser,
								{ 0.f,0.f,0.f });
						}
					}
				}
				else
				{
					EntityInfo.IgnoreTicks++;
				}
			}
		}
	}
}
