#include "use_tech.h"
#include "SCBW/scbwdata.h"
#include "SCBW/api.h"
#include "hook_tools.h"


void __declspec(naked) UseTechWrapper()
{
	static u16 id;
	__asm
	{
		MOV id, CX
		PUSHAD
	}
	hooks::BTNSACT_UseTech(id);
	__asm
	{
		POPAD
		RETN
	}
}


namespace hooks
{
	void injectUseTechHooks()
	{
		jmpPatch(UseTechWrapper, 0x00423F70);
	}
}
