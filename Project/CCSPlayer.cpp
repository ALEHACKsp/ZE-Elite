#include "CCSPlayer.h"
#include "Assorted.h"


bool CBaseEntity::CanSeePlayer(CBaseEntity* player, const Vector& pos)
{
	CGameTrace tr;

	UTIL_TraceLine(GetEyePosition(), pos, CS_MASK_SHOOT | CONTENTS_HITBOX, this, 0, &tr);

	return tr.m_pEnt == player || tr.fraction > 0.97f;
}

const Vector CBaseEntity::GetHitboxPosition(int iHitbox)
{
	auto model{ GetModel() };
	if (!model)
		return Vector();

	studiohdr_t* hdr = ModelInfo->GetStudiomodel(model);
	if (!hdr)
		return Vector();

	matrix3x4 matrix[128];
	if (!SetupBones(matrix, 128, 0x100, Globals->curtime))
		return Vector();

	int HitboxSetIndex = *(int*)((DWORD)hdr + 0xB0);
	if (!HitboxSetIndex)
		return Vector();

	mstudiohitboxset_t* pSet = (mstudiohitboxset_t*)(((PBYTE)hdr) + HitboxSetIndex);

	mstudiobbox_t* box = pSet->GetHitbox(iHitbox);
	if (!box)
		return Vector();

	Vector vCenter = (box->bbmin + box->bbmax) * 0.5f;

	Vector vHitbox;

	Math::VectorTransform(vCenter, matrix[box->bone], vHitbox);

	return vHitbox;
}
