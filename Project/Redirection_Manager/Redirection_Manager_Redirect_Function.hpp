#pragma once

void Redirect_Function(__int8 Modify_Access_Rights, void* Original_Function_Location, void* Redirected_Function_Location)
{
	unsigned long __int32 Previous_Access_Rights;

	if (Modify_Access_Rights == 1)
	{
		#ifndef Bits_32
			VirtualProtect(Original_Function_Location, 16, PAGE_EXECUTE_READWRITE, &Previous_Access_Rights);
		#else
			VirtualProtect(Original_Function_Location, 6, PAGE_EXECUTE_READWRITE, &Previous_Access_Rights);
		#endif
	}

	#ifndef Bits_32
		*(__int8*)Original_Function_Location = 80;

		*(unsigned __int16*)((unsigned __int64)Original_Function_Location + 1) = 47176;

		*(void**)((unsigned __int64)Original_Function_Location + 3) = Redirected_Function_Location;

		*(__int32*)((unsigned __int64)Original_Function_Location + 11) = 604276552;
			
		*(unsigned __int8*)((unsigned __int64)Original_Function_Location + 15) = 195;
	#else
		*(__int8*)Original_Function_Location = 104;

		*(void**)((unsigned __int32)Original_Function_Location + 1) = Redirected_Function_Location;

		*(unsigned __int8*)((unsigned __int32)Original_Function_Location + 5) = 195;
	#endif

	if (Modify_Access_Rights == 1)
	{
		#ifndef Bits_32
			VirtualProtect(Original_Function_Location, 16, Previous_Access_Rights, &Previous_Access_Rights);
		#else
			VirtualProtect(Original_Function_Location, 6, Previous_Access_Rights, &Previous_Access_Rights);
		#endif
	}
}

#ifndef Bits_32
	void Redirect_Function(void*& Original_Function_Original_Location, unsigned __int64 Original_Function_Caller_Offset, void* Original_Function_Location, __int8 Modify_Access_Rights, void* Redirected_Function_Location)
#else
	void Redirect_Function(void*& Original_Function_Original_Location, unsigned __int32 Original_Function_Caller_Offset, void* Original_Function_Location, __int8 Modify_Access_Rights, void* Redirected_Function_Location)
#endif
	{
		unsigned long __int32 Previous_Access_Rights;

		#ifndef Bits_32
			Original_Function_Original_Location = malloc(33 + Original_Function_Caller_Offset);

			*(void**)Original_Function_Original_Location = *(void**)Original_Function_Location;

			*(void**)((unsigned __int64)Original_Function_Original_Location + 8) = *(void**)((unsigned __int64)Original_Function_Location + 8);

			*(unsigned __int8*)((unsigned __int64)Original_Function_Original_Location + 16) = *(unsigned __int8*)((unsigned __int64)Original_Function_Location + 16);

			unsigned __int64 Byte_Number = 0;

			Set_Byte_Label:
			{
				if (Original_Function_Caller_Offset != Original_Function_Caller_Offset)
				{
					*(unsigned __int8*)((unsigned __int64)Original_Function_Original_Location + 17 + Byte_Number) = *(unsigned __int32*)((unsigned __int64)Original_Function_Location + 17 + Byte_Number);

					Byte_Number += 1;

					goto Set_Byte_Label;
				}
			}

			*(__int8*)((unsigned __int64)Original_Function_Original_Location + 17 + Original_Function_Caller_Offset) = 80;

			*(unsigned __int16*)((unsigned __int64)Original_Function_Original_Location + 18 + Original_Function_Caller_Offset) = 47176;

			*(void**)((unsigned __int64)Original_Function_Original_Location + 20 + Original_Function_Caller_Offset) = (void*)((unsigned __int64)Original_Function_Location + 17 + Original_Function_Caller_Offset);

			*(__int32*)((unsigned __int64)Original_Function_Original_Location + 28 + Original_Function_Caller_Offset) = 604276552;

			*(unsigned __int8*)((unsigned __int64)Original_Function_Original_Location + 32 + Original_Function_Caller_Offset) = 195;

			VirtualProtect(Original_Function_Original_Location, 33 + Original_Function_Caller_Offset, PAGE_EXECUTE_READWRITE, &Previous_Access_Rights);
		#else
			Original_Function_Original_Location = malloc(12 + Original_Function_Caller_Offset);

			*(void**)Original_Function_Original_Location = *(void**)Original_Function_Location;

			*(unsigned __int16*)((unsigned __int32)Original_Function_Original_Location + 4) = *(unsigned __int16*)((unsigned __int32)Original_Function_Location + 4);

			unsigned __int32 Byte_Number = 0;

			Copy_Byte_Label:
			{
				if (Original_Function_Caller_Offset != Byte_Number)
				{
					*(unsigned __int8*)((unsigned __int32)Original_Function_Original_Location + 6 + Byte_Number) = *(unsigned __int32*)((unsigned __int64)Original_Function_Location + 6 + Byte_Number);

					Byte_Number += 1;

					goto Copy_Byte_Label;
				}
			}

			*(__int8*)((unsigned __int32)Original_Function_Original_Location + 6 + Original_Function_Caller_Offset) = 104;

			*(void**)((unsigned __int32)Original_Function_Original_Location + 7 + Original_Function_Caller_Offset) = (void*)((unsigned __int32)Original_Function_Location + 6 + Original_Function_Caller_Offset);

			*(unsigned __int8*)((unsigned __int32)Original_Function_Original_Location + 11 + Original_Function_Caller_Offset) = 195;

			VirtualProtect(Original_Function_Original_Location, 12 + Original_Function_Caller_Offset, PAGE_EXECUTE_READWRITE, &Previous_Access_Rights);
		#endif

		if (Modify_Access_Rights == 1)
		{
			#ifndef Bits_32
				VirtualProtect(Original_Function_Location, 16, PAGE_EXECUTE_READWRITE, &Previous_Access_Rights);
			#else
				VirtualProtect(Original_Function_Location, 6, PAGE_EXECUTE_READWRITE, &Previous_Access_Rights);
			#endif
		}

		#ifndef Bits_32
			*(__int8*)Original_Function_Location = 80;

			*(unsigned __int16*)((unsigned __int64)Original_Function_Location + 1) = 47176;

			*(void**)((unsigned __int64)Original_Function_Location + 3) = Redirected_Function_Location;

			*(__int32*)((unsigned __int64)Original_Function_Location + 11) = 604276552;
			
			*(unsigned __int8*)((unsigned __int64)Original_Function_Location + 15) = 195;
		#else
			*(__int8*)Original_Function_Location = 104;

			*(void**)((unsigned __int32)Original_Function_Location + 1) = Redirected_Function_Location;

			*(unsigned __int8*)((unsigned __int32)Original_Function_Location + 5) = 195;
		#endif

		if (Modify_Access_Rights == 1)
		{
			#ifndef Bits_32
				VirtualProtect(Original_Function_Location, 16, Previous_Access_Rights, &Previous_Access_Rights);
			#else
				VirtualProtect(Original_Function_Location, 6, Previous_Access_Rights, &Previous_Access_Rights);
			#endif
		}
	}