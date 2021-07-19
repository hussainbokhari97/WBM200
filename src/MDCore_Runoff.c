/******************************************************************************

GHAAS Water Balance/Transport Model
Global Hydrological Archive and Analysis System
Copyright 1994-2021, UNH - ASRC/CUNY

MDCore_Runoff.c

bfekete@gc.cuny.edu

*******************************************************************************/

#include <MF.h>
#include <MD.h>

// Input
static int _MDInSurfCore_RunoffID = MFUnset;
static int _MDInBaseFlowID   = MFUnset;
static int _MDInRunoffCorrID = MFUnset;
// Output
static int _MDOutCore_RunoffID    = MFUnset;

static void _MDRunoff (int itemID) {
// Input
	float baseFlow;
	float surfaceRO;
	float runoffCorr;

	baseFlow  = MFVarGetFloat (_MDInBaseFlowID,   itemID, 0.0);
	surfaceRO = MFVarGetFloat (_MDInSurfCore_RunoffID, itemID, 0.0);
	runoffCorr = _MDInRunoffCorrID == MFUnset ? 1.0 : MFVarGetFloat (_MDInRunoffCorrID, itemID, 1.0);
	MFVarSetFloat (_MDOutCore_RunoffID, itemID, (baseFlow + surfaceRO) * runoffCorr);
}
 
enum { MDinput, MDcalculate, MDcorrected, MDhelp };

int MDCore_RunoffDef () {
	int  optID = MDinput;
	const char *optStr, *optName = MDVarCore_Runoff;
	const char *options [] = { MFinputStr, MFcalculateStr, "corrected", MFhelpStr, (char *) NULL };

	if (_MDOutCore_RunoffID != MFUnset) return (_MDOutCore_RunoffID);

	MFDefEntering ("Runoff");
	if ((optStr = MFOptionGet (optName)) != (char *) NULL) optID = CMoptLookup (options, optStr, true);
	switch (optID) {
		case MDhelp:  MFOptionMessage (optName, optStr, options);
		case MDinput: _MDOutCore_RunoffID = MFVarGetID (MDVarCore_Runoff, "mm", MFInput, MFFlux, MFBoundary); break;
		case MDcorrected:
			if ((_MDInRunoffCorrID  = MFVarGetID (MDVarDataAssim_RunoffCorretion, MFNoUnit, MFInput, MFState, MFBoundary)) == CMfailed)
				return (CMfailed);
			break;
		case MDcalculate:		
			if (((_MDInBaseFlowID        = MDCore_BaseFlowDef()) == CMfailed) ||
                ((_MDInSurfCore_RunoffID = MDCore_SurfRunoffDef()) == CMfailed) ||
                ((_MDOutCore_RunoffID    = MFVarGetID (MDVarCore_Runoff, "mm", MFOutput, MFFlux, MFBoundary)) == CMfailed) ||
                (MFModelAddFunction (_MDRunoff) == CMfailed)) return (CMfailed);
			break;
		default: MFOptionMessage (optName, optStr, options); return (CMfailed);
	}
	MFDefLeaving  ("Runoff");
	return (_MDOutCore_RunoffID);
}
