

class CViewSetup
{
public:
#include "Inconstructible.h"
	INCONSTRUCTIBLE(CViewSetup);

	uint32_t x; //0x0000 
	uint32_t m_nUnscaledX; //0x0004 
	uint32_t y; //0x0008 
	uint32_t m_nUnscaledY; //0x000C 
	uint32_t width; //0x0010 
	uint32_t m_nUnscaledWidth; //0x0014 
	uint32_t height; //0x0018 
	uint32_t m_eStereoEye; //0x001C 
	uint32_t m_nUnscaledHeight; //0x0020 
	bool m_bOrtho; //0x0024 
	float m_OrthoLeft; //0x0028 
	float m_OrthoTop; //0x002C 
	float m_OrthoRight; //0x0030 
	float m_OrthoBottom; //0x0034 
	float m_fov; //0x0038 
	float m_viewmodel_fov; //0x003C 
	Vector origin; //0x0040 
	Vector angles; //0x004C 
	float zNear; //0x0058 
	float zFar; //0x005C 
	float viewmodel_zNear; //0x0060 
	float viewmodel_zFar; //0x0064 
	bool m_bRenderToSubrectOfLargerScreen; //0x0068 
	float m_flAspectRatio; //0x006C 
	bool m_bOffCenter; //0x0070 
	float m_flOffCenterTop; //0x0074 
	float m_flOffCenterBottom; //0x0078 
	float m_flOffCenterLeft; //0x007C 
	float m_flOffCenterRight; //0x0080 
	bool m_bDoBloomAndToneMapping; //0x0084 
	bool m_bCacheFullSceneState; //0x0088 
	bool m_bViewToProjectionOverride; //0x008C 
	matrix3x4 m_ViewToProjection; //0x0090 
};