#include "definitions.h"
#include "Plugin.h"

//V241 for VS2008

//Called when the user hits the configure button in MPQDraft or FireGraft
BOOL WINAPI Plugin::Configure(HWND hParentWnd) {
  /*
   *  You can't really change the version from
   *  here with the current format.  However, you
   *  can check the version and probably set any
   *  globals.
   */

  MessageBox(
    hParentWnd,
    PLUGIN_NAME " (ID: " STR(PLUGIN_ID) ")"
    "\nCompiled by lucifirius"
    "\nBuilt on " __DATE__ " " __TIME__
    "\n"
	"\nFeatures:"
	"\n-stuff, ask me anything. mega thanks to pr0nogo and his discord for everything"
	"\n-thanks to kysxd for code"
	"\n-Open-source"
    ,
    PLUGIN_NAME,
    MB_TASKMODAL
  );

  return TRUE;
}

