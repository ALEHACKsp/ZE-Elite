#pragma once

void* Copy_Table(void* Table_Location)
{
	#ifndef Bits_32
		unsigned __int64 Functions_Amount = 0;
	#else
		unsigned __int32 Functions_Amount = 0;
	#endif
	
	Take_Function_Into_Account_Label:
	{
		#ifndef Bits_32
			if (*(void**)((unsigned __int64)Table_Location + Functions_Amount) != nullptr)
		#else
			if (*(void**)((unsigned __int32)Table_Location + Functions_Amount) != nullptr)
		#endif
			{
				Functions_Amount += sizeof(void*);

				goto Take_Function_Into_Account_Label;
			}
	}

	void* Copied_Table_Location = malloc(Functions_Amount);

	#ifndef Bits_32
		unsigned __int64 Function_Number = 0;
	#else
		unsigned __int32 Function_Number = 0;
	#endif

	Set_Function_Label:
	{
		#ifndef Bits_32
			*(void**)((unsigned __int64)Copied_Table_Location + Function_Number) = *(void**)((unsigned __int64)Table_Location + Function_Number);

			Function_Number += 8;
		#else
			*(void**)((unsigned __int32)Copied_Table_Location + Function_Number) = *(void**)((unsigned __int32)Table_Location + Function_Number);

			Function_Number += 4;
		#endif

		if (Functions_Amount != Function_Number)
		{
			goto Set_Function_Label;
		}
	}

	return Copied_Table_Location;
}