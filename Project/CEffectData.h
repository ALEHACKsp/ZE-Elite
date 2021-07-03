#include "Vector.h"

class CTeslaInfo
{
public:
	Vector			m_vPos;
	QAngle			m_vAngles;
	int				m_nEntIndex;
	const char* m_pszSpriteName;
	float			m_flBeamWidth;
	int				m_nBeams;
	Vector			m_vColor;
	float			m_flTimeVisible;
	float			m_flRadius;
};

enum SimpleParticleFlag_t
{
	SIMPLE_PARTICLE_FLAG_WINDBLOWN = 0x1,
	SIMPLE_PARTICLE_FLAG_NO_VEL_DECAY = 0x2 // Used by the blood spray emitter. By default, it decays the
											// particle velocity.
};

class CParticleSubTexture;

struct Particle
{
	Particle* m_pPrev, * m_pNext;

	// Which sub texture this particle uses (so we can get at the tcoord mins and maxs).
	CParticleSubTexture* m_pSubTexture;

	// If m_Pos isn't used to store the world position, then implement IParticleEffect::GetParticlePosition()
	Vector m_Pos;           // Position of the particle in world space
};


class SimpleParticle : public Particle
{
public:
	SimpleParticle() : m_iFlags(0) {}

	// AddSimpleParticle automatically initializes these fields.
	Vector      m_vecVelocity;
	float       m_flRoll;
	float       m_flDieTime;    // How long it lives for.
	float       m_flLifetime;   // How long it has been alive for so far.
	unsigned char   m_uchColor[3];
	unsigned char   m_uchStartAlpha;
	unsigned char   m_uchEndAlpha;
	unsigned char   m_uchStartSize;
	unsigned char   m_uchEndSize;
	unsigned char   m_iFlags;   // See SimpleParticleFlag_t above
	float       m_flRollDelta;
};


enum ParticleAttachment_t
{
	PATTACH_ABSORIGIN = 0,			// Create at absorigin, but don't follow
	PATTACH_ABSORIGIN_FOLLOW,		// Create at absorigin, and update to follow the entity
	PATTACH_CUSTOMORIGIN,			// Create at a custom origin, but don't follow
	PATTACH_POINT,					// Create on attachment point, but don't follow
	PATTACH_POINT_FOLLOW,			// Create on attachment point, and update to follow the entity

	PATTACH_WORLDORIGIN,			// Used for control points that don't attach to an entity

	PATTACH_ROOTBONE_FOLLOW,		// Create at the root bone of the entity, and update to follow

	MAX_PATTACH_TYPES,
};

struct te_tf_particle_effects_colors_t
{
	Vector m_vecColor1;
	Vector m_vecColor2;
};

struct te_tf_particle_effects_control_point_t
{
	ParticleAttachment_t m_eParticleAttachment;
	Vector m_vecOffset;
};

class CEffectData
{
public:
	Vector m_vOrigin;
	Vector m_vStart;
	Vector m_vNormal;
	QAngle m_vAngles;
	int		m_fFlags;
	int		m_nEntIndex;
	float	m_flScale;
	float	m_flMagnitude;
	float	m_flRadius;
	int		m_nAttachmentIndex;
	short	m_nSurfaceProp;

	// Some TF2 specific things
	int		m_nMaterial;
	int		m_nDamageType;
	int		m_nHitBox;

	unsigned char	m_nColor;

	// Color customizability
	bool							m_bCustomColors;
	te_tf_particle_effects_colors_t	m_CustomColors;

	bool									m_bControlPoint1;
	te_tf_particle_effects_control_point_t	m_ControlPoint1;

	// Don't mess with stuff below here. DispatchEffect handles all of this.
public:
	CEffectData()
	{
		m_vOrigin.Init();
		m_vStart.Init();
		m_vNormal.Init();
		m_vAngles.Init();

		m_fFlags = 0;
		m_nEntIndex = 0;
		m_flScale = 1.f;
		m_nAttachmentIndex = 0;
		m_nSurfaceProp = 0;

		m_flMagnitude = 0.0f;
		m_flRadius = 0.0f;

		m_nMaterial = 0;
		m_nDamageType = 0;
		m_nHitBox = 0;

		m_nColor = 0;

		m_bCustomColors = false;
		m_CustomColors.m_vecColor1.Init(1.f, 1.f, 1.f);
		m_CustomColors.m_vecColor2.Init(1.f, 1.f, 1.f);

		m_bControlPoint1 = false;
		m_ControlPoint1.m_eParticleAttachment = PATTACH_ABSORIGIN;
		m_ControlPoint1.m_vecOffset.Init();
	}

	int GetEffectNameIndex() { return m_iEffectName; }

private:
	int m_iEffectName;	// Entry in the EffectDispatch network string table. The is automatically handled by DispatchEffect().
};


inline float anglemod(float a)
{
	a = (360.f / 65536) * ((int)(a * (65536.f / 360.0f)) & 65535);
	return a;
}

inline Vector RandomVector(float minVal, float maxVal)
{
	static Vector random;
	random.Random(minVal, maxVal);
	return random;
}