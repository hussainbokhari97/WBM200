/******************************************************************************
 GHAAS Water Balance/Transport Model
 Global Hydrological Archive and Analysis System
 Copyright 1994-2022, UNH - ASRC/CUNY

 MDCommon_HumidityRelative.c

 amiara@ccny.cuny.edu

 *******************************************************************************/

/********************************************************************************
 * Calculates Specific Humidity from pressure, air temperature and rh
 * ******************************************************************************/

#include <math.h>
#include <MF.h>
#include <MD.h>

// Input
static int _MDInCommon_HumiditySaturatedVaporPressID = MFUnset;
static int _MDInCommon_HumidityVaporPressureID       = MFUnset;
// Output
static int _MDOutCommon_HumidityRelativeID           = MFUnset;

static void _MDCommon_HumidityRelative (int itemID) {
    float saturatedVP;      // Saturated vapor pressure in Pa
    float vaporPress;       // Vapor pressure in Pa
    float relativehumidity; // Relative humidity in precent

    saturatedVP = MFVarGetFloat (_MDInCommon_HumiditySaturatedVaporPressID, itemID, 0.0);
    vaporPress  = MFVarGetFloat (_MDInCommon_HumidityVaporPressureID,       itemID, 0.0); //pressure (Pa)

    relativehumidity = vaporPress < saturatedVP ? 100.0 * vaporPress / saturatedVP : 100.0;
    MFVarSetFloat(_MDOutCommon_HumidityRelativeID, itemID, relativehumidity);
}

int MDCommon_HumidityRelativeDef () {
    int optID = MFinput;
    const char *optStr;
     
    if (_MDOutCommon_HumidityRelativeID != MFUnset) return (_MDOutCommon_HumidityRelativeID);    

    MFDefEntering ("RelativeHumidity");
    if ((optStr = MFOptionGet(MDOptWeather_RelativeHumidity)) != (char *) NULL) optID = CMoptLookup(MFsourceOptions, optStr, true);
    switch (optID) {
        default:
        case MFhelp: MFOptionMessage (MDOptWeather_RelativeHumidity, optStr, MFsourceOptions); return (CMfailed);
        case MFinput: _MDOutCommon_HumidityRelativeID = MFVarGetID (MDVarCommon_HumidityRelative, "%", MFInput, MFState, MFBoundary); break;
        case MFcalculate:
            if (((_MDInCommon_HumiditySaturatedVaporPressID = MDCommon_HumiditySaturatedVaporPressureDef ()) == CMfailed) ||
                ((_MDInCommon_HumidityVaporPressureID       = MDCommon_HumidityVaporPressureDef ())          == CMfailed) ||
                ((_MDOutCommon_HumidityRelativeID = MFVarGetID (MDVarCommon_HumidityRelative, "%", MFOutput, MFState, MFBoundary)) == CMfailed) ||
                ((MFModelAddFunction (_MDCommon_HumidityRelative) == CMfailed))) return (CMfailed);
            break;
    }
    MFDefLeaving ("RelativeHumidity");
    return (_MDOutCommon_HumidityRelativeID);
}
