#include <fbsdk/fbsdk.h>

// NOTE: Do NOT specify 'quoted' strings for FBLiraryDeclare and FBLibraryRegister macros.

FBLibraryDeclare(OSCInputDeviceSample) // Register the plugin library
{
    FBLibraryRegister(OSCInputDeviceSampleDevice); // Register Device Class
    FBLibraryRegister(OSCInputDeviceSampleLayout); // Register Device Layout Class
}
FBLibraryDeclareEnd;

// NOTE: We do not need to custom these functions
//       unless you have to do some specific initialization or cleanup.
//
bool FBLibrary::LibInit() { return true; }    // Called "before" application starts
bool FBLibrary::LibOpen() { return true; }    // Called "before" application starts
bool FBLibrary::LibReady() { return true; }   // Called "before" application starts
bool FBLibrary::LibClose() { return true; }   // Called "after" application closes
bool FBLibrary::LibRelease() { return true; } // Called "after" application closes