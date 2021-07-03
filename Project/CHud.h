

class CHudChat
{
public:
#include "Inconstructible.h"
	INCONSTRUCTIBLE(CHudChat);

	enum ChatFilters
	{
		CHAT_FILTER_NONE = 0,
		CHAT_FILTER_JOINLEAVE = 0x000001,
		CHAT_FILTER_NAMECHANGE = 0x000002,
		CHAT_FILTER_PUBLICCHAT = 0x000004,
		CHAT_FILTER_SERVERMSG = 0x000008,
		CHAT_FILTER_TEAMCHANGE = 0x000010,
		//=============================================================================
		// HPE_BEGIN:
		// [tj]Added a filter for achievement announce
		//=============================================================================

		CHAT_FILTER_ACHIEVEMENT = 0x000020,

		//=============================================================================
		// HPE_END
		//=============================================================================
	};

	void ChatPrintf(int iPlayerIndex, int iFilter, const char* fmt, ...)
	{
		va_list va_alist;
		char buf[256];
		va_start(va_alist, fmt);
		_vsnprintf(buf, sizeof(buf), fmt, va_alist);
		va_end(va_alist);

		Get_vFunction<void(__cdecl*)(void*, int, int, const char*)>(this, 18)(this, iPlayerIndex, iFilter, buf);
	}
};