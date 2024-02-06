#pragma once
// Metallicafan212:	This file defines bools to properly define behavior for specific windows versions
// 
// 
// 
// Metallicafan212:	Bool to mark the currently running application as a wine app
extern UBOOL						GWineAndDine;

// Metallicafan212:	Bool to say that the current OS is 8.1 or higher
//					Certain DPI options REQUIRE this
extern UBOOL						GWin81;

// Metallicafan212:	Same here, but for Windows 10
extern UBOOL						GWin10;

void SetupWindowsVersionCheck();