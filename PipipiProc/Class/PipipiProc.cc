// -*- C++ -*-
//
// Package:     PipipiProc
// Module:      PipipiProc
// 
// Description: <one line class summary>
//
// Implementation:
//     <Notes on implementation>
//
// Author:      pts/14
// Created:     Wed Apr  7 11:35:17 CDT 2010
// $Id$
//
// Revision history
//
// $Log$
//

#include "Experiment/Experiment.h"

// system include files

// user include files
#include "PipipiProc/PipipiProc.h"
#include "Experiment/report.h"
#include "Experiment/units.h"  // for converting to/from standard CLEO units
#include "HistogramInterface/HINtupleVarNames.h"
#include "HistogramInterface/HINtupleArray.h"
#include "HistogramInterface/HIHist1D.h"

#include "DataHandler/Record.h"
#include "DataHandler/Frame.h"
#include "FrameAccess/extract.h"
#include "FrameAccess/FAItem.h"
#include "FrameAccess/FATable.h"

#include "Navigation/NavShower.h"
#include "C3cc/CcShowerAttributes.h"
#include "Navigation/NavShowerServer.h"
#include "C3ccProd/CcFortranShowerCorrector.h"

//I added the following at the suggestion of the NavShower web page
#include "Navigation/NavConReg.h"
#include "KinematicTrajectory/KTKinematicData.h"
#include "Navigation/NavTkShMatch.h"
#include "C3cc/CcAssignedEnergyHit.h"

#include "Navigation/NavTrack.h"
#include "TrackRoot/TRHelixFit.h"
#include "TrackRoot/TRTrackFitQuality.h"
#include "TrackRoot/TRSeedTrackQuality.h"
#include "TrackDelivery/TDKinematicFit.h"
#include "Navigation/NavElecId.h"

#include "FitEvt/FitEvtSettings.h"
#include "FitEvt/FitEvt.h"
#include "BeamEnergy/BeamEnergy.h"
#include "MagField/MagneticField.h"
#include "MagField/MFFieldMap.h"
#include "BeamSpot/BeamSpot.h"

#include "Navigation/NavPi0ToGG.h"
#include "Navigation/NavEtaToGG.h"
#include "PhotonDecays/PhdPi0.h"
#include "PhotonDecays/PhdEtaToGG.h"
#include "Navigation/NavTkShMatch.h"
#include <PhotonDecays/PhdPi0.h>
#include <CleoDB/DBEventHeader.h>


#include "CleoDB/DBEventHeader.h"
#include "TriggerData/TriggerData.h"
#include "TriggerL1Data/TriggerL1Data.h"
#include "Level4Proc/Level4Decision.h"

//RICH example
#include "Navigation/NavRich.h"
//Dedx example
#include "DedxInfo/DedxInfo.h"


// STL classes
// You may have to uncomment some of these or other stl headers 
// depending on what other header files you include (e.g. FrameAccess etc.)!
//#include <string>
//#include <vector>
//#include <set>
//#include <map>
//#include <algorithm>
//#include <utility>

//
// constants, enums and typedefs
//
static const char* const kFacilityString = "Processor.PipipiProc" ;

// ---- cvs-based strings (Id and Tag with which file was checked out)
static const char* const kIdString  = "$Id: processor.cc,v 1.41 2006/02/08 19:38:02 wsun Exp $";
static const char* const kTagString = "$Name: v07_03_00 $";

//
// static data member definitions
//



//
// constructors and destructor
//
PipipiProc::PipipiProc( void )               // anal1
   : Processor( "PipipiProc" )
{
   report( DEBUG, kFacilityString ) << "here in ctor()" << endl;

   // ---- bind a method to a stream -----
   // These lines ARE VERY IMPORTANT! If you don't bind the 
   // code you've just written (the "action") to a stream, 
   // your code won't get executed!

   bind( &PipipiProc::event,    Stream::kEvent );
   bind( &PipipiProc::beginRun, Stream::kBeginRun );
   //bind( &PipipiProc::endRun,   Stream::kEndRun );

   // do anything here that needs to be done at creation time
   // (e.g. allocate resources etc.)


}

PipipiProc::~PipipiProc()                    // anal5
{
   report( DEBUG, kFacilityString ) << "here in dtor()" << endl;
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}

//
// member functions
//

// ------------ methods for beginning/end "Interactive" ------------
// --------------------------- init method -------------------------
void
PipipiProc::init( void )          // anal1 "Interactive"
{
   report( DEBUG, kFacilityString ) << "here in init()" << endl;

   // do any initialization here based on Parameter Input by User
   // (e.g. run expensive algorithms that are based on parameters
   //  specified by user at run-time)

}

// -------------------- terminate method ----------------------------
void
PipipiProc::terminate( void )     // anal5 "Interactive"
{
   report( DEBUG, kFacilityString ) << "here in terminate()" << endl;

   // do anything here BEFORE New Parameter Change
   // (e.g. write out result based on parameters from user-input)
 
}

enum {knshower, kntrack, knpi0, //number of showers, tracks and pi0s

chisqv1,chisqf1, //chisq of pi+ pi-pi0 gamma fit
chisqv2,chisqf2, //chisq of pi+ pi- pi0 pi0 gamma fit
chisqv0,chisqf0, // chisq of pi+pi- gamma fit

e1,th1,phi1,e9251,       // unfitted photon energy
e2,th2,phi2,e9252,
e3,th3,phi3,e9253,
e4,th4,phi4,e9254,
e5,th5,phi5,e9255,
e6,th6,phi6,e9256,
e7,th7,phi7,e9257,
e8,th8,phi8,e9258,
e9,th9,phi9,e9259,
e10,th10,phi10,e92510,
e11,th11,phi11,e92511,      
e12,th12,phi12,e92512,       
e13,th13,phi13,e92513,       
e14,th14,phi14,e92514,       
e15,th15,phi15,e92515,       
e16,th16,phi16,e92516,

pi0hie1,pi0loe1,      // photon (from pi0) energy
pi0hie2,pi0loe2,
pi0hie3,pi0loe3,
pi0hie4,pi0loe4,
pi0hie5,pi0loe5,
pi0hie6,pi0loe6,
pi0hie7,pi0loe7,
pi0hie8,pi0loe8,

me1,eop1,me2,eop2,  //matched energy and E/p

pi0te1,pi0px1,pi0py1,pi0pz1,      // unfitted pi0 energy and momenta
pi0te2,pi0px2,pi0py2,pi0pz2,
pi0te3,pi0px3,pi0py3,pi0pz3,
pi0te4,pi0px4,pi0py4,pi0pz4,
pi0te5,pi0px5,pi0py5,pi0pz5,
pi0te6,pi0px6,pi0py6,pi0pz6,
pi0te7,pi0px7,pi0py7,pi0pz7,
pi0te8,pi0px8,pi0py8,pi0pz8,

fe1,fth1,fphi1,  // fitted ISR photon energy fitted with 3 pions
fe2,fth2,fphi2,  // fitted ISR photon energy fitted with 4 pions
fe0,fth0,fphi0, // fitted ISR photon energy fitted with 2 pions


pite1,pipx1,pipy1,pipz1,pite2,pipx2,pipy2,pipz2,pisigma1,pill1,pisigma2,pill2,
mute1,mupx1,mupy1,mupz1,mute2,mupx2,mupy2,mupz2,musigma1,mull1,musigma2,mull2,
kte1,kpx1,kpy1,kpz1,kte2,kpx2,kpy2,kpz2,ksigma1,kll1,ksigma2,kll2,

f1pite1,f1pipx1,f1pipy1,f1pipz1,f1pite2,f1pipx2,f1pipy2,f1pipz2, // fitted pi+pi- energy and momenta fitted with 3 pions
f2pite1,f2pipx1,f2pipy1,f2pipz1,f2pite2,f2pipx2,f2pipy2,f2pipz2, // fitted pi+pi- energy and momenta fitted with 4 pions
f3pite1,f3pipx1,f3pipy1,f3pipz1,f3pite2,f3pipx2,f3pipy2,f3pipz2, // fitted pi+pi- energy and momenta fitted with 4 pions (no ISR)
f0pite1,f0pipx1,f0pipy1,f0pipz1,f0pite2,f0pipx2,f0pipy2,f0pipz2, // fitted pi+pi- energy and momenta fitted with 2 pions

f1pi0te,f1pi0px,f1pi0py,f1pi0pz,                   // fitted pi0 energy and momenta with 3 pions
f1pi0hie,f1pi0loe,                                 // and its photon energies

f2pi0te1,f2pi0px1,f2pi0py1,f2pi0pz1,               // fitted pi0 pi0 energies and momenta with 4 pions
f2pi0te2,f2pi0px2,f2pi0py2,f2pi0pz2,
f2pi0hie1,f2pi0loe1,f2pi0hie2,f2pi0loe2,           // and its photon energies

f3pi0te1,f3pi0px1,f3pi0py1,f3pi0pz1,               // fitted pi0 pi0 energies and momenta with 4 pions (no ISR)
f3pi0te2,f3pi0px2,f3pi0py2,f3pi0pz2,
f3pi0hie1,f3pi0loe1,f3pi0hie2,f3pi0loe2,           // and its photon energies

charge1, charge2, //charges of the 2 tracks

kVarNum};



// ---------------- standard place to book histograms ---------------
void
PipipiProc::hist_book( HIHistoManager& iHistoManager )
{
   report( DEBUG, kFacilityString ) << "here in hist_book()" << endl;

   // book your histograms here

   HINtupleVarNames ntupleNames(kVarNum);

   ntupleNames.addVar(knshower,       "knshower");
   ntupleNames.addVar(kntrack,       "kntrack");
   ntupleNames.addVar(knpi0,         "knpi0");

   ntupleNames.addVar(chisqv1,       "chisqv1");
   ntupleNames.addVar(chisqf1,       "chisqf1");
   ntupleNames.addVar(chisqv2,       "chisqv2");
   ntupleNames.addVar(chisqf2,       "chisqf2");
   ntupleNames.addVar(chisqv0,       "chisqv0");
   ntupleNames.addVar(chisqf0,       "chisqf0");

   ntupleNames.addVar(e1,          "e1");
   ntupleNames.addVar(th1,        "th1");
   ntupleNames.addVar(phi1,       "phi1");
   ntupleNames.addVar(e9251,    "e9251");

   ntupleNames.addVar(e2,          "e2");
   ntupleNames.addVar(th2,        "th2");
   ntupleNames.addVar(phi2,       "phi2");
   ntupleNames.addVar(e9252,    "e9252");

   ntupleNames.addVar(e3,          "e3");
   ntupleNames.addVar(th3,        "th3");
   ntupleNames.addVar(phi3,       "phi3");
   ntupleNames.addVar(e9253,    "e9253");

   ntupleNames.addVar(e4,          "e4");
   ntupleNames.addVar(th4,        "th4");
   ntupleNames.addVar(phi4,       "phi4");
   ntupleNames.addVar(e9254,    "e9254");

   ntupleNames.addVar(e5,          "e5");
   ntupleNames.addVar(th5,        "th5");
   ntupleNames.addVar(phi5,       "phi5");
   ntupleNames.addVar(e9255,    "e9255");

   ntupleNames.addVar(e6,          "e6");
   ntupleNames.addVar(th6,        "th6");
   ntupleNames.addVar(phi6,       "phi6");
   ntupleNames.addVar(e9256,    "e9256");

   ntupleNames.addVar(e7,          "e7");
   ntupleNames.addVar(th7,        "th7");
   ntupleNames.addVar(phi7,       "phi7");
   ntupleNames.addVar(e9257,    "e9257");

   ntupleNames.addVar(e8,          "e8");
   ntupleNames.addVar(th8,        "th8");
   ntupleNames.addVar(phi8,       "phi8");
   ntupleNames.addVar(e9258,    "e9258");

   ntupleNames.addVar(e9,          "e9");
   ntupleNames.addVar(th9,        "th9");
   ntupleNames.addVar(phi9,       "phi9");
   ntupleNames.addVar(e9259,    "e9259");

   ntupleNames.addVar(e10,          "e10");
   ntupleNames.addVar(th10,        "th10");
   ntupleNames.addVar(phi10,       "phi10");
   ntupleNames.addVar(e92510,    "e92510");

   ntupleNames.addVar(e11,          "e11");
   ntupleNames.addVar(th11,        "th11");
   ntupleNames.addVar(phi11,       "phi11");
   ntupleNames.addVar(e92511,    "e92511");

   ntupleNames.addVar(e12,          "e12");
   ntupleNames.addVar(th12,        "th12");
   ntupleNames.addVar(phi12,       "phi12");
   ntupleNames.addVar(e92512,    "e92512");

   ntupleNames.addVar(e13,          "e13");
   ntupleNames.addVar(th13,        "th13");
   ntupleNames.addVar(phi13,       "phi13");
   ntupleNames.addVar(e92513,    "e92513");

   ntupleNames.addVar(e14,          "e14");
   ntupleNames.addVar(th14,        "th14");
   ntupleNames.addVar(phi14,       "phi14");
   ntupleNames.addVar(e92514,    "e92514");

   ntupleNames.addVar(e15,          "e15");
   ntupleNames.addVar(th15,        "th15");
   ntupleNames.addVar(phi15,       "phi15");
   ntupleNames.addVar(e92515,    "e92515");

   ntupleNames.addVar(e16,          "e16");
   ntupleNames.addVar(th16,        "th16");
   ntupleNames.addVar(phi16,       "phi16");
   ntupleNames.addVar(e92516,    "e92516");

   ntupleNames.addVar(pi0hie1,          "pi0hie1");
   ntupleNames.addVar(pi0loe1,          "pi0loe1");

   ntupleNames.addVar(pi0hie2,          "pi0hie2");
   ntupleNames.addVar(pi0loe2,          "pi0loe2");

   ntupleNames.addVar(pi0hie3,          "pi0hie3");
   ntupleNames.addVar(pi0loe3,          "pi0loe3");

   ntupleNames.addVar(pi0hie4,          "pi0hie4");
   ntupleNames.addVar(pi0loe4,          "pi0loe4");

   ntupleNames.addVar(pi0hie5,          "pi0hie5");
   ntupleNames.addVar(pi0loe5,          "pi0loe5");

   ntupleNames.addVar(pi0hie6,          "pi0hie6");
   ntupleNames.addVar(pi0loe6,          "pi0loe6");

   ntupleNames.addVar(pi0hie7,          "pi0hie7");
   ntupleNames.addVar(pi0loe7,          "pi0loe7");

   ntupleNames.addVar(pi0hie8,          "pi0hie8");
   ntupleNames.addVar(pi0loe8,          "pi0loe8");

   ntupleNames.addVar(me1,          "me1");
   ntupleNames.addVar(eop1,          "eop1");
   ntupleNames.addVar(me2,          "me2");
   ntupleNames.addVar(eop2,          "eop2");

   ntupleNames.addVar(pi0te1,    "pi0te1");
   ntupleNames.addVar(pi0px1,    "pi0px1");
   ntupleNames.addVar(pi0py1,    "pi0py1");
   ntupleNames.addVar(pi0pz1,    "pi0pz1");

   ntupleNames.addVar(pi0te2,    "pi0te2");
   ntupleNames.addVar(pi0px2,    "pi0px2");
   ntupleNames.addVar(pi0py2,    "pi0py2");
   ntupleNames.addVar(pi0pz2,    "pi0pz2");

   ntupleNames.addVar(pi0te3,    "pi0te3");
   ntupleNames.addVar(pi0px3,    "pi0px3");
   ntupleNames.addVar(pi0py3,    "pi0py3");
   ntupleNames.addVar(pi0pz3,    "pi0pz3");

   ntupleNames.addVar(pi0te4,    "pi0te4");
   ntupleNames.addVar(pi0px4,    "pi0px4");
   ntupleNames.addVar(pi0py4,    "pi0py4");
   ntupleNames.addVar(pi0pz4,    "pi0pz4");

   ntupleNames.addVar(pi0te5,    "pi0te5");
   ntupleNames.addVar(pi0px5,    "pi0px5");
   ntupleNames.addVar(pi0py5,    "pi0py5");
   ntupleNames.addVar(pi0pz5,    "pi0pz5");

   ntupleNames.addVar(pi0te6,    "pi0te6");
   ntupleNames.addVar(pi0px6,    "pi0px6");
   ntupleNames.addVar(pi0py6,    "pi0py6");
   ntupleNames.addVar(pi0pz6,    "pi0pz6");

   ntupleNames.addVar(pi0te7,    "pi0te7");
   ntupleNames.addVar(pi0px7,    "pi0px7");
   ntupleNames.addVar(pi0py7,    "pi0py7");
   ntupleNames.addVar(pi0pz7,    "pi0pz7");

   ntupleNames.addVar(pi0te8,    "pi0te8");
   ntupleNames.addVar(pi0px8,    "pi0px8");
   ntupleNames.addVar(pi0py8,    "pi0py8");
   ntupleNames.addVar(pi0pz8,    "pi0pz8");

   ntupleNames.addVar(fe1,          "fe1");
   ntupleNames.addVar(fth1,        "fth1");
   ntupleNames.addVar(fphi1,       "fphi1");

   ntupleNames.addVar(fe2,          "fe2");
   ntupleNames.addVar(fth2,         "fth2");
   ntupleNames.addVar(fphi2,        "fphi2");

   ntupleNames.addVar(fe0,          "fe0");
   ntupleNames.addVar(fth0,	    "fth0");
   ntupleNames.addVar(fphi0,        "fphi0");

   ntupleNames.addVar(pite1,    "pite1");
   ntupleNames.addVar(pipx1,    "pipx1");
   ntupleNames.addVar(pipy1,    "pipy1");
   ntupleNames.addVar(pipz1,    "pipz1");
   ntupleNames.addVar(pite2,    "pite2");
   ntupleNames.addVar(pipx2,    "pipx2");
   ntupleNames.addVar(pipy2,    "pipy2");
   ntupleNames.addVar(pipz2,    "pipz2");

   ntupleNames.addVar(mute1,    "mute1");
   ntupleNames.addVar(mupx1,    "mupx1");
   ntupleNames.addVar(mupy1,    "mupy1");
   ntupleNames.addVar(mupz1,    "mupz1");
   ntupleNames.addVar(mute2,    "mute2");
   ntupleNames.addVar(mupx2,    "mupx2");
   ntupleNames.addVar(mupy2,    "mupy2");
   ntupleNames.addVar(mupz2,    "mupz2");

   ntupleNames.addVar(kte1,    "kte1");
   ntupleNames.addVar(kpx1,    "kpx1");
   ntupleNames.addVar(kpy1,    "kpy1");
   ntupleNames.addVar(kpz1,    "kpz1");
   ntupleNames.addVar(kte2,    "kte2");
   ntupleNames.addVar(kpx2,    "kpx2");
   ntupleNames.addVar(kpy2,    "kpy2");
   ntupleNames.addVar(kpz2,    "kpz2");

   ntupleNames.addVar(pisigma1, "pisigma1");
   ntupleNames.addVar(pill1,    "pill1");
   ntupleNames.addVar(pisigma2, "pisigma2");
   ntupleNames.addVar(pill2,    "pill2");

   ntupleNames.addVar(musigma1, "musigma1");
   ntupleNames.addVar(mull1,    "mull1");
   ntupleNames.addVar(musigma2, "musigma2");
   ntupleNames.addVar(mull2,    "mull2");

   ntupleNames.addVar(ksigma1, "ksigma1");
   ntupleNames.addVar(kll1,    "kll1");
   ntupleNames.addVar(ksigma2, "ksigma2");
   ntupleNames.addVar(kll2,    "kll2");

   ntupleNames.addVar(f1pite1,    "f1pite1");
   ntupleNames.addVar(f1pipx1,    "f1pipx1");
   ntupleNames.addVar(f1pipy1,    "f1pipy1");
   ntupleNames.addVar(f1pipz1,    "f1pipz1");
   ntupleNames.addVar(f1pite2,    "f1pite2");
   ntupleNames.addVar(f1pipx2,    "f1pipx2");
   ntupleNames.addVar(f1pipy2,    "f1pipy2");
   ntupleNames.addVar(f1pipz2,    "f1pipz2");

   ntupleNames.addVar(f2pite1,    "f2pite1");
   ntupleNames.addVar(f2pipx1,    "f2pipx1");
   ntupleNames.addVar(f2pipy1,    "f2pipy1");
   ntupleNames.addVar(f2pipz1,    "f2pipz1");
   ntupleNames.addVar(f2pite2,    "f2pite2");
   ntupleNames.addVar(f2pipx2,    "f2pipx2");
   ntupleNames.addVar(f2pipy2,    "f2pipy2");
   ntupleNames.addVar(f2pipz2,    "f2pipz2");

   ntupleNames.addVar(f3pite1,    "f3pite1");
   ntupleNames.addVar(f3pipx1,    "f3pipx1");
   ntupleNames.addVar(f3pipy1,    "f3pipy1");
   ntupleNames.addVar(f3pipz1,    "f3pipz1");
   ntupleNames.addVar(f3pite2,    "f3pite2");
   ntupleNames.addVar(f3pipx2,    "f3pipx2");
   ntupleNames.addVar(f3pipy2,    "f3pipy2");
   ntupleNames.addVar(f3pipz2,    "f3pipz2");

   ntupleNames.addVar(f0pite1,    "f0pite1");
   ntupleNames.addVar(f0pipx1,    "f0pipx1");
   ntupleNames.addVar(f0pipy1,    "f0pipy1");
   ntupleNames.addVar(f0pipz1,    "f0pipz1");
   ntupleNames.addVar(f0pite2,    "f0pite2");
   ntupleNames.addVar(f0pipx2,    "f0pipx2");
   ntupleNames.addVar(f0pipy2,    "f0pipy2");
   ntupleNames.addVar(f0pipz2,    "f0pipz2");

   ntupleNames.addVar(f1pi0te,    "f1pi0te");
   ntupleNames.addVar(f1pi0px,    "f1pi0px");
   ntupleNames.addVar(f1pi0py,    "f1pi0py");
   ntupleNames.addVar(f1pi0pz,    "f1pi0pz");

   ntupleNames.addVar(f1pi0hie,    "f1pi0hie");
   ntupleNames.addVar(f1pi0loe,    "f1pi0loe");

   ntupleNames.addVar(f2pi0te1,    "f2pi0te1");
   ntupleNames.addVar(f2pi0px1,    "f2pi0px1");
   ntupleNames.addVar(f2pi0py1,    "f2pi0py1");
   ntupleNames.addVar(f2pi0pz1,    "f2pi0pz1");

   ntupleNames.addVar(f2pi0te2,    "f2pi0te2");
   ntupleNames.addVar(f2pi0px2,    "f2pi0px2");
   ntupleNames.addVar(f2pi0py2,    "f2pi0py2");
   ntupleNames.addVar(f2pi0pz2,    "f2pi0pz2");

   ntupleNames.addVar(f2pi0hie1,    "f2pi0hie1");
   ntupleNames.addVar(f2pi0loe1,    "f2pi0loe1");

   ntupleNames.addVar(f2pi0hie2,    "f2pi0hie2");
   ntupleNames.addVar(f2pi0loe2,    "f2pi0loe2");

   ntupleNames.addVar(f3pi0te1,    "f3pi0te1");
   ntupleNames.addVar(f3pi0px1,    "f3pi0px1");
   ntupleNames.addVar(f3pi0py1,    "f3pi0py1");
   ntupleNames.addVar(f3pi0pz1,    "f3pi0pz1");

   ntupleNames.addVar(f3pi0te2,    "f3pi0te2");
   ntupleNames.addVar(f3pi0px2,    "f3pi0px2");
   ntupleNames.addVar(f3pi0py2,    "f3pi0py2");
   ntupleNames.addVar(f3pi0pz2,    "f3pi0pz2");

   ntupleNames.addVar(f3pi0hie1,    "f3pi0hie1");
   ntupleNames.addVar(f3pi0loe1,    "f3pi0loe1");

   ntupleNames.addVar(f3pi0hie2,    "f3pi0hie2");
   ntupleNames.addVar(f3pi0loe2,    "f3pi0loe2");

   ntupleNames.addVar(charge1,    "charge1");
   ntupleNames.addVar(charge2,    "charge2");

   m_showerTuple = iHistoManager.ntuple(10,"tuple", kVarNum, 10000, ntupleNames.names());



}

// --------------------- methods bound to streams -------------------
ActionBase::ActionResult
PipipiProc::event( Frame& iFrame )          // anal3 equiv.
{
   report( DEBUG, kFacilityString ) << "here in event()" << endl;

   const int kMaxTrack = 2;
   const int kMaxShower = 16;
   const int kMaxPi0 = 8;

   double ChisqV1 = 1000000.;
   double ChisqF1 = 1000000.;
   double ChisqV2 = 1000000.;
   double ChisqF2 = 1000000.;
   double ChisqV3 = 1000000.;
   double ChisqF3 = 1000000.;
   double ChisqV0 = 1000000.;
   double ChisqF0 = 1000000.;

   double TempChisqF1 = 1000000.;
   double TempChisqF2 = 1000000.;
   double TempChisqF3 = 1000000.;


   double E[kMaxShower] = {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
   double Theta[kMaxShower] = {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
   double Phi[kMaxShower] = {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
   double E925[kMaxShower] = {0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};

   double ME[kMaxTrack] = {0.,0.};
   double EOP[kMaxTrack] = {0.,0.};

   double F1E = 0.;
   double F1Theta = 0.;
   double F1Phi = 0.;

   double F2E = 0.;
   double F2Theta = 0.;
   double F2Phi = 0.;

   double F0E = 0.;
   double F0Theta = 0.;
   double F0Phi = 0.;

   double PIPE[kMaxTrack] = {0.,0.};
   double PIPX[kMaxTrack] = {0.,0.};
   double PIPY[kMaxTrack] = {0.,0.};
   double PIPZ[kMaxTrack] = {0.,0.};
   double MUPE[kMaxTrack] = {0.,0.};
   double MUPX[kMaxTrack] = {0.,0.};
   double MUPY[kMaxTrack] = {0.,0.};
   double MUPZ[kMaxTrack] = {0.,0.};
   double KPE[kMaxTrack] = {0.,0.};
   double KPX[kMaxTrack] = {0.,0.};
   double KPY[kMaxTrack] = {0.,0.};
   double KPZ[kMaxTrack] = {0.,0.};

   double PI0PE[kMaxPi0] = {0.,0.,0.,0.,0.,0.,0.,0.};
   double PI0PX[kMaxPi0] = {0.,0.,0.,0.,0.,0.,0.,0.};
   double PI0PY[kMaxPi0] = {0.,0.,0.,0.,0.,0.,0.,0.};
   double PI0PZ[kMaxPi0] = {0.,0.,0.,0.,0.,0.,0.,0.};

   double PI0HIE[kMaxPi0] = {0.,0.,0.,0.,0.,0.,0.,0.};
   double PI0LOE[kMaxPi0] = {0.,0.,0.,0.,0.,0.,0.,0.};

   double PPISIGMA[kMaxTrack] = {-1000.,-1000.};
   double PPILL[kMaxTrack] = {-1000.,-1000.};
   double PMUSIGMA[kMaxTrack] = {-1000.,-1000.};
   double PMULL[kMaxTrack] = {-1000.,-1000.};
   double PKSIGMA[kMaxTrack] = {-1000.,-1000.};
   double PKLL[kMaxTrack] = {-1000.,-1000.};

   double PIF1PE[kMaxTrack] = {0.,0.};
   double PIF1PX[kMaxTrack] = {0.,0.};
   double PIF1PY[kMaxTrack] = {0.,0.};
   double PIF1PZ[kMaxTrack] = {0.,0.};
   double PIF2PE[kMaxTrack] = {0.,0.};
   double PIF2PX[kMaxTrack] = {0.,0.};
   double PIF2PY[kMaxTrack] = {0.,0.};
   double PIF2PZ[kMaxTrack] = {0.,0.};
   double PIF3PE[kMaxTrack] = {0.,0.};
   double PIF3PX[kMaxTrack] = {0.,0.};
   double PIF3PY[kMaxTrack] = {0.,0.};
   double PIF3PZ[kMaxTrack] = {0.,0.};
   double PIF0PE[kMaxTrack] = {0.,0.};
   double PIF0PX[kMaxTrack] = {0.,0.};
   double PIF0PY[kMaxTrack] = {0.,0.};
   double PIF0PZ[kMaxTrack] = {0.,0.};

   double PI0F1PE = 0.;
   double PI0F1PX = 0.;
   double PI0F1PY = 0.;
   double PI0F1PZ = 0.;
   double PI0F1HIE = 0.;
   double PI0F1LOE = 0.;

   double PI0F2PE[2] = {0.,0.};
   double PI0F2PX[2] = {0.,0.};
   double PI0F2PY[2] = {0.,0.};
   double PI0F2PZ[2] = {0.,0.};
   double PI0F2HIE[2] = {0.,0.};
   double PI0F2LOE[2] = {0.,0.};

   double PI0F3PE[2] = {0.,0.};
   double PI0F3PX[2] = {0.,0.};
   double PI0F3PY[2] = {0.,0.};
   double PI0F3PZ[2] = {0.,0.};
   double PI0F3HIE[2] = {0.,0.};
   double PI0F3LOE[2] = {0.,0.};

   double CHARGE[kMaxTrack] = {0.,0.};

   float tuple[kVarNum];



// if we're in MC, check to see if the trigger fired
   FAItem< DBEventHeader > eventHeader ;
   extract( iFrame.record( Stream::kEvent ), eventHeader ) ;

//   double the_run = eventHeader->run();
//   double the_evt = eventHeader->number();

   if( eventHeader->monteCarlo() ) {
      double L1Trigger = 0., L4Decision = 0.;

////////////////////////////////////////////////////////////////////////////
// Trigger L1
////////////////////////////////////////////////////////////////////////////
//   int l1Trigger = 0, l1Trigger_Sel = 0, l1Trigger_Hadron = 0;
//   int l1Trigger_MuPair = 0, l1Trigger_ElTrack = 0, l1Trigger_RadTau = 0;
//   int l1Trigger_TwoTrack = 0;

      FAItem< TriggerL1Data > trigDat;
      extract (iFrame.record(Stream::kEvent), trigDat);

// All possible trigger lines
// --------------------------
// DABoolean isHadron() const;  //Ax track > 2 & CB lo > 0
// DABoolean isMuPair() const;  //Hi track > 1 & Back-to-back, 1 vs. 3/24
// DABoolean isElTrack() const;  //Ax track > 0 & CB med > 0
// DABoolean isRadTau() const;  //Stereo (hi + low) track > 1 & CB lo > 0
// DABoolean isBarrelBhabha() const; //Back-to-back Hi CB clusters

// DABoolean isCBSingleHi() const;  //CB hi > 0
// DABoolean isCESingleHi() const;  //CE hi > 0
// DABoolean isEndcapBhabha() const; //CE hi clusters in opposite ends
// DABoolean isTwoTrack() const; //Ax track > 1
// DABoolean isPulser() const;  //Pulser trigger
// DABoolean isRandom() const;  //Random trigger
// DABoolean isMinBias() const; //Tracking or CB Timing bit (Ax>0 || CB lo>1)
      // Decode trigger line information //

      // L1 Accept if any trigger line fires

      if(trigDat->isHadron()) L1Trigger = 1.;
      if(trigDat->isRadTau()) L1Trigger = 2.;
      if(trigDat->isElTrack()) L1Trigger = 3.;
      if(trigDat->isBarrelBhabha()) L1Trigger = 4.;
      if(trigDat->isEndcapBhabha()) L1Trigger = 5.;
      if(trigDat->isMuPair()) L1Trigger = 6.;
      if(trigDat->isCBSingleHi()) L1Trigger = 7.;
      if(trigDat->isCESingleHi()) L1Trigger = 8.;
      if(trigDat->isPulser()) L1Trigger = 9.;
      if(trigDat->isRandom()) L1Trigger = 10.;
      if(trigDat->isTwoTrack()) L1Trigger = 11.;
      if(trigDat->isMinBias()) L1Trigger = 12.;
      if(trigDat->isPhotonA()) L1Trigger = 13.;
      if(trigDat->isPhotonB()) L1Trigger = 14.;
      if(trigDat->isPhotonC()) L1Trigger = 15.;
      if(trigDat->isPhotonD()) L1Trigger = 16.;

      // L1 Accept if any non-prescaled trigger line fires
//   if ( (trigDat->isHadron())       ||
//      (trigDat->isMuPair())       ||
//      (trigDat->isElTrack())      ||
//      (trigDat->isRadTau())       ||
//      (trigDat->isBarrelBhabha())  )
//   { l1Trigger_Sel = 1; }

////////////////////////////////////////////////////////////////////////////
// Software Trigger (L3 & L4)
////////////////////////////////////////////////////////////////////////////
      FAItem<Level4Decision > decision;
      extract(iFrame.record(Stream::kEvent), decision );

      switch( decision->decision() )
      {
         case Level4Decision::kKeepLevel3Garbage :
         {
            L4Decision = 1.;
            break;
         }
         case Level4Decision::kLevel3AutoAccept :
         {
            L4Decision = 2.;
            break;
         }
         case Level4Decision::kLevel4Keep :
         {
            L4Decision = 3.;
            break;
         }
      }

      if( (!L1Trigger) || (!L4Decision) )
         return ActionBase::kFailed;
   }



////////////////////////////////////////////////////// tracks //////////////////////////////////////////////////

   FATable< NavTrack > trackTable;
   extract( iFrame.record( Stream::kEvent ) , trackTable, "GoodThings"  );

   FATable< NavTrack >::const_iterator trackBegin = trackTable.begin();
   FATable< NavTrack >::const_iterator trackEnd = trackTable.end();

   if(trackTable.size() != 2 )
      return ActionBase::kFailed;

   FAItem<TDKinematicFit> pi1 = (*trackBegin).pionFit();
   FAItem<TDKinematicFit> pi2 = (*(trackBegin+1)).pionFit();

   CHARGE[0] = (*pi1).charge();
   CHARGE[1] = (*pi2).charge();

   bool piok = false;

   if(pi1.valid() && pi2.valid() && ((*pi1).charge() == -(*pi2).charge()))
     piok = true;

   if(!piok)
     return ActionBase::kFailed;

   int unmatchedTracks = 0;

   for ( FATable< NavTrack >::const_iterator trackItr = trackBegin;
         trackItr != trackEnd ;
         ++trackItr )
   {
      FAItem<DedxInfo> the_dedx = (*trackItr).dedxInfo();
      FAItem<NavRich> the_rich = (*trackItr).richInfo();
      FAItem<TDKinematicFit> pionFit = (*trackItr).pionFit();
      FAItem<TDKinematicFit> muonFit = (*trackItr).muonFit();
      FAItem<TDKinematicFit> kaonFit = (*trackItr).kaonFit();
      double cos_theta1 = pionFit->pz() / pionFit->pmag();
      double cos_theta2 = muonFit->pz() / muonFit->pmag();
      double cos_theta3 = kaonFit->pz() / kaonFit->pmag();

      int j = unmatchedTracks++;

      if(j < kMaxTrack){

      if(trackItr->trackShowerMatch().valid())
         ME[j] = trackItr->trackShowerMatch()->matchedEnergy();

      EOP[j] = trackItr->elecId().eOverP();

      if( (pionFit.valid()) && (the_dedx.valid()) )  {

        PIPE[j]=pionFit->lorentzMomentum().e();
        PIPX[j]=pionFit->px();
        PIPY[j]=pionFit->py();
        PIPZ[j]=pionFit->pz();
        PPISIGMA[j]=the_dedx->piSigma();

        if ((the_rich.valid()) && (fabs(cos_theta1) < 0.8) && (the_rich->pionHypWasAnalyzed()))
          PPILL[j]=the_rich->pionLogLikelihood();
      }

      if( (muonFit.valid()) && (the_dedx.valid()) )  {

        MUPE[j]=muonFit->lorentzMomentum().e();
        MUPX[j]=muonFit->px();
        MUPY[j]=muonFit->py();
        MUPZ[j]=muonFit->pz();
        PMUSIGMA[j]=the_dedx->muSigma();

        if ((the_rich.valid()) && (fabs(cos_theta2) < 0.8) && (the_rich->muonHypWasAnalyzed()))
          PMULL[j]=the_rich->muonLogLikelihood();
      }

      if( (kaonFit.valid()) && (the_dedx.valid()) )  {

        KPE[j]=kaonFit->lorentzMomentum().e();
        KPX[j]=kaonFit->px();
        KPY[j]=kaonFit->py();
        KPZ[j]=kaonFit->pz();
        PKSIGMA[j]=the_dedx->kSigma();

        if ((the_rich.valid()) && (fabs(cos_theta3) < 0.8) && (the_rich->kaonHypWasAnalyzed()))
          PKLL[j]=the_rich->kaonLogLikelihood();
      }

   }
   }

/////////////////////////////////////////// showers //////////////////////////////////////////////////////

   FATable< NavShower > showerTable;
   extract( iFrame.record( Stream::kEvent ) , showerTable, "GoodThings" );

   if(showerTable.size() < 3)
      return ActionBase::kFailed;

   FATable< NavShower >::const_iterator showerBegin = showerTable.begin();
   FATable< NavShower >::const_iterator showerEnd = showerTable.end();

   int unmatchedShowers = 0;

   for ( FATable< NavShower >::const_iterator showerItr = showerBegin;
         showerItr != showerEnd ;
         ++showerItr )
   {

      int j = unmatchedShowers++;

      if(j<kMaxShower){

        E[j] = showerItr->attributes().energy();
        Theta[j] = showerItr->attributes().theta();
        Phi[j] = showerItr->attributes().phi();
        E925[j] = showerItr->attributes().x925();
      }
   }

////////////////////////////////////// pi0 /////////////////////////////////////////////////////////////////

  FATable<NavPi0ToGG> pi0Table;
  extract( iFrame.record( Stream::kEvent ) , pi0Table );
 
  FATable< NavPi0ToGG >::const_iterator pi0Begin = pi0Table.begin();
  FATable< NavPi0ToGG >::const_iterator pi0End = pi0Table.end();

  bool pi0ok = false;
  int unmatchedPi0s = 0;
  
  for (FATable< NavPi0ToGG >::const_iterator pi0Itr = pi0Begin;
       pi0Itr != pi0End ; ++pi0Itr) {
   
    if(pi0Itr.valid()
	   && pi0Itr->pi0().fit()
	   && pi0Itr->pi0().converged()
	   && (pi0Itr->pi0().chisq() < 500)
	   && (fabs(pi0Itr->pi0().pullMass()) < 3) ) {
      
      pi0ok = true;
      int j = unmatchedPi0s++;
  
      if(j<kMaxPi0){
        PI0PE[j] = pi0Itr->pi0().energy();
        PI0PX[j] = pi0Itr->pi0().px();
        PI0PY[j] = pi0Itr->pi0().py();
        PI0PZ[j] = pi0Itr->pi0().pz();
        PI0HIE[j] = pi0Itr->hiEnShower().attributes().energy();
        PI0LOE[j] = pi0Itr->loEnShower().attributes().energy();
      }
    }
  }

  if(!pi0ok)
    return ActionBase::kFailed;

  
/////////////////////////////////////////////// fit /////////////////////////////////////////////////////////

NavShowerServer *navsh_server = new NavShowerServer( iFrame.record(Stream::kEvent ) ) ;

const CcShowerAttributes &atts = showerBegin->attributes();

CcShowerAttributes new_atts ( atts.basicShowerAtts(),
                                       *sh_corr,
                                       atts.energy(),
                                       10,
                                       10,
                                       atts.theta(),
                                       atts.errTheta(),
                                       atts.uncErrTheta(),
                                       atts.phi(),
                                       atts.errPhi(),
                                       atts.uncErrPhi(),
                                       atts.distance(),
                                       atts.momentum(),
                                       atts.position(),
                                       atts.origin(),
                                       atts.errMatMom(),
                                       atts.errMatPos(),
                                       atts.errMatOri(),
                                       atts.status() );

NavShower *tmp_sh =  new NavShower( new_atts, *navsh_server )  ;

////////////////////////////// pi+ pi- gamma fit ////////////////////////////////////////////////////////////

   for ( FATable< NavShower >::const_iterator fastest_shower = showerBegin;
         fastest_shower != showerEnd ;
         ++fastest_shower )
   {
      if(fastest_shower->attributes().energy() > 0.5) {

         FitEvt pipi0( "Pipi0", FitEvt::k_P4VecCM );
         pipi0.newPhoton(*tmp_sh);
         pipi0.newTrack(*pi1);
         pipi0.newTrack(*pi2);
         pipi0.doTheFit();

         if( (pipi0.chisqVtx()>0) && (pipi0.chisqFit()>0) ) {

           const HepLorentzVector vgpi0 (pipi0.kdFitVec()[0]->lorentzMomentum());
           const HepLorentzVector v1pi0 (pipi0.kdFitVec()[1]->lorentzMomentum());
           const HepLorentzVector v2pi0 (pipi0.kdFitVec()[2]->lorentzMomentum());

           ChisqV0 = pipi0.chisqVtx();
           ChisqF0 = pipi0.chisqFit();

           F0E = vgpi0.e();
           F0Theta = vgpi0.theta();
           F0Phi = vgpi0.phi();

           PIF0PE[0]=v1pi0.e();
           PIF0PX[0]=v1pi0.px();
           PIF0PY[0]=v1pi0.py();
           PIF0PZ[0]=v1pi0.pz();

           PIF0PE[1]=v2pi0.e();
           PIF0PX[1]=v2pi0.px();
           PIF0PY[1]=v2pi0.py();
           PIF0PZ[1]=v2pi0.pz();

         }
      }
   }



////////////////////////////// pi+ pi- pi0 gamma fit /////////////////////////////////////////////////////////


   for ( FATable< NavShower >::const_iterator fastest_shower = showerBegin;
         fastest_shower != showerEnd ;
         ++fastest_shower )
   {
     
      if(fastest_shower->attributes().energy() > 0.5) {

         for (FATable< NavPi0ToGG >::const_iterator pi0Itr = pi0Begin;
             pi0Itr != pi0End ; ++pi0Itr) {

              if(pi0Itr.valid()
                 && pi0Itr->pi0().fit()
                 && pi0Itr->pi0().converged()
                 && (pi0Itr->pi0().chisq() < 500)
                 && (fabs(pi0Itr->pi0().pullMass()) < 3) 
                 && (pi0Itr->hiEnShower().identifier() != fastest_shower->identifier())           // make sure pi0 photons do not overlap with ISR photon
                 && (pi0Itr->loEnShower().identifier() != fastest_shower->identifier()) ) {

                FitEvt pipi1( "Pipi1", FitEvt::k_P4VecCM );
                pipi1.newPhoton(*tmp_sh);
                pipi1.newPi0(pi0Itr->hiEnShower(),pi0Itr->loEnShower(), FitEvtSettings::instance().beamSpotKD());
                pipi1.newTrack(*pi1);
                pipi1.newTrack(*pi2);
                pipi1.doTheFit();

                if( (pipi1.chisqVtx()>0) && (pipi1.chisqFit()>0) ) {

                  const HepLorentzVector vgpi1 (pipi1.kdFitVec()[0]->lorentzMomentum());
                  const HepLorentzVector v0pi1 (pipi1.kdFitVec()[1]->lorentzMomentum());
                  const HepLorentzVector v1pi1 (pipi1.kdFitVec()[2]->lorentzMomentum());
                  const HepLorentzVector v2pi1 (pipi1.kdFitVec()[3]->lorentzMomentum());

                  TempChisqF1 = pipi1.chisqFit();

                  if(TempChisqF1 < ChisqF1){

                    ChisqV1 = pipi1.chisqVtx();
                    ChisqF1 = pipi1.chisqFit();

                    F1E = vgpi1.e();
                    F1Theta = vgpi1.theta();
                    F1Phi = vgpi1.phi();

                    PI0F1PE = v0pi1.e();
                    PI0F1PX = v0pi1.px();
                    PI0F1PY = v0pi1.py();
                    PI0F1PZ = v0pi1.pz();

                    PI0F1HIE = pi0Itr->hiEnShower().attributes().energy(); // before fitting value
                    PI0F1LOE = pi0Itr->loEnShower().attributes().energy(); // before fitting value

                    PIF1PE[0]=v1pi1.e();
                    PIF1PX[0]=v1pi1.px();
                    PIF1PY[0]=v1pi1.py();
                    PIF1PZ[0]=v1pi1.pz();

                    PIF1PE[1]=v2pi1.e();
                    PIF1PX[1]=v2pi1.px();
                    PIF1PY[1]=v2pi1.py();
                    PIF1PZ[1]=v2pi1.pz();

                 }
               }
            }
          }
        }
      }


////////////////////////////// pi+ pi- pi0 pi0 gamma fit /////////////////////////////////////////////////////////


   for ( FATable< NavShower >::const_iterator fastest_shower = showerBegin;
         fastest_shower != showerEnd ;
         ++fastest_shower )
   {

      if(fastest_shower->attributes().energy() > 0.5) {

        for (FATable< NavPi0ToGG >::const_iterator pi0Itr = pi0Begin;
             pi0Itr != pi0End ; ++pi0Itr) {

          if(pi0Itr.valid()
             && pi0Itr->pi0().fit()
             && pi0Itr->pi0().converged()
             && (pi0Itr->pi0().chisq() < 500)
             && (fabs(pi0Itr->pi0().pullMass()) < 3) 
             && (pi0Itr->hiEnShower().identifier() != fastest_shower->identifier())           // make sure pi0 photons do not overlap with ISR photon
             && (pi0Itr->loEnShower().identifier() != fastest_shower->identifier()) ) {

         for (FATable< NavPi0ToGG >::const_iterator pi0Itr2 = pi0Itr+1;
               pi0Itr2 != pi0End ; ++pi0Itr2) {

            if(pi0Itr2.valid()
               && pi0Itr2->pi0().fit()
               && pi0Itr2->pi0().converged()
               && (pi0Itr2->pi0().chisq() < 500)
               && (fabs(pi0Itr2->pi0().pullMass()) < 3) 
               && (pi0Itr2->hiEnShower().identifier() != fastest_shower->identifier())          
               && (pi0Itr2->loEnShower().identifier() != fastest_shower->identifier())     // make sure pi0 photons do not overlap with ISR photon
               && (pi0Itr2->hiEnShower().identifier() != pi0Itr->hiEnShower().identifier())
               && (pi0Itr2->hiEnShower().identifier() != pi0Itr->loEnShower().identifier())
               && (pi0Itr2->loEnShower().identifier() != pi0Itr->hiEnShower().identifier()) // make sure pi0 photons do not overlap with each other
               && (pi0Itr2->loEnShower().identifier() != pi0Itr->loEnShower().identifier()) ) {


              FitEvt pipi2( "Pipi2", FitEvt::k_P4VecCM );
              pipi2.newPhoton(*tmp_sh);
              pipi2.newPi0(pi0Itr->hiEnShower(),pi0Itr->loEnShower(), FitEvtSettings::instance().beamSpotKD());
              pipi2.newPi0(pi0Itr2->hiEnShower(),pi0Itr2->loEnShower(), FitEvtSettings::instance().beamSpotKD());
              pipi2.newTrack(*pi1);
              pipi2.newTrack(*pi2);
              pipi2.doTheFit();

              if( (pipi2.chisqVtx()>0) && (pipi2.chisqFit()>0) ) {
 
                const HepLorentzVector vgpi2 (pipi2.kdFitVec()[0]->lorentzMomentum());
                const HepLorentzVector v1pi02 (pipi2.kdFitVec()[1]->lorentzMomentum());
                const HepLorentzVector v2pi02 (pipi2.kdFitVec()[2]->lorentzMomentum());
                const HepLorentzVector v1pi2 (pipi2.kdFitVec()[3]->lorentzMomentum());
                const HepLorentzVector v2pi2 (pipi2.kdFitVec()[4]->lorentzMomentum());

                TempChisqF2 = pipi2.chisqFit();
                if(TempChisqF2 < ChisqF2){

                  ChisqV2 = pipi2.chisqVtx();
                  ChisqF2 = pipi2.chisqFit();

                  F2E = vgpi2.e();
                  F2Theta = vgpi2.theta();
                  F2Phi = vgpi2.phi();

                  PI0F2PE[0] = v1pi02.e();
                  PI0F2PX[0] = v1pi02.px();
                  PI0F2PY[0] = v1pi02.py();
                  PI0F2PZ[0] = v1pi02.pz();
                  PI0F2HIE[0] = pi0Itr->hiEnShower().attributes().energy(); // before fitting value
                  PI0F2LOE[0] = pi0Itr->loEnShower().attributes().energy(); // before fitting value

                  PI0F2PE[1] = v2pi02.e();
                  PI0F2PX[1] = v2pi02.px();
                  PI0F2PY[1] = v2pi02.py();
                  PI0F2PZ[1] = v2pi02.pz();
                  PI0F2HIE[1] = pi0Itr2->hiEnShower().attributes().energy(); // before fitting value
                  PI0F2LOE[1] = pi0Itr2->loEnShower().attributes().energy(); // before fitting value

                  PIF2PE[0]=v1pi2.e();
                  PIF2PX[0]=v1pi2.px();
                  PIF2PY[0]=v1pi2.py();
                  PIF2PZ[0]=v1pi2.pz();

                  PIF2PE[1]=v2pi2.e();
                  PIF2PX[1]=v2pi2.px();
                  PIF2PY[1]=v2pi2.py();
                  PIF2PZ[1]=v2pi2.pz();
                }
             }
          }
         }
       }
     }
    }
   }

////////////////////////////// pi+ pi- pi0 pi0 fit //////////////////////////////////////////////////////////////


  
  for (FATable< NavPi0ToGG >::const_iterator pi0Itr = pi0Begin;
       pi0Itr != pi0End ; ++pi0Itr) {

     if(pi0Itr.valid()
        && pi0Itr->pi0().fit()
        && pi0Itr->pi0().converged()
        && (pi0Itr->pi0().chisq() < 500)
        && (fabs(pi0Itr->pi0().pullMass()) < 3)) {

       for (FATable< NavPi0ToGG >::const_iterator pi0Itr2 = pi0Itr+1;
           pi0Itr2 != pi0End ; ++pi0Itr2) {

         if(pi0Itr2.valid()
            && pi0Itr2->pi0().fit()
            && pi0Itr2->pi0().converged()
            && (pi0Itr2->pi0().chisq() < 500)
            && (fabs(pi0Itr2->pi0().pullMass()) < 3)
            && (pi0Itr2->hiEnShower().identifier() != pi0Itr->hiEnShower().identifier())
            && (pi0Itr2->hiEnShower().identifier() != pi0Itr->loEnShower().identifier())
            && (pi0Itr2->loEnShower().identifier() != pi0Itr->hiEnShower().identifier()) // make sure pi0 photons do not overlap with each other
            && (pi0Itr2->loEnShower().identifier() != pi0Itr->loEnShower().identifier()) ) {


           FitEvt pipi3( "Pipi3", FitEvt::k_P4VecCM );
           pipi3.newPi0(pi0Itr->hiEnShower(),pi0Itr->loEnShower(), FitEvtSettings::instance().beamSpotKD());
           pipi3.newPi0(pi0Itr2->hiEnShower(),pi0Itr2->loEnShower(), FitEvtSettings::instance().beamSpotKD());
           pipi3.newTrack(*pi1);
           pipi3.newTrack(*pi2);
           pipi3.doTheFit();

           if( (pipi3.chisqVtx()>0) && (pipi3.chisqFit()>0) ) {

             const HepLorentzVector v1pi03 (pipi3.kdFitVec()[0]->lorentzMomentum());
             const HepLorentzVector v2pi03 (pipi3.kdFitVec()[1]->lorentzMomentum());
             const HepLorentzVector v1pi3 (pipi3.kdFitVec()[2]->lorentzMomentum());
             const HepLorentzVector v2pi3 (pipi3.kdFitVec()[3]->lorentzMomentum());

             TempChisqF3 = pipi3.chisqFit();
             if(TempChisqF3 < ChisqF3){

               ChisqV3 = pipi3.chisqVtx();
               ChisqF3 = pipi3.chisqFit();

               PI0F3PE[0] = v1pi03.e();
               PI0F3PX[0] = v1pi03.px();
               PI0F3PY[0] = v1pi03.py();
               PI0F3PZ[0] = v1pi03.pz();
               PI0F3HIE[0] = pi0Itr->hiEnShower().attributes().energy(); // before fitting value
               PI0F3LOE[0] = pi0Itr->loEnShower().attributes().energy(); // before fitting value

               PI0F3PE[1] = v2pi03.e();
               PI0F3PX[1] = v2pi03.px();
               PI0F3PY[1] = v2pi03.py();
               PI0F3PZ[1] = v2pi03.pz();
               PI0F3HIE[1] = pi0Itr2->hiEnShower().attributes().energy(); // before fitting value
               PI0F3LOE[1] = pi0Itr2->loEnShower().attributes().energy(); // before fitting value

               PIF3PE[0]=v1pi3.e();
               PIF3PX[0]=v1pi3.px();
               PIF3PY[0]=v1pi3.py();
               PIF3PZ[0]=v1pi3.pz();

               PIF3PE[1]=v2pi3.e();
               PIF3PX[1]=v2pi3.px();
               PIF3PY[1]=v2pi3.py();
               PIF3PZ[1]=v2pi3.pz();
             }
          }
       }
   }
       }
     }

   delete tmp_sh;
   delete navsh_server;



   tuple[knshower] = unmatchedShowers;
   tuple[kntrack] = unmatchedTracks;
   tuple[knpi0] = unmatchedPi0s;

   tuple[chisqv1] = ChisqV1;
   tuple[chisqf1] = ChisqF1;
   tuple[chisqv2] = ChisqV2;
   tuple[chisqf2] = ChisqF2;
   tuple[chisqv0] = ChisqV0;
   tuple[chisqf0] = ChisqF0;

   tuple[e1] = E[0];
   tuple[th1] = Theta[0];
   tuple[phi1] = Phi[0];
   tuple[e9251] = E925[0];

   tuple[e2] = E[1];
   tuple[th2] = Theta[1];
   tuple[phi2] = Phi[1];
   tuple[e9252] = E925[1];

   tuple[e3] = E[2];
   tuple[th3] = Theta[2];
   tuple[phi3] = Phi[2];
   tuple[e9253] = E925[2];

   tuple[e4] = E[3];
   tuple[th4] = Theta[3];
   tuple[phi4] = Phi[3];
   tuple[e9254] = E925[3];

   tuple[e5] = E[4];
   tuple[th5] = Theta[4];
   tuple[phi5] = Phi[4];
   tuple[e9255] = E925[4];

   tuple[e6] = E[5];
   tuple[th6] = Theta[5];
   tuple[phi6] = Phi[5];
   tuple[e9256] = E925[5];

   tuple[e7] = E[6];
   tuple[th7] = Theta[6];
   tuple[phi7] = Phi[6];
   tuple[e9257] = E925[6];

   tuple[e8] = E[7];
   tuple[th8] = Theta[7];
   tuple[phi8] = Phi[7];
   tuple[e9258] = E925[7];

   tuple[e9] = E[8];
   tuple[th9] = Theta[8];
   tuple[phi9] = Phi[8];
   tuple[e9259] = E925[8];

   tuple[e10] = E[9];
   tuple[th10] = Theta[9];
   tuple[phi10] = Phi[9];
   tuple[e92510] = E925[9];

   tuple[e11] = E[10];
   tuple[th11] = Theta[10];
   tuple[phi11] = Phi[10];
   tuple[e92511] = E925[10];

   tuple[e12] = E[11];
   tuple[th12] = Theta[11];
   tuple[phi12] = Phi[11];
   tuple[e92512] = E925[11];

   tuple[e13] = E[12];
   tuple[th13] = Theta[12];
   tuple[phi13] = Phi[12];
   tuple[e92513] = E925[12];

   tuple[e14] = E[13];
   tuple[th14] = Theta[13];
   tuple[phi14] = Phi[13];
   tuple[e92514] = E925[13];

   tuple[e15] = E[14];
   tuple[th15] = Theta[14];
   tuple[phi15] = Phi[14];
   tuple[e92515] = E925[14];

   tuple[e16] = E[15];
   tuple[th16] = Theta[15];
   tuple[phi16] = Phi[15];
   tuple[e92516] = E925[15];

   tuple[me1] = ME[0];
   tuple[eop1] = EOP[0];
   tuple[me2] = ME[1];
   tuple[eop2] = EOP[1];

   tuple[fe1] = F1E;
   tuple[fth1] = F1Theta;
   tuple[fphi1] = F1Phi;
   tuple[fe2] = F2E;
   tuple[fth2] = F2Theta;
   tuple[fphi2] = F2Phi;
   tuple[fe0] = F0E;
   tuple[fth0] = F0Theta;
   tuple[fphi0] = F0Phi;

   tuple[pite1] = PIPE[0];
   tuple[pipx1] = PIPX[0];
   tuple[pipy1] = PIPY[0];
   tuple[pipz1] = PIPZ[0];
   tuple[pite2] = PIPE[1];
   tuple[pipx2] = PIPX[1];
   tuple[pipy2] = PIPY[1];
   tuple[pipz2] = PIPZ[1];

   tuple[pi0te1] = PI0PE[0];
   tuple[pi0px1] = PI0PX[0];
   tuple[pi0py1] = PI0PY[0];
   tuple[pi0pz1] = PI0PZ[0];
   tuple[pi0te2] = PI0PE[1];
   tuple[pi0px2] = PI0PX[1];
   tuple[pi0py2] = PI0PY[1];
   tuple[pi0pz2] = PI0PZ[1];
   tuple[pi0te3] = PI0PE[2];
   tuple[pi0px3] = PI0PX[2];
   tuple[pi0py3] = PI0PY[2];
   tuple[pi0pz3] = PI0PZ[2];
   tuple[pi0te4] = PI0PE[3];
   tuple[pi0px4] = PI0PX[3];
   tuple[pi0py4] = PI0PY[3];
   tuple[pi0pz4] = PI0PZ[3];
   tuple[pi0te5] = PI0PE[4];
   tuple[pi0px5] = PI0PX[4];
   tuple[pi0py5] = PI0PY[4];
   tuple[pi0pz5] = PI0PZ[4];
   tuple[pi0te6] = PI0PE[5];
   tuple[pi0px6] = PI0PX[5];
   tuple[pi0py6] = PI0PY[5];
   tuple[pi0pz6] = PI0PZ[5];
   tuple[pi0te7] = PI0PE[6];
   tuple[pi0px7] = PI0PX[6];
   tuple[pi0py7] = PI0PY[6];
   tuple[pi0pz7] = PI0PZ[6];
   tuple[pi0te8] = PI0PE[7];
   tuple[pi0px8] = PI0PX[7];
   tuple[pi0py8] = PI0PY[7];
   tuple[pi0pz8] = PI0PZ[7];

   tuple[pi0hie1] = PI0HIE[0];
   tuple[pi0hie2] = PI0HIE[1];
   tuple[pi0hie3] = PI0HIE[2];
   tuple[pi0hie4] = PI0HIE[3];
   tuple[pi0hie5] = PI0HIE[4];
   tuple[pi0hie6] = PI0HIE[5];
   tuple[pi0hie7] = PI0HIE[6];
   tuple[pi0hie8] = PI0HIE[7];
   tuple[pi0loe1] = PI0LOE[0];
   tuple[pi0loe2] = PI0LOE[1];
   tuple[pi0loe3] = PI0LOE[2];
   tuple[pi0loe4] = PI0LOE[3];
   tuple[pi0loe5] = PI0LOE[4];
   tuple[pi0loe6] = PI0LOE[5];
   tuple[pi0loe7] = PI0LOE[6];
   tuple[pi0loe8] = PI0LOE[7];

   tuple[mute1] = MUPE[0];
   tuple[mupx1] = MUPX[0];
   tuple[mupy1] = MUPY[0];
   tuple[mupz1] = MUPZ[0];
   tuple[mute2] = MUPE[1];
   tuple[mupx2] = MUPX[1];
   tuple[mupy2] = MUPY[1];
   tuple[mupz2] = MUPZ[1];

   tuple[kte1] = KPE[0];
   tuple[kpx1] = KPX[0];
   tuple[kpy1] = KPY[0];
   tuple[kpz1] = KPZ[0];
   tuple[kte2] = KPE[1];
   tuple[kpx2] = KPX[1];
   tuple[kpy2] = KPY[1];
   tuple[kpz2] = KPZ[1];

   tuple[pisigma1] = PPISIGMA[0];
   tuple[pill1] = PPILL[0];
   tuple[pisigma2] = PPISIGMA[1];
   tuple[pill2] = PPILL[1];

   tuple[musigma1] = PMUSIGMA[0];
   tuple[mull1] = PMULL[0];
   tuple[musigma2] = PMUSIGMA[1];
   tuple[mull2] = PMULL[1];

   tuple[ksigma1] = PKSIGMA[0];
   tuple[kll1] = PKLL[0];
   tuple[ksigma2] = PKSIGMA[1];
   tuple[kll2] = PKLL[1];

   tuple[f0pite1] = PIF0PE[0];
   tuple[f0pipx1] = PIF0PX[0];
   tuple[f0pipy1] = PIF0PY[0];
   tuple[f0pipz1] = PIF0PZ[0];
   tuple[f0pite2] = PIF0PE[1];
   tuple[f0pipx2] = PIF0PX[1];
   tuple[f0pipy2] = PIF0PY[1];
   tuple[f0pipz2] = PIF0PZ[1];

   tuple[f1pite1] = PIF1PE[0];
   tuple[f1pipx1] = PIF1PX[0];
   tuple[f1pipy1] = PIF1PY[0];
   tuple[f1pipz1] = PIF1PZ[0];
   tuple[f1pite2] = PIF1PE[1];
   tuple[f1pipx2] = PIF1PX[1];
   tuple[f1pipy2] = PIF1PY[1];
   tuple[f1pipz2] = PIF1PZ[1];

   tuple[f2pite1] = PIF2PE[0];
   tuple[f2pipx1] = PIF2PX[0];
   tuple[f2pipy1] = PIF2PY[0];
   tuple[f2pipz1] = PIF2PZ[0];
   tuple[f2pite2] = PIF2PE[1];
   tuple[f2pipx2] = PIF2PX[1];
   tuple[f2pipy2] = PIF2PY[1];
   tuple[f2pipz2] = PIF2PZ[1];

   tuple[f3pite1] = PIF3PE[0];
   tuple[f3pipx1] = PIF3PX[0];
   tuple[f3pipy1] = PIF3PY[0];
   tuple[f3pipz1] = PIF3PZ[0];
   tuple[f3pite2] = PIF3PE[1];
   tuple[f3pipx2] = PIF3PX[1];
   tuple[f3pipy2] = PIF3PY[1];
   tuple[f3pipz2] = PIF3PZ[1];

   tuple[f1pi0te] = PI0F1PE;
   tuple[f1pi0px] = PI0F1PX;
   tuple[f1pi0py] = PI0F1PY;
   tuple[f1pi0pz] = PI0F1PZ;
   tuple[f1pi0hie] = PI0F1HIE;
   tuple[f1pi0loe] = PI0F1LOE;

   tuple[f2pi0te1] = PI0F2PE[0];
   tuple[f2pi0px1] = PI0F2PX[0];
   tuple[f2pi0py1] = PI0F2PY[0];
   tuple[f2pi0pz1] = PI0F2PZ[0];
   tuple[f2pi0hie1] = PI0F2HIE[0];
   tuple[f2pi0loe1] = PI0F2LOE[0];

   tuple[f2pi0te2] = PI0F2PE[1];
   tuple[f2pi0px2] = PI0F2PX[1];
   tuple[f2pi0py2] = PI0F2PY[1];
   tuple[f2pi0pz2] = PI0F2PZ[1];
   tuple[f2pi0hie2] = PI0F2HIE[1];
   tuple[f2pi0loe2] = PI0F2LOE[1];

   tuple[f3pi0te1] = PI0F3PE[0];
   tuple[f3pi0px1] = PI0F3PX[0];
   tuple[f3pi0py1] = PI0F3PY[0];
   tuple[f3pi0pz1] = PI0F3PZ[0];
   tuple[f3pi0hie1] = PI0F3HIE[0];
   tuple[f3pi0loe1] = PI0F3LOE[0];

   tuple[f3pi0te2] = PI0F3PE[1];
   tuple[f3pi0px2] = PI0F3PX[1];
   tuple[f3pi0py2] = PI0F3PY[1];
   tuple[f3pi0pz2] = PI0F3PZ[1];
   tuple[f3pi0hie2] = PI0F3HIE[1];
   tuple[f3pi0loe2] = PI0F3LOE[1];

   tuple[charge1] = CHARGE[0];
   tuple[charge2] = CHARGE[1];

   (*m_showerTuple).fill(tuple);


   return ActionBase::kPassed;
}


ActionBase::ActionResult
PipipiProc::beginRun( Frame& iFrame )       // anal2 equiv.
{
   report( DEBUG, kFacilityString ) << "here in beginRun()" << endl;

   FAItem< BeamSpot > spot;
   extract( iFrame.record( Stream::kBeginRun ), spot );

   FAItem< BeamEnergy > beam_energy;
   extract( iFrame.record( Stream::kBeginRun ), beam_energy );

   FAItem< MagneticField > cleoBField;
   extract( iFrame.record( Stream::kBeginRun ), cleoBField );

   FitEvtSettings &settings(FitEvtSettings::instance());
   settings.setField(*cleoBField);
   settings.setBeamSpot(*spot);
   settings.setLorVecCM( beam_energy->value() );
//   settings.setVerbose();

   return ActionBase::kPassed;
}


/*
ActionBase::ActionResult
PipipiProc::endRun( Frame& iFrame )         // anal4 equiv.
{
   report( DEBUG, kFacilityString ) << "here in endRun()" << endl;

   return ActionBase::kPassed;
}
*/

//
// const member functions
//

//
// static member functions
//
