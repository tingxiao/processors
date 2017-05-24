// -*- C++ -*-
//
// Package:     TwoTracksISRProc
// Module:      TwoTracksISRProc
// 
// Description: 1 shower & 2 tracks
//
// Implementation:
//     <Notes on implementation>
//
// Author:      Ting Xiao
// Created:     Fri Jun 26 18:02:00 EDT 2009
// $Id$
//
// Revision history
//
// $Log$
//

#include "Experiment/Experiment.h"

// system include files

// user include files
#include "TwoTracksISRProc/TwoTracksISRProc.h"
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
#include "C3Mu/MuTrack.h"
#include "Navigation/NavMuonId.h"

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

#include "CleoDB/DBEventHeader.h" 
#include "TriggerData/TriggerData.h"
#include "TriggerL1Data/TriggerL1Data.h" 
#include "Level4Proc/Level4Decision.h"

#include "MCInfo/MCDecayTree/MCDecayTree.h"
#include "MCInfo/MCDecayTree/MCParticle.h"
#include "MCInfo/MCDecayTree/MCVertex.h"
#include "MCInfo/MCParticleProperty/MCParticleProperty.h"

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
static const char* const kFacilityString = "Processor.TwoTracksISRProc" ;

// ---- cvs-based strings (Id and Tag with which file was checked out)
static const char* const kIdString  = "$Id: processor.cc,v 1.41 2006/02/08 19:38:02 wsun Exp $";
static const char* const kTagString = "$Name: v07_03_00 $";

//
// static data member definitions
//



//
// constructors and destructor
//
TwoTracksISRProc::TwoTracksISRProc( void )               // anal1
   : Processor( "TwoTracksISRProc" )
{
   report( DEBUG, kFacilityString ) << "here in ctor()" << endl;

   // ---- bind a method to a stream -----
   // These lines ARE VERY IMPORTANT! If you don't bind the 
   // code you've just written (the "action") to a stream, 
   // your code won't get executed!

   bind( &TwoTracksISRProc::event,    Stream::kEvent );
   bind( &TwoTracksISRProc::beginRun, Stream::kBeginRun );
   //bind( &TwoTracksISRProc::endRun,   Stream::kEndRun );

   // do anything here that needs to be done at creation time
   // (e.g. allocate resources etc.)


}

TwoTracksISRProc::~TwoTracksISRProc()                    // anal5
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
TwoTracksISRProc::init( void )          // anal1 "Interactive"
{
   report( DEBUG, kFacilityString ) << "here in init()" << endl;

   // do any initialization here based on Parameter Input by User
   // (e.g. run expensive algorithms that are based on parameters
   //  specified by user at run-time)

}

// -------------------- terminate method ----------------------------
void
TwoTracksISRProc::terminate( void )     // anal5 "Interactive"
{
   report( DEBUG, kFacilityString ) << "here in terminate()" << endl;

   // do anything here BEFORE New Parameter Change
   // (e.g. write out result based on parameters from user-input)
 
}


// ---------------- standard place to book histograms ---------------

enum {knshower, ccknshower,
kntrack1shower, kntrack2shower,  // number of showers matched to a track
chisqvpi,chisqfpi, //chisq of pion fit
chisqvmu,chisqfmu,    // chisq of muon fit
chisqvk,chisqfk,      // chisq of kaon fit
chisqvpi1,chisqfpi1, //chisq of pion fit (additional FSR)
chisqvmu1,chisqfmu1, //chisq of muon fit (additional FSR)
chisqvk1,chisqfk1, //chisq of kaon fit (additional FSR)

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

cce1,ccth1,ccphi1,cce9251,ccd1p,ccd1n,       // without GoodThingsProd
cce2,ccth2,ccphi2,cce9252,ccd2p,ccd2n,
cce3,ccth3,ccphi3,cce9253,ccd3p,ccd3n,
cce4,ccth4,ccphi4,cce9254,ccd4p,ccd4n,
cce5,ccth5,ccphi5,cce9255,ccd5p,ccd5n,
cce6,ccth6,ccphi6,cce9256,ccd6p,ccd6n,
cce7,ccth7,ccphi7,cce9257,ccd7p,ccd7n,
cce8,ccth8,ccphi8,cce9258,ccd8p,ccd8n,
cce9,ccth9,ccphi9,cce9259,ccd9p,ccd9n,
cce10,ccth10,ccphi10,cce92510,ccd10p,ccd10n,
cce11,ccth11,ccphi11,cce92511,ccd11p,ccd11n,
cce12,ccth12,ccphi12,cce92512,ccd12p,ccd12n,
cce13,ccth13,ccphi13,cce92513,ccd13p,ccd13n,
cce14,ccth14,ccphi14,cce92514,ccd14p,ccd14n,
cce15,ccth15,ccphi15,cce92515,ccd15p,ccd15n,
cce16,ccth16,ccphi16,cce92516,ccd16p,ccd16n,
cce17,ccth17,ccphi17,cce92517,ccd17p,ccd17n,
cce18,ccth18,ccphi18,cce92518,ccd18p,ccd18n,
cce19,ccth19,ccphi19,cce92519,ccd19p,ccd19n,
cce20,ccth20,ccphi20,cce92520,ccd20p,ccd20n,

me1,eop1,me2,eop2,  //matched energy and E/p
meop1,meop2,dis1,dis2, //E/p and distance from track and matched shower
dis101,dis102,dis103,dis104,dis105,dis106,dis107,dis108,dis109,dis110,
dis201,dis202,dis203,dis204,dis205,dis206,dis207,dis208,dis209,dis210,
me101,me102,me103,me104,me105,me106,me107,me108,me109,me110,
me201,me202,me203,me204,me205,me206,me207,me208,me209,me210,
conme101,conme102,conme103,conme104,conme105,conme106,conme107,conme108,conme109,conme110,
conme201,conme202,conme203,conme204,conme205,conme206,conme207,conme208,conme209,conme210,
mme1,mme2,
mudepth1,mudepth2,  //muon depth 

fepi,fthpi,fphipi,  // fitted photon energy fitted with pions
femu,fthmu,fphimu,  // fitted photon energy fitted with muons
fek,fthk,fphik,   // fitted photon energy fitted with kaons

epi2,thpi2,phipi2,  // unfitted photon energy fitted with pions (additional FSR)

f1epi1,f1thpi1,f1phipi1,  // fitted photon energy fitted with pions (additional FSR)
f1epi2,f1thpi2,f1phipi2,  //additional FSR
f1emu1,f1thmu1,f1phimu1,  // fitted photon energy fitted with muons (additional FSR)
f1emu2,f1thmu2,f1phimu2,  //additional FSR
f1ek1,f1thk1,f1phik1,  // fitted photon energy fitted with kaons (additional FSR)
f1ek2,f1thk2,f1phik2,  //additional FSR

pite1,pipx1,pipy1,pipz1,mute1,mupx1,mupy1,mupz1,kte1,kpx1,kpy1,kpz1,
pisigma1,musigma1,pill1,mull1,ksigma1,kll1,
pite2,pipx2,pipy2,pipz2,mute2,mupx2,mupy2,mupz2,kte2,kpx2,kpy2,kpz2,
pisigma2,musigma2,pill2,mull2,ksigma2,kll2,

fpite1,fpipx1,fpipy1,fpipz1,fmute1,fmupx1,fmupy1,fmupz1,fkte1,fkpx1,fkpy1,fkpz1,
fpite2,fpipx2,fpipy2,fpipz2,fmute2,fmupx2,fmupy2,fmupz2,fkte2,fkpx2,fkpy2,fkpz2,

f1pite1,f1pipx1,f1pipy1,f1pipz1,f1pite2,f1pipx2,f1pipy2,f1pipz2, //(additional FSR)
f1mute1,f1mupx1,f1mupy1,f1mupz1,f1mute2,f1mupx2,f1mupy2,f1mupz2, //(additional FSR)
f1kte1,f1kpx1,f1kpy1,f1kpz1,f1kte2,f1kpx2,f1kpy2,f1kpz2, //(additional FSR)

charge1, charge2, //charges of the 2 tracks
beam_e, // beam energy
m_trk,  //track mass variable
mmpi_gen, // m(pipi) at glevel

l1trigger1,l1trigger2,l1trigger3,l1trigger4,l1trigger5,l1trigger6,l1trigger7,l1trigger8,
l1trigger9,l1trigger10,l1trigger11,l1trigger12,l1trigger13,l1trigger14,l1trigger15,l1trigger16,

kVarNum};


void
TwoTracksISRProc::hist_book( HIHistoManager& iHistoManager )
{
   report( DEBUG, kFacilityString ) << "here in hist_book()" << endl;

   // book your histograms here
   HINtupleVarNames ntupleNames(kVarNum);

   ntupleNames.addVar(knshower,       "knshower");
   ntupleNames.addVar(ccknshower,       "ccknshower");

   ntupleNames.addVar(kntrack1shower,       "kntrack1shower");
   ntupleNames.addVar(kntrack2shower,       "kntrack2shower");

   ntupleNames.addVar(chisqvpi,       "chisqvpi");
   ntupleNames.addVar(chisqfpi,       "chisqfpi");

   ntupleNames.addVar(chisqvmu,       "chisqvmu");
   ntupleNames.addVar(chisqfmu,       "chisqfmu");

   ntupleNames.addVar(chisqvk,       "chisqvk");
   ntupleNames.addVar(chisqfk,       "chisqfk");

   ntupleNames.addVar(chisqvpi1,       "chisqvpi1");
   ntupleNames.addVar(chisqfpi1,       "chisqfpi1");

   ntupleNames.addVar(chisqvmu1,       "chisqvmu1");
   ntupleNames.addVar(chisqfmu1,       "chisqfmu1");

   ntupleNames.addVar(chisqvk1,       "chisqvk1");
   ntupleNames.addVar(chisqfk1,       "chisqfk1");

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

   ntupleNames.addVar(cce1,          "cce1");
   ntupleNames.addVar(ccth1,        "ccth1");
   ntupleNames.addVar(ccphi1,	  "ccphi1");
   ntupleNames.addVar(cce9251,    "cce9251");

   ntupleNames.addVar(cce2,          "cce2");
   ntupleNames.addVar(ccth2,        "ccth2");
   ntupleNames.addVar(ccphi2,	  "ccphi2");
   ntupleNames.addVar(cce9252,    "cce9252");

   ntupleNames.addVar(cce3,          "cce3");
   ntupleNames.addVar(ccth3,        "ccth3");
   ntupleNames.addVar(ccphi3,	  "ccphi3");
   ntupleNames.addVar(cce9253,    "cce9253");

   ntupleNames.addVar(cce4,          "cce4");
   ntupleNames.addVar(ccth4,        "ccth4");
   ntupleNames.addVar(ccphi4,       "ccphi4");
   ntupleNames.addVar(cce9254,    "cce9254");

   ntupleNames.addVar(cce5,          "cce5");
   ntupleNames.addVar(ccth5,        "ccth5");
   ntupleNames.addVar(ccphi5,       "ccphi5");
   ntupleNames.addVar(cce9255,    "cce9255");

   ntupleNames.addVar(cce6,          "cce6");
   ntupleNames.addVar(ccth6,        "ccth6");
   ntupleNames.addVar(ccphi6,	  "ccphi6");
   ntupleNames.addVar(cce9256,    "cce9256");

   ntupleNames.addVar(cce7,          "cce7");
   ntupleNames.addVar(ccth7,        "ccth7");
   ntupleNames.addVar(ccphi7,	  "ccphi7");
   ntupleNames.addVar(cce9257,    "cce9257");

   ntupleNames.addVar(cce8,          "cce8");
   ntupleNames.addVar(ccth8,        "ccth8");
   ntupleNames.addVar(ccphi8,       "ccphi8");
   ntupleNames.addVar(cce9258,    "cce9258");

   ntupleNames.addVar(cce9,          "cce9");
   ntupleNames.addVar(ccth9,        "ccth9");
   ntupleNames.addVar(ccphi9,	  "ccphi9");
   ntupleNames.addVar(cce9259,    "cce9259");

   ntupleNames.addVar(cce10,  	    "cce10");
   ntupleNames.addVar(ccth10,        "ccth10");
   ntupleNames.addVar(ccphi10,       "ccphi10");
   ntupleNames.addVar(cce92510,    "cce92510");

   ntupleNames.addVar(cce11,          "cce11");
   ntupleNames.addVar(ccth11,        "ccth11");
   ntupleNames.addVar(ccphi11,     "ccphi11");
   ntupleNames.addVar(cce92511,    "cce92511");

   ntupleNames.addVar(cce12,          "cce12");
   ntupleNames.addVar(ccth12,        "ccth12");
   ntupleNames.addVar(ccphi12,     "ccphi12");
   ntupleNames.addVar(cce92512,    "cce92512");

   ntupleNames.addVar(cce13,          "cce13");
   ntupleNames.addVar(ccth13,        "ccth13");
   ntupleNames.addVar(ccphi13,     "ccphi13");
   ntupleNames.addVar(cce92513,    "cce92513");

   ntupleNames.addVar(cce14,          "cce14");
   ntupleNames.addVar(ccth14,        "ccth14");
   ntupleNames.addVar(ccphi14,       "ccphi14");
   ntupleNames.addVar(cce92514,    "cce92514");

   ntupleNames.addVar(cce15,          "cce15");
   ntupleNames.addVar(ccth15,        "ccth15");
   ntupleNames.addVar(ccphi15,       "ccphi15");
   ntupleNames.addVar(cce92515,    "cce92515");

   ntupleNames.addVar(cce16,          "cce16");
   ntupleNames.addVar(ccth16,        "ccth16");
   ntupleNames.addVar(ccphi16,     "ccphi16");
   ntupleNames.addVar(cce92516,    "cce92516");

   ntupleNames.addVar(cce17,          "cce17");
   ntupleNames.addVar(ccth17,        "ccth17");
   ntupleNames.addVar(ccphi17,     "ccphi17");
   ntupleNames.addVar(cce92517,    "cce92517");

   ntupleNames.addVar(cce18,          "cce18");
   ntupleNames.addVar(ccth18,        "ccth18");
   ntupleNames.addVar(ccphi18,       "ccphi18");
   ntupleNames.addVar(cce92518,    "cce92518");

   ntupleNames.addVar(cce19,          "cce19");
   ntupleNames.addVar(ccth19,        "ccth19");
   ntupleNames.addVar(ccphi19,     "ccphi19");
   ntupleNames.addVar(cce92519,    "cce92519");

   ntupleNames.addVar(cce20,	    "cce20");
   ntupleNames.addVar(ccth20,        "ccth20");
   ntupleNames.addVar(ccphi20,       "ccphi20");
   ntupleNames.addVar(cce92520,    "cce92520");

   ntupleNames.addVar(ccd1p,        "ccd1p");
   ntupleNames.addVar(ccd2p,        "ccd2p");
   ntupleNames.addVar(ccd3p,        "ccd3p");
   ntupleNames.addVar(ccd4p,        "ccd4p");
   ntupleNames.addVar(ccd5p,        "ccd5p");
   ntupleNames.addVar(ccd6p,        "ccd6p");
   ntupleNames.addVar(ccd7p,        "ccd7p");
   ntupleNames.addVar(ccd8p,        "ccd8p");
   ntupleNames.addVar(ccd9p,        "ccd9p");
   ntupleNames.addVar(ccd10p,        "ccd10p");

   ntupleNames.addVar(ccd11p,        "ccd11p");
   ntupleNames.addVar(ccd12p,        "ccd12p");
   ntupleNames.addVar(ccd13p,        "ccd13p");
   ntupleNames.addVar(ccd14p,        "ccd14p");
   ntupleNames.addVar(ccd15p,        "ccd15p");
   ntupleNames.addVar(ccd16p,        "ccd16p");
   ntupleNames.addVar(ccd17p,        "ccd17p");
   ntupleNames.addVar(ccd18p,        "ccd18p");
   ntupleNames.addVar(ccd19p,        "ccd19p");
   ntupleNames.addVar(ccd20p,        "ccd20p");

   ntupleNames.addVar(ccd1n,        "ccd1n");
   ntupleNames.addVar(ccd2n,        "ccd2n");
   ntupleNames.addVar(ccd3n,        "ccd3n");
   ntupleNames.addVar(ccd4n,        "ccd4n");
   ntupleNames.addVar(ccd5n,        "ccd5n");
   ntupleNames.addVar(ccd6n,        "ccd6n");
   ntupleNames.addVar(ccd7n,        "ccd7n");
   ntupleNames.addVar(ccd8n,        "ccd8n");
   ntupleNames.addVar(ccd9n,        "ccd9n");
   ntupleNames.addVar(ccd10n,        "ccd10n");

   ntupleNames.addVar(ccd11n,        "ccd11n");
   ntupleNames.addVar(ccd12n,        "ccd12n");
   ntupleNames.addVar(ccd13n,        "ccd13n");
   ntupleNames.addVar(ccd14n,        "ccd14n");
   ntupleNames.addVar(ccd15n,        "ccd15n");
   ntupleNames.addVar(ccd16n,        "ccd16n");
   ntupleNames.addVar(ccd17n,        "ccd17n");
   ntupleNames.addVar(ccd18n,        "ccd18n");
   ntupleNames.addVar(ccd19n,        "ccd19n");
   ntupleNames.addVar(ccd20n,        "ccd20n");


   ntupleNames.addVar(me1,          "me1");
   ntupleNames.addVar(eop1,          "eop1");
   ntupleNames.addVar(me2,          "me2");
   ntupleNames.addVar(eop2,          "eop2");
   ntupleNames.addVar(mme1,          "mme1");
   ntupleNames.addVar(mme2,          "mme2");

   ntupleNames.addVar(meop1,          "meop1");
   ntupleNames.addVar(meop2,          "meop2");
   ntupleNames.addVar(dis1,          "dis1");
   ntupleNames.addVar(dis2,          "dis2");

   ntupleNames.addVar(dis101,          "dis101");
   ntupleNames.addVar(dis102,          "dis102");
   ntupleNames.addVar(dis103,          "dis103");
   ntupleNames.addVar(dis104,          "dis104");
   ntupleNames.addVar(dis105,          "dis105");
   ntupleNames.addVar(dis106,          "dis106");
   ntupleNames.addVar(dis107,          "dis107");
   ntupleNames.addVar(dis108,          "dis108");
   ntupleNames.addVar(dis109,          "dis109");
   ntupleNames.addVar(dis110,          "dis110");
   ntupleNames.addVar(dis201,          "dis201");
   ntupleNames.addVar(dis202,          "dis202");
   ntupleNames.addVar(dis203,          "dis203");
   ntupleNames.addVar(dis204,          "dis204");
   ntupleNames.addVar(dis205,          "dis205");
   ntupleNames.addVar(dis206,          "dis206");
   ntupleNames.addVar(dis207,          "dis207");
   ntupleNames.addVar(dis208,          "dis208");
   ntupleNames.addVar(dis209,          "dis209");
   ntupleNames.addVar(dis210,          "dis210");

   ntupleNames.addVar(me101,          "me101");
   ntupleNames.addVar(me102,          "me102");
   ntupleNames.addVar(me103,          "me103");
   ntupleNames.addVar(me104,          "me104");
   ntupleNames.addVar(me105,          "me105");
   ntupleNames.addVar(me106,          "me106");
   ntupleNames.addVar(me107,          "me107");
   ntupleNames.addVar(me108,          "me108");
   ntupleNames.addVar(me109,          "me109");
   ntupleNames.addVar(me110,          "me110");

   ntupleNames.addVar(me201,          "me201");
   ntupleNames.addVar(me202,          "me202");
   ntupleNames.addVar(me203,          "me203");
   ntupleNames.addVar(me204,          "me204");
   ntupleNames.addVar(me205,          "me205");
   ntupleNames.addVar(me206,          "me206");
   ntupleNames.addVar(me207,          "me207");
   ntupleNames.addVar(me208,          "me208");
   ntupleNames.addVar(me209,          "me209");
   ntupleNames.addVar(me210,          "me210");

   ntupleNames.addVar(conme101,          "conme101");
   ntupleNames.addVar(conme102,          "conme102");
   ntupleNames.addVar(conme103,          "conme103");
   ntupleNames.addVar(conme104,          "conme104");
   ntupleNames.addVar(conme105,          "conme105");
   ntupleNames.addVar(conme106,          "conme106");
   ntupleNames.addVar(conme107,          "conme107");
   ntupleNames.addVar(conme108,          "conme108");
   ntupleNames.addVar(conme109,          "conme109");
   ntupleNames.addVar(conme110,          "conme110");

   ntupleNames.addVar(conme201,          "conme201");
   ntupleNames.addVar(conme202,          "conme202");
   ntupleNames.addVar(conme203,          "conme203");
   ntupleNames.addVar(conme204,          "conme204");
   ntupleNames.addVar(conme205,          "conme205");
   ntupleNames.addVar(conme206,          "conme206");
   ntupleNames.addVar(conme207,          "conme207");
   ntupleNames.addVar(conme208,          "conme208");
   ntupleNames.addVar(conme209,          "conme209");
   ntupleNames.addVar(conme210,          "conme210");


   ntupleNames.addVar(mudepth1,      "mudepth1");
   ntupleNames.addVar(mudepth2,      "mudepth2");

   ntupleNames.addVar(epi2,          "epi2");
   ntupleNames.addVar(thpi2,        "thpi2");
   ntupleNames.addVar(phipi2,       "phipi2");


   ntupleNames.addVar(fepi,          "fepi");
   ntupleNames.addVar(fthpi,        "fthpi");
   ntupleNames.addVar(fphipi,       "fphipi");

   ntupleNames.addVar(femu,          "femu");
   ntupleNames.addVar(fthmu,        "fthmu");
   ntupleNames.addVar(fphimu,       "fphimu");

   ntupleNames.addVar(fek,          "fek");
   ntupleNames.addVar(fthk,        "fthk");
   ntupleNames.addVar(fphik,       "fphik");

   ntupleNames.addVar(f1epi1,          "f1epi1");
   ntupleNames.addVar(f1thpi1,        "f1thpi1");
   ntupleNames.addVar(f1phipi1,       "f1phipi1");

   ntupleNames.addVar(f1epi2,          "f1epi2");
   ntupleNames.addVar(f1thpi2,        "f1thpi2");
   ntupleNames.addVar(f1phipi2,       "f1phipi2");

   ntupleNames.addVar(f1emu1,          "f1emu1");
   ntupleNames.addVar(f1thmu1,        "f1thmu1");
   ntupleNames.addVar(f1phimu1,       "f1phimu1");

   ntupleNames.addVar(f1emu2,          "f1emu2");
   ntupleNames.addVar(f1thmu2,        "f1thmu2");
   ntupleNames.addVar(f1phimu2,       "f1phimu2");

   ntupleNames.addVar(f1ek1,          "f1ek1");
   ntupleNames.addVar(f1thk1,        "f1thk1");
   ntupleNames.addVar(f1phik1,       "f1phik1");

   ntupleNames.addVar(f1ek2,          "f1ek2");
   ntupleNames.addVar(f1thk2,        "f1thk2");
   ntupleNames.addVar(f1phik2,       "f1phik2");

   ntupleNames.addVar(pite1,    "pite1");
   ntupleNames.addVar(pipx1,    "pipx1");
   ntupleNames.addVar(pipy1,    "pipy1");
   ntupleNames.addVar(pipz1,    "pipz1");

   ntupleNames.addVar(mute1,    "mute1");
   ntupleNames.addVar(mupx1,    "mupx1");
   ntupleNames.addVar(mupy1,    "mupy1");
   ntupleNames.addVar(mupz1,    "mupz1");

   ntupleNames.addVar(kte1,    "kte1");
   ntupleNames.addVar(kpx1,    "kpx1");
   ntupleNames.addVar(kpy1,    "kpy1");
   ntupleNames.addVar(kpz1,    "kpz1");

   ntupleNames.addVar(pisigma1, "pisigma1");
   ntupleNames.addVar(musigma1, "musigma1");
   ntupleNames.addVar(ksigma1, "ksigma1");

   ntupleNames.addVar(pill1,    "pill1");
   ntupleNames.addVar(mull1,    "mull1");
   ntupleNames.addVar(kll1,    "kll1");

   ntupleNames.addVar(pite2,    "pite2");
   ntupleNames.addVar(pipx2,    "pipx2");
   ntupleNames.addVar(pipy2,    "pipy2");
   ntupleNames.addVar(pipz2,    "pipz2");

   ntupleNames.addVar(mute2,    "mute2");
   ntupleNames.addVar(mupx2,    "mupx2");
   ntupleNames.addVar(mupy2,    "mupy2");
   ntupleNames.addVar(mupz2,    "mupz2");

   ntupleNames.addVar(kte2,    "kte2");
   ntupleNames.addVar(kpx2,    "kpx2");
   ntupleNames.addVar(kpy2,    "kpy2");
   ntupleNames.addVar(kpz2,    "kpz2");

   ntupleNames.addVar(pisigma2, "pisigma2");
   ntupleNames.addVar(musigma2, "musigma2");
   ntupleNames.addVar(ksigma2, "ksigma2");

   ntupleNames.addVar(pill2,    "pill2");
   ntupleNames.addVar(mull2,    "mull2");
   ntupleNames.addVar(kll2,    "kll2");

   ntupleNames.addVar(fpite1,    "fpite1");
   ntupleNames.addVar(fpipx1,    "fpipx1");
   ntupleNames.addVar(fpipy1,    "fpipy1");
   ntupleNames.addVar(fpipz1,    "fpipz1");
   ntupleNames.addVar(fmute1,    "fmute1");
   ntupleNames.addVar(fmupx1,    "fmupx1");
   ntupleNames.addVar(fmupy1,    "fmupy1");
   ntupleNames.addVar(fmupz1,    "fmupz1");
   ntupleNames.addVar(fkte1,    "fkte1");
   ntupleNames.addVar(fkpx1,    "fkpx1");
   ntupleNames.addVar(fkpy1,    "fkpy1");
   ntupleNames.addVar(fkpz1,    "fkpz1");

   ntupleNames.addVar(fpite2,    "fpite2");
   ntupleNames.addVar(fpipx2,    "fpipx2");
   ntupleNames.addVar(fpipy2,    "fpipy2");
   ntupleNames.addVar(fpipz2,    "fpipz2");
   ntupleNames.addVar(fmute2,    "fmute2");
   ntupleNames.addVar(fmupx2,    "fmupx2");
   ntupleNames.addVar(fmupy2,    "fmupy2");
   ntupleNames.addVar(fmupz2,    "fmupz2");
   ntupleNames.addVar(fkte2,    "fkte2");
   ntupleNames.addVar(fkpx2,    "fkpx2");
   ntupleNames.addVar(fkpy2,    "fkpy2");
   ntupleNames.addVar(fkpz2,    "fkpz2");

   ntupleNames.addVar(f1pite1,    "f1pite1");
   ntupleNames.addVar(f1pipx1,    "f1pipx1");
   ntupleNames.addVar(f1pipy1,    "f1pipy1");
   ntupleNames.addVar(f1pipz1,    "f1pipz1");
   ntupleNames.addVar(f1pite2,    "f1pite2");
   ntupleNames.addVar(f1pipx2,    "f1pipx2");
   ntupleNames.addVar(f1pipy2,    "f1pipy2");
   ntupleNames.addVar(f1pipz2,    "f1pipz2");
   ntupleNames.addVar(f1mute1,    "f1mute1");
   ntupleNames.addVar(f1mupx1,    "f1mupx1");
   ntupleNames.addVar(f1mupy1,    "f1mupy1");
   ntupleNames.addVar(f1mupz1,    "f1mupz1");
   ntupleNames.addVar(f1mute2,    "f1mute2");
   ntupleNames.addVar(f1mupx2,    "f1mupx2");
   ntupleNames.addVar(f1mupy2,    "f1mupy2");
   ntupleNames.addVar(f1mupz2,    "f1mupz2");
   ntupleNames.addVar(f1kte1,    "f1kte1");
   ntupleNames.addVar(f1kpx1,    "f1kpx1");
   ntupleNames.addVar(f1kpy1,    "f1kpy1");
   ntupleNames.addVar(f1kpz1,    "f1kpz1");
   ntupleNames.addVar(f1kte2,    "f1kte2");
   ntupleNames.addVar(f1kpx2,    "f1kpx2");
   ntupleNames.addVar(f1kpy2,    "f1kpy2");
   ntupleNames.addVar(f1kpz2,    "f1kpz2");

   ntupleNames.addVar(charge1,    "charge1");
   ntupleNames.addVar(charge2,    "charge2");

   ntupleNames.addVar(beam_e,"beam_e");
   ntupleNames.addVar(m_trk,"m_trk");
   ntupleNames.addVar(mmpi_gen,"mmpi_gen");

   ntupleNames.addVar(l1trigger1,          "l1trigger1");
   ntupleNames.addVar(l1trigger2,          "l1trigger2");
   ntupleNames.addVar(l1trigger3,          "l1trigger3");
   ntupleNames.addVar(l1trigger4,          "l1trigger4");
   ntupleNames.addVar(l1trigger5,          "l1trigger5");
   ntupleNames.addVar(l1trigger6,          "l1trigger6");
   ntupleNames.addVar(l1trigger7,          "l1trigger7");
   ntupleNames.addVar(l1trigger8,          "l1trigger8");
   ntupleNames.addVar(l1trigger9,          "l1trigger9");
   ntupleNames.addVar(l1trigger10,          "l1trigger10");
   ntupleNames.addVar(l1trigger11,          "l1trigger11");
   ntupleNames.addVar(l1trigger12,          "l1trigger12");
   ntupleNames.addVar(l1trigger13,          "l1trigger13");
   ntupleNames.addVar(l1trigger14,          "l1trigger14");
   ntupleNames.addVar(l1trigger15,          "l1trigger15");
   ntupleNames.addVar(l1trigger16,          "l1trigger16");

   m_showerTuple = iHistoManager.ntuple(10,"tuple", kVarNum, 10000, ntupleNames.names());


}

// --------------------- methods bound to streams -------------------
ActionBase::ActionResult
TwoTracksISRProc::event( Frame& iFrame )          // anal3 equiv.
{
   report( DEBUG, kFacilityString ) << "here in event()" << endl;

   const int kMaxTrack = 2;
   const int kMaxShower = 10;
   const int cckMaxShower = 20;

   double ChisqVpi = 10000000.;
   double ChisqFpi = 10000000.;

   double ChisqVmu = 10000000.;
   double ChisqFmu = 10000000.;

   double ChisqVk = 10000000.;
   double ChisqFk = 10000000.;

   double ChisqVpi1 = 10000000.;
   double ChisqFpi1 = 10000000.;
   double ChisqVmu1 = 10000000.;
   double ChisqFmu1 = 10000000.;
   double ChisqVk1 = 10000000.;
   double ChisqFk1 = 10000000.;
   double TempChisqVpi1 = 10000000.;
   double TempChisqFpi1 = 10000000.;
   double TempChisqVmu1 = 10000000.;
   double TempChisqFmu1 = 10000000.;
   double TempChisqVk1 = 10000000.;
   double TempChisqFk1 = 10000000.;


   double E[kMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};
   double Theta[kMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};
   double Phi[kMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};
   double E925[kMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};

   double DIS2[kMaxTrack][kMaxShower] = {{-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.},{-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.}};
   double ME2[kMaxTrack][kMaxShower] = {{-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.},{-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.}};
   double CONME2[kMaxTrack][kMaxShower] = {{-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.},{-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.}};

   double CCE[cckMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};
   double CCTheta[cckMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};
   double CCPhi[cckMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};
   double CCE925[cckMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};
   double CCD1[cckMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};
   double CCD2[cckMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};

   double KN[kMaxTrack] = {0.,0.};

   double ME[kMaxTrack] = {-10.,-10.};
   double MME[kMaxTrack] = {-10.,-10.};

   double EOP[kMaxTrack] = {-10.,-10.};
   double MUDEPTH[kMaxTrack] = {0.,0.};

   double MEOP[kMaxTrack] = {-10.,-10.};
   double DISTANCE[kMaxTrack] = {-10.,-10.};

   double BEAME = -10.;
   double TRKM = -10.;
   double MMPI_GEN = -10.;

   double FE1 = -10.;
   double FTheta1 = -10.;
   double FPhi1 = -10.;

   double FE2 = -10.;
   double FTheta2 = -10.;
   double FPhi2 = -10.;

   double FE3 = -10.;
   double FTheta3 = -10.;
   double FPhi3 = -10.;

   double PIE = -10.;
   double PITheta = -10.;
   double PIPhi = -10.;

   double PIF1E[kMaxTrack] = {-10.,-10.};
   double PIF1Theta[kMaxTrack] = {-10.,-10.};
   double PIF1Phi[kMaxTrack] = {-10.,-10.};
   double MUF1E[kMaxTrack] = {-10.,-10.};
   double MUF1Theta[kMaxTrack] = {-10.,-10.};
   double MUF1Phi[kMaxTrack] = {-10.,-10.};
   double KF1E[kMaxTrack] = {-10.,-10.};
   double KF1Theta[kMaxTrack] = {-10.,-10.};
   double KF1Phi[kMaxTrack] = {-10.,-10.};

   double PE1[kMaxTrack] = {-10.,-10.};
   double PX1[kMaxTrack] = {-10.,-10.};
   double PY1[kMaxTrack] = {-10.,-10.};
   double PZ1[kMaxTrack] = {-10.,-10.};
   double PE2[kMaxTrack] = {-10.,-10.};
   double PX2[kMaxTrack] = {-10.,-10.};
   double PY2[kMaxTrack] = {-10.,-10.};
   double PZ2[kMaxTrack] = {-10.,-10.};
   double PE3[kMaxTrack] = {-10.,-10.};
   double PX3[kMaxTrack] = {-10.,-10.};
   double PY3[kMaxTrack] = {-10.,-10.};
   double PZ3[kMaxTrack] = {-10.,-10.};

   double PPISIGMA[kMaxTrack] = {-1000.,-1000.};
   double PMUSIGMA[kMaxTrack] = {-1000.,-1000.};
   double PKSIGMA[kMaxTrack] = {-1000.,-1000.};

   double PPILL[kMaxTrack] = {-1000.,-1000.};
   double PMULL[kMaxTrack] = {-1000.,-1000.};
   double PKLL[kMaxTrack] = {-1000.,-1000.};

   double FPE1[kMaxTrack] = {-10.,-10.};
   double FPX1[kMaxTrack] = {-10.,-10.};
   double FPY1[kMaxTrack] = {-10.,-10.};
   double FPZ1[kMaxTrack] = {-10.,-10.};
   double FPE2[kMaxTrack] = {-10.,-10.};
   double FPX2[kMaxTrack] = {-10.,-10.};
   double FPY2[kMaxTrack] = {-10.,-10.};
   double FPZ2[kMaxTrack] = {-10.,-10.};
   double FPE3[kMaxTrack] = {-10.,-10.};
   double FPX3[kMaxTrack] = {-10.,-10.};
   double FPY3[kMaxTrack] = {-10.,-10.};
   double FPZ3[kMaxTrack] = {-10.,-10.};

   double PIF1PE[kMaxTrack] = {-10.,-10.};
   double PIF1PX[kMaxTrack] = {-10.,-10.};
   double PIF1PY[kMaxTrack] = {-10.,-10.};
   double PIF1PZ[kMaxTrack] = {-10.,-10.};
   double MUF1PE[kMaxTrack] = {-10.,-10.};
   double MUF1PX[kMaxTrack] = {-10.,-10.};
   double MUF1PY[kMaxTrack] = {-10.,-10.};
   double MUF1PZ[kMaxTrack] = {-10.,-10.};
   double KF1PE[kMaxTrack] = {-10.,-10.};
   double KF1PX[kMaxTrack] = {-10.,-10.};
   double KF1PY[kMaxTrack] = {-10.,-10.};
   double KF1PZ[kMaxTrack] = {-10.,-10.};


   double CHARGE[kMaxTrack] = {0.,0.};   

   double L1TRIGGER1 = 0.;
   double L1TRIGGER2 = 0.;
   double L1TRIGGER3 = 0.;
   double L1TRIGGER4 = 0.;
   double L1TRIGGER5 = 0.;
   double L1TRIGGER6 = 0.;
   double L1TRIGGER7 = 0.;
   double L1TRIGGER8 = 0.;
   double L1TRIGGER9 = 0.;
   double L1TRIGGER10 = 0.;
   double L1TRIGGER11 = 0.;
   double L1TRIGGER12 = 0.;
   double L1TRIGGER13 = 0.;
   double L1TRIGGER14 = 0.;
   double L1TRIGGER15 = 0.;
   double L1TRIGGER16 = 0.;

   float tuple[kVarNum];



// if we're in MC, check to see if the trigger fired
   FAItem< DBEventHeader > eventHeader ;
   extract( iFrame.record( Stream::kEvent ), eventHeader ) ;

   double the_run = eventHeader->run();
   double the_evt = eventHeader->number();

//   if( eventHeader->monteCarlo() ) {
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

/* 
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
*/

      if(trigDat->isHadron()) {
        L1Trigger = 1.;
        L1TRIGGER1 = 1.;
      }
      if(trigDat->isRadTau()) {
        L1Trigger = 2.;
        L1TRIGGER2 = 1.;
      }
      if(trigDat->isElTrack()) {
        L1Trigger = 3.;
        L1TRIGGER3 = 1.;
      }
      if(trigDat->isBarrelBhabha()) {
        L1Trigger = 4.;
        L1TRIGGER4 = 1.;
      }
      if(trigDat->isEndcapBhabha()) {
        L1Trigger = 5.;
        L1TRIGGER5 = 1.;
      }
      if(trigDat->isMuPair()) {
        L1Trigger = 6.;
        L1TRIGGER6 = 1.;
      }
      if(trigDat->isCBSingleHi()) {
        L1Trigger = 7.;
        L1TRIGGER7 = 1.;
      }
      if(trigDat->isCESingleHi()) {
        L1Trigger = 8.;
        L1TRIGGER8 = 1.;
      }
      if(trigDat->isPulser()) {
        L1Trigger = 9.;
        L1TRIGGER9 = 1.;
      }
      if(trigDat->isRandom()) {
        L1Trigger = 10.;
        L1TRIGGER10 = 1.;
      }
      if(trigDat->isTwoTrack()) {
        L1Trigger = 11.;
        L1TRIGGER11 = 1.;
      }
      if(trigDat->isMinBias()) {
        L1Trigger = 12.;
        L1TRIGGER12 = 1.;
      }
      if(trigDat->isPhotonA()) {
        L1Trigger = 13.;
        L1TRIGGER13 = 1.;
      }
      if(trigDat->isPhotonB()) {
        L1Trigger = 14.;
        L1TRIGGER14 = 1.;
      }
      if(trigDat->isPhotonC()) {
        L1Trigger = 15.;
        L1TRIGGER15 = 1.;
      }
      if(trigDat->isPhotonD()) {
        L1Trigger = 16.;
        L1TRIGGER16 = 1.;
      }


 
      // L1 Accept if any non-prescaled trigger line fires
//   if ( (trigDat->isHadron())       ||
//	(trigDat->isMuPair())       ||
//	(trigDat->isElTrack())      ||
//	(trigDat->isRadTau())       ||
//	(trigDat->isBarrelBhabha())  )
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

//   }



HepLorentzVector pip4v, pim4v;
if( eventHeader->monteCarlo() ) {
        FAItem< MCDecayTree > decayTree;
        extract(iFrame.record(Stream::kEvent), decayTree, "Generator" );
        const MCDecayTree& mcDecayTree(*decayTree);
        // report(INFO, kFacilityString) << mcDecayTree << endl;

        int ngam = 0;
        for(MCDecayTree::const_pIterator it = decayTree->pBegin();
            it != decayTree->pEnd(); it++) {

                if( it->properties().PDGId() == 211 )
                        pip4v = it->lorentzMomentum(); 
                if( it->properties().PDGId() == -211 )
                        pim4v = it->lorentzMomentum(); 

        }


        MMPI_GEN = (pip4v + pim4v).m();
}








   FATable< NavTrack > trackTable;
   extract( iFrame.record( Stream::kEvent ) , trackTable, "GoodThings"  );

   FATable< NavTrack >::const_iterator trackBegin = trackTable.begin();
   FATable< NavTrack >::const_iterator trackEnd = trackTable.end();

   if(trackTable.size()!=2)
   //if (trackEnd != trackBegin+2)
      return ActionBase::kFailed;

   FAItem<TDKinematicFit> pi1 = (*trackBegin).pionFit();
   FAItem<TDKinematicFit> pi2 = (*(trackBegin+1)).pionFit();
   FAItem<TDKinematicFit> mu1 = (*trackBegin).muonFit();
   FAItem<TDKinematicFit> mu2 = (*(trackBegin+1)).muonFit();
   FAItem<TDKinematicFit> k1 = (*trackBegin).kaonFit();
   FAItem<TDKinematicFit> k2 = (*(trackBegin+1)).kaonFit();

   if(!pi1.valid() || !pi2.valid())
      return ActionBase::kFailed;

   CHARGE[0] = (*pi1).charge();
   CHARGE[1] = (*pi2).charge();

   if(CHARGE[0]!=-CHARGE[1])
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

      if(j<kMaxTrack){

      if(trackItr->trackShowerMatch().valid()) {
         ME[j] = trackItr->trackShowerMatch()->matchedEnergy();
         KN[j] = trackItr->trackShowerMatch()->navShowers().size();
         MEOP[j] = trackItr->trackShowerMatch()->eOverP();
         DISTANCE[j] = trackItr->trackShowerMatch()->distance();

         int unmatchedmatchSh = 0;
         for(FATable< NavShower >::const_iterator matchShItr = trackItr->trackShowerMatch()->navShowers().begin();
                          matchShItr != trackItr->trackShowerMatch()->navShowers().end(); matchShItr++) {

            int k = unmatchedmatchSh++;

            if(k<kMaxShower){

             DIS2[j][k] = matchShItr->distance(*trackItr);
             ME2[j][k] = matchShItr->attributes().energy();
             CONME2[j][k] =  matchShItr->conReg().energy();

                      }
         }
      }

      EOP[j] = trackItr->elecId().eOverP();
      MUDEPTH[j] = trackItr->muonId().depth();
      if(EOP[j]>0.8)
        return ActionBase::kFailed;

      if( (pionFit.valid()) && (the_dedx.valid()) )  {

      PE1[j]=pionFit->lorentzMomentum().e();
      PX1[j]=pionFit->px();
      PY1[j]=pionFit->py();
      PZ1[j]=pionFit->pz();
      PPISIGMA[j]=the_dedx->piSigma();

      if ((the_rich.valid()) && (fabs(cos_theta1) < 0.8) && (the_rich->pionHypWasAnalyzed()))
      PPILL[j]=the_rich->pionLogLikelihood();
      }

      if( (muonFit.valid()) && (the_dedx.valid()) )  {

      PE2[j]=muonFit->lorentzMomentum().e();
      PX2[j]=muonFit->px();
      PY2[j]=muonFit->py();
      PZ2[j]=muonFit->pz();
      PMUSIGMA[j]=the_dedx->muSigma();

      if ((the_rich.valid()) && (fabs(cos_theta2) < 0.8) && (the_rich->muonHypWasAnalyzed()))
      PMULL[j]=the_rich->muonLogLikelihood();
      }

      if( (kaonFit.valid()) && (the_dedx.valid()) )  {

      PE3[j]=kaonFit->lorentzMomentum().e();
      PX3[j]=kaonFit->px();
      PY3[j]=kaonFit->py();
      PZ3[j]=kaonFit->pz();
      PKSIGMA[j]=the_dedx->kSigma();

      if ((the_rich.valid()) && (fabs(cos_theta3) < 0.8) && (the_rich->kaonHypWasAnalyzed()))
      PKLL[j]=the_rich->kaonLogLikelihood();
      }

   }
   }



   FATable< NavShower > showerTable;
   extract( iFrame.record( Stream::kEvent ) , showerTable, "GoodThings" );

   if(showerTable.size()==0)
      return ActionBase::kFailed;

   FATable< NavShower >::const_iterator showerBegin = showerTable.begin();
   FATable< NavShower >::const_iterator showerEnd = showerTable.end();

   FATable< NavShower >::const_iterator fastest_shower = showerBegin;

   if(fastest_shower->attributes().x925() < 1)   
      return ActionBase::kFailed;
   if(fastest_shower->attributes().energy() < 0.5)   
      return ActionBase::kFailed;


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

   FATable< NavShower > ccshowerTable;
   extract( iFrame.record( Stream::kEvent ) , ccshowerTable );


   FATable< NavShower >::const_iterator ccshowerBegin = ccshowerTable.begin();
   FATable< NavShower >::const_iterator ccshowerEnd = ccshowerTable.end();

   int ccunmatchedShowers = 0;

   for ( FATable< NavShower >::const_iterator ccshowerItr = ccshowerBegin;
         ccshowerItr != ccshowerEnd ;
         ++ccshowerItr )
   {

      int j = ccunmatchedShowers++;

      if(j<cckMaxShower){

        CCE[j] = ccshowerItr->attributes().energy();
        CCTheta[j] = ccshowerItr->attributes().theta();
        CCPhi[j] = ccshowerItr->attributes().phi();
        CCE925[j] = ccshowerItr->attributes().x925();
        CCD1[j] = ccshowerItr->distance(*trackBegin);
        CCD2[j] = ccshowerItr->distance(*(trackBegin+1));

      }
   }

///////////////////////////////////////////////////////////////////////////////////////////////

NavShowerServer *navsh_server = new NavShowerServer( iFrame.record(Stream::kEvent ) ) ;

//CcBasicShowerAttsArg basic_atts (fastest_shower->attributes().basicShowerAtts() );
CcFortranShowerCorrector *sh_corr = new CcFortranShowerCorrector ;

const CcShowerAttributes &atts = fastest_shower->attributes();

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


///////////////////////////////////////////////////////////////// 4C fit ///////////////////////

   // do the pion fit
   FitEvt pipi( "Pipi", FitEvt::k_P4VecCM );
   pipi.newPhoton(*tmp_sh);
   pipi.newTrack(*pi1);
   pipi.newTrack(*pi2);
   pipi.doTheFit();

   if( (pipi.chisqVtx()>0) && (pipi.chisqFit()>0) ) {
   // fit converged  -  save this event

      const HepLorentzVector vgpi (pipi.kdFitVec()[0]->lorentzMomentum());
      const HepLorentzVector v1pi (pipi.kdFitVec()[1]->lorentzMomentum());
      const HepLorentzVector v2pi (pipi.kdFitVec()[2]->lorentzMomentum());


      ChisqVpi = pipi.chisqVtx();
      ChisqFpi = pipi.chisqFit();

      FE1 = vgpi.e();
      FTheta1 = vgpi.theta();
      FPhi1 = vgpi.phi();

      FPE1[0]=v1pi.e();
      FPX1[0]=v1pi.px();
      FPY1[0]=v1pi.py();
      FPZ1[0]=v1pi.pz();

      FPE1[1]=v2pi.e();
      FPX1[1]=v2pi.px();
      FPY1[1]=v2pi.py();
      FPZ1[1]=v2pi.pz();
   }

   // do the muon fit
   FitEvt mumu( "Mumu", FitEvt::k_P4VecCM );
   mumu.newPhoton(*tmp_sh);
   mumu.newTrack(*mu1);
   mumu.newTrack(*mu2);
   mumu.doTheFit();

   if( (mumu.chisqVtx()>0) && (mumu.chisqFit()>0) ) {
   // fit converged  -  save this event

      const HepLorentzVector vgmu (mumu.kdFitVec()[0]->lorentzMomentum());
      const HepLorentzVector v1mu (mumu.kdFitVec()[1]->lorentzMomentum());
      const HepLorentzVector v2mu (mumu.kdFitVec()[2]->lorentzMomentum());


      ChisqVmu = mumu.chisqVtx();
      ChisqFmu = mumu.chisqFit();

      FE2 = vgmu.e();
      FTheta2 = vgmu.theta();
      FPhi2 = vgmu.phi();

      FPE2[0]=v1mu.e();
      FPX2[0]=v1mu.px();
      FPY2[0]=v1mu.py();
      FPZ2[0]=v1mu.pz();

      FPE2[1]=v2mu.e();
      FPX2[1]=v2mu.px();
      FPY2[1]=v2mu.py();
      FPZ2[1]=v2mu.pz();
   }

   // do the kaon fit
   FitEvt kk( "Kk", FitEvt::k_P4VecCM );
   kk.newPhoton(*tmp_sh);
   kk.newTrack(*k1);
   kk.newTrack(*k2);
   kk.doTheFit();

   if( (kk.chisqVtx()>0) && (kk.chisqFit()>0) ) {
   // fit converged  -  save this event

      const HepLorentzVector vgk (kk.kdFitVec()[0]->lorentzMomentum());
      const HepLorentzVector v1k (kk.kdFitVec()[1]->lorentzMomentum());
      const HepLorentzVector v2k (kk.kdFitVec()[2]->lorentzMomentum());


      ChisqVk = kk.chisqVtx();
      ChisqFk = kk.chisqFit();

      FE3 = vgk.e();
      FTheta3 = vgk.theta();
      FPhi3 = vgk.phi();

      FPE3[0]=v1k.e();
      FPX3[0]=v1k.px();
      FPY3[0]=v1k.py();
      FPZ3[0]=v1k.pz();

      FPE3[1]=v2k.e();
      FPX3[1]=v2k.px();
      FPY3[1]=v2k.py();
      FPZ3[1]=v2k.pz();
   }

////////////////////////////////////////////// add.FSR fit ///////////////////////////////////////////

if(showerTable.size() > 1){

   for ( FATable< NavShower >::const_iterator showerItr = showerBegin+1;
         showerItr != showerEnd ;
         ++showerItr ) {

   if(showerItr->attributes().x925() < 1)  continue;
   if(showerItr->attributes().energy() < 0.03) continue;

   // do the pion fit

   FitEvt pipi1( "Pipi1", FitEvt::k_P4VecCM );
   pipi1.newPhoton(*tmp_sh);
   pipi1.newPhoton(*showerItr);
   pipi1.newTrack(*pi1);
   pipi1.newTrack(*pi2);
   pipi1.doTheFit();

   if( (pipi1.chisqVtx()>0) && (pipi1.chisqFit()>0) ) {
   // fit converged  -  save this event
      HepLorentzVector vgpi (pipi1.kdFitVec()[0]->lorentzMomentum());
      HepLorentzVector v0pi (pipi1.kdFitVec()[1]->lorentzMomentum());
      HepLorentzVector v1pi (pipi1.kdFitVec()[2]->lorentzMomentum());
      HepLorentzVector v2pi (pipi1.kdFitVec()[3]->lorentzMomentum());

      TempChisqVpi1 = pipi1.chisqVtx();
      TempChisqFpi1 = pipi1.chisqFit();

      if(TempChisqFpi1 < ChisqFpi1){

        ChisqVpi1 = pipi1.chisqVtx();
        ChisqFpi1 = pipi1.chisqFit();

        PIE = showerItr->attributes().energy();
        PITheta = showerItr->attributes().theta();
        PIPhi = showerItr->attributes().phi();

        PIF1E[0] = vgpi.e();
        PIF1Theta[0] = vgpi.theta();
        PIF1Phi[0] = vgpi.phi();

        PIF1E[1] = v0pi.e();
        PIF1Theta[1] = v0pi.theta();
        PIF1Phi[1] = v0pi.phi();

        PIF1PE[0]=v1pi.e();
        PIF1PX[0]=v1pi.px();
        PIF1PY[0]=v1pi.py();
        PIF1PZ[0]=v1pi.pz();
        PIF1PE[1]=v2pi.e();
        PIF1PX[1]=v2pi.px();
        PIF1PY[1]=v2pi.py();
        PIF1PZ[1]=v2pi.pz();
      }
   }

   // do the muon fit

   FitEvt mumu1( "Mumu1", FitEvt::k_P4VecCM );
   mumu1.newPhoton(*tmp_sh);
   mumu1.newPhoton(*showerItr);
   mumu1.newTrack(*mu1);
   mumu1.newTrack(*mu2);
   mumu1.doTheFit();


   if( (mumu1.chisqVtx()>0) && (mumu1.chisqFit()>0) ) {
   // fit converged  -  save this event

      HepLorentzVector vgmu (mumu1.kdFitVec()[0]->lorentzMomentum());
      HepLorentzVector v0mu (mumu1.kdFitVec()[1]->lorentzMomentum());
      HepLorentzVector v1mu (mumu1.kdFitVec()[2]->lorentzMomentum());
      HepLorentzVector v2mu (mumu1.kdFitVec()[3]->lorentzMomentum());

      TempChisqVmu1 = mumu1.chisqVtx();
      TempChisqFmu1 = mumu1.chisqFit();

      if(TempChisqFmu1 < ChisqFmu1){
        ChisqVmu1 = mumu1.chisqVtx();
        ChisqFmu1 = mumu1.chisqFit();

        MUF1E[0] = vgmu.e();
        MUF1Theta[0] = vgmu.theta();
        MUF1Phi[0] = vgmu.phi();

        MUF1E[1] = v0mu.e();
        MUF1Theta[1] = v0mu.theta();
        MUF1Phi[1] = v0mu.phi();

        MUF1PE[0]=v1mu.e();
        MUF1PX[0]=v1mu.px();
        MUF1PY[0]=v1mu.py();
        MUF1PZ[0]=v1mu.pz();

        MUF1PE[1]=v2mu.e();
        MUF1PX[1]=v2mu.px();
        MUF1PY[1]=v2mu.py();
        MUF1PZ[1]=v2mu.pz();
     }
   }

   // do the kaon fit

   FitEvt kk1( "Kk1", FitEvt::k_P4VecCM );
   kk1.newPhoton(*tmp_sh);
   kk1.newPhoton(*showerItr);
   kk1.newTrack(*k1);
   kk1.newTrack(*k2);
   kk1.doTheFit();

   if( (kk1.chisqVtx()>0) && (kk1.chisqFit()>0) ) {
   // fit converged  -  save this event

      HepLorentzVector vgk (kk1.kdFitVec()[0]->lorentzMomentum());
      HepLorentzVector v0k (kk1.kdFitVec()[1]->lorentzMomentum());
      HepLorentzVector v1k (kk1.kdFitVec()[2]->lorentzMomentum());
      HepLorentzVector v2k (kk1.kdFitVec()[3]->lorentzMomentum());

      TempChisqVk1 = kk1.chisqVtx();
      TempChisqFk1 = kk1.chisqFit();

      if(TempChisqFk1 < ChisqFk1){
        ChisqVk1 = kk1.chisqVtx();
        ChisqFk1 = kk1.chisqFit();

        KF1E[0] = vgk.e();
        KF1Theta[0] = vgk.theta();
        KF1Phi[0] = vgk.phi();
 
        KF1E[1] = v0k.e();
        KF1Theta[1] = v0k.theta();
        KF1Phi[1] = v0k.phi();

        KF1PE[0]=v1k.e();
        KF1PX[0]=v1k.px();
        KF1PY[0]=v1k.py();
        KF1PZ[0]=v1k.pz();

        KF1PE[1]=v2k.e();
        KF1PX[1]=v2k.px();
        KF1PY[1]=v2k.py();
        KF1PZ[1]=v2k.pz();
      }
   }
  }
}


   FAItem< BeamEnergy > beam_energy;
   extract( iFrame.record( Stream::kBeginRun ), beam_energy );
   BEAME = beam_energy->value();
   double s = pow((2*BEAME),2);
   double pppn = sqrt(pow((PX1[0]+PX1[1]),2)+pow((PY1[0]+PY1[1]),2)+pow((PZ1[0]+PZ1[1]),2));
   double pp2 = pow(PX1[0],2)+pow(PY1[0],2)+pow(PZ1[0],2);
   double pn2 = pow(PX1[1],2)+pow(PY1[1],2)+pow(PZ1[1],2);
   double mm = (pow((pow((sqrt(s)-pppn),2)-pp2-pn2),2)-4*pp2*pn2)/(4*pow((sqrt(s)-pppn),2));
//   if(PIPX[0]<1000 && PIPY[0]<1000 && PIPZ[0] < 1000 && PIPX[1]<1000 && PIPY[1]<1000 && PIPZ[1] < 1000)
   TRKM = sqrt(mm);   



   tuple[knshower] = unmatchedShowers;
   tuple[ccknshower] = ccunmatchedShowers;

   tuple[chisqvpi] = ChisqVpi;
   tuple[chisqfpi] = ChisqFpi;
   tuple[chisqvmu] = ChisqVmu;
   tuple[chisqfmu] = ChisqFmu;
   tuple[chisqvk] = ChisqVk;
   tuple[chisqfk] = ChisqFk;

   tuple[chisqvpi1] = ChisqVpi1;
   tuple[chisqfpi1] = ChisqFpi1;

   tuple[chisqvmu1] = ChisqVmu1;
   tuple[chisqfmu1] = ChisqFmu1;

   tuple[chisqvk1] = ChisqVk1;
   tuple[chisqfk1] = ChisqFk1;


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

   tuple[cce11] = CCE[10];
   tuple[ccth11] = CCTheta[10];
   tuple[ccphi11] = CCPhi[10];
   tuple[cce92511] = CCE925[10];

   tuple[cce12] = CCE[11];
   tuple[ccth12] = CCTheta[11];
   tuple[ccphi12] = CCPhi[11];
   tuple[cce92512] = CCE925[11];

   tuple[cce13] = CCE[12];
   tuple[ccth13] = CCTheta[12];
   tuple[ccphi13] = CCPhi[12];
   tuple[cce92513] = CCE925[12];

   tuple[cce14] = CCE[13];
   tuple[ccth14] = CCTheta[13];
   tuple[ccphi14] = CCPhi[13];
   tuple[cce92514] = CCE925[13];

   tuple[cce15] = CCE[14];
   tuple[ccth15] = CCTheta[14];
   tuple[ccphi15] = CCPhi[14];
   tuple[cce92515] = CCE925[14];

   tuple[cce16] = CCE[15];
   tuple[ccth16] = CCTheta[15];
   tuple[ccphi16] = CCPhi[15];
   tuple[cce92516] = CCE925[15];

   tuple[cce17] = CCE[16];
   tuple[ccth17] = CCTheta[16];
   tuple[ccphi17] = CCPhi[16];
   tuple[cce92517] = CCE925[16];

   tuple[cce18] = CCE[17];
   tuple[ccth18] = CCTheta[17];
   tuple[ccphi18] = CCPhi[17];
   tuple[cce92518] = CCE925[17];

   tuple[cce19] = CCE[18];
   tuple[ccth19] = CCTheta[18];
   tuple[ccphi19] = CCPhi[18];
   tuple[cce92519] = CCE925[18];

   tuple[cce20] = CCE[19];
   tuple[ccth20] = CCTheta[19];
   tuple[ccphi20] = CCPhi[19];
   tuple[cce92520] = CCE925[19];

   tuple[cce1] = CCE[0];
   tuple[ccth1] = CCTheta[0];
   tuple[ccphi1] = CCPhi[0];
   tuple[cce9251] = CCE925[0];

   tuple[cce2] = CCE[1];
   tuple[ccth2] = CCTheta[1];
   tuple[ccphi2] = CCPhi[1];
   tuple[cce9252] = CCE925[1];

   tuple[cce3] = CCE[2];
   tuple[ccth3] = CCTheta[2];
   tuple[ccphi3] = CCPhi[2];
   tuple[cce9253] = CCE925[2];

   tuple[cce4] = CCE[3];
   tuple[ccth4] = CCTheta[3];
   tuple[ccphi4] = CCPhi[3];
   tuple[cce9254] = CCE925[3];

   tuple[cce5] = CCE[4];
   tuple[ccth5] = CCTheta[4];
   tuple[ccphi5] = CCPhi[4];
   tuple[cce9255] = CCE925[4];

   tuple[cce6] = CCE[5];
   tuple[ccth6] = CCTheta[5];
   tuple[ccphi6] = CCPhi[5];
   tuple[cce9256] = CCE925[5];

   tuple[cce7] = CCE[6];
   tuple[ccth7] = CCTheta[6];
   tuple[ccphi7] = CCPhi[6];
   tuple[cce9257] = CCE925[6];

   tuple[cce8] = CCE[7];
   tuple[ccth8] = CCTheta[7];
   tuple[ccphi8] = CCPhi[7];
   tuple[cce9258] = CCE925[7];

   tuple[cce9] = CCE[8];
   tuple[ccth9] = CCTheta[8];
   tuple[ccphi9] = CCPhi[8];
   tuple[cce9259] = CCE925[8];

   tuple[cce10] = CCE[9];
   tuple[ccth10] = CCTheta[9];
   tuple[ccphi10] = CCPhi[9];
   tuple[cce92510] = CCE925[9];

   tuple[epi2] = PIE;
   tuple[thpi2] = PITheta;
   tuple[phipi2] = PIPhi;

   tuple[fepi] = FE1;
   tuple[fthpi] = FTheta1;
   tuple[fphipi] = FPhi1;
   tuple[femu] = FE2;
   tuple[fthmu] = FTheta2;
   tuple[fphimu] = FPhi2;
   tuple[fek] = FE3;
   tuple[fthk] = FTheta3;
   tuple[fphik] = FPhi3;

   tuple[f1epi1] = PIF1E[0];
   tuple[f1thpi1] = PIF1Theta[0];
   tuple[f1phipi1] = PIF1Phi[0];
   tuple[f1epi2] = PIF1E[1];
   tuple[f1thpi2] = PIF1Theta[1];
   tuple[f1phipi2] = PIF1Phi[1];
   tuple[f1emu1] = MUF1E[0];
   tuple[f1thmu1] = MUF1Theta[0];
   tuple[f1phimu1] = MUF1Phi[0];
   tuple[f1emu2] = MUF1E[1];
   tuple[f1thmu2] = MUF1Theta[1];
   tuple[f1phimu2] = MUF1Phi[1];
   tuple[f1ek1] = KF1E[0];
   tuple[f1thk1] = KF1Theta[0];
   tuple[f1phik1] = KF1Phi[0];
   tuple[f1ek2] = KF1E[1];
   tuple[f1thk2] = KF1Theta[1];
   tuple[f1phik2] = KF1Phi[1];


   tuple[l1trigger1] = L1TRIGGER1;
   tuple[l1trigger2] = L1TRIGGER2;
   tuple[l1trigger3] = L1TRIGGER3;
   tuple[l1trigger4] = L1TRIGGER4;
   tuple[l1trigger5] = L1TRIGGER5;
   tuple[l1trigger6] = L1TRIGGER6;
   tuple[l1trigger7] = L1TRIGGER7;
   tuple[l1trigger8] = L1TRIGGER8;
   tuple[l1trigger9] = L1TRIGGER9;
   tuple[l1trigger10] = L1TRIGGER10;
   tuple[l1trigger11] = L1TRIGGER11;
   tuple[l1trigger12] = L1TRIGGER12;
   tuple[l1trigger13] = L1TRIGGER13;
   tuple[l1trigger14] = L1TRIGGER14;
   tuple[l1trigger15] = L1TRIGGER15;
   tuple[l1trigger16] = L1TRIGGER16;

  if(CHARGE[0]==1 && CHARGE[1]==-1){

   tuple[kntrack1shower] = KN[0];
   tuple[kntrack2shower] = KN[1];

   tuple[ccd1p] = CCD1[0];
   tuple[ccd2p] = CCD1[1];
   tuple[ccd3p] = CCD1[2];
   tuple[ccd4p] = CCD1[3];
   tuple[ccd5p] = CCD1[4];
   tuple[ccd6p] = CCD1[5];
   tuple[ccd7p] = CCD1[6];
   tuple[ccd8p] = CCD1[7];
   tuple[ccd9p] = CCD1[8];
   tuple[ccd10p] = CCD1[9];
   tuple[ccd11p] = CCD1[10];
   tuple[ccd12p] = CCD1[11];
   tuple[ccd13p] = CCD1[12];
   tuple[ccd14p] = CCD1[13];
   tuple[ccd15p] = CCD1[14];
   tuple[ccd16p] = CCD1[15];
   tuple[ccd17p] = CCD1[16];
   tuple[ccd18p] = CCD1[17];
   tuple[ccd19p] = CCD1[18];
   tuple[ccd20p] = CCD1[19];

   tuple[ccd1n] = CCD2[0];
   tuple[ccd2n] = CCD2[1];
   tuple[ccd3n] = CCD2[2];
   tuple[ccd4n] = CCD2[3];
   tuple[ccd5n] = CCD2[4];
   tuple[ccd6n] = CCD2[5];
   tuple[ccd7n] = CCD2[6];
   tuple[ccd8n] = CCD2[7];
   tuple[ccd9n] = CCD2[8];
   tuple[ccd10n] = CCD2[9];
   tuple[ccd11n] = CCD2[10];
   tuple[ccd12n] = CCD2[11];
   tuple[ccd13n] = CCD2[12];
   tuple[ccd14n] = CCD2[13];
   tuple[ccd15n] = CCD2[14];
   tuple[ccd16n] = CCD2[15];
   tuple[ccd17n] = CCD2[16];
   tuple[ccd18n] = CCD2[17];
   tuple[ccd19n] = CCD2[18];
   tuple[ccd20n] = CCD2[19];


   tuple[me1] = ME[0];
   tuple[mme1] = MME[0];
   tuple[eop1] = EOP[0];
   tuple[me2] = ME[1];
   tuple[mme2] = MME[1];
   tuple[eop2] = EOP[1];
   tuple[mudepth1] = MUDEPTH[0];
   tuple[mudepth2] = MUDEPTH[1];
   tuple[meop1] = MEOP[0];
   tuple[meop2] = MEOP[1];
   tuple[dis1] = DISTANCE[0];
   tuple[dis2] = DISTANCE[1];

   tuple[dis101] = DIS2[0][0];
   tuple[dis102] = DIS2[0][1];
   tuple[dis103] = DIS2[0][2];
   tuple[dis104] = DIS2[0][3];
   tuple[dis105] = DIS2[0][4];
   tuple[dis106] = DIS2[0][5];
   tuple[dis107] = DIS2[0][6];
   tuple[dis108] = DIS2[0][7];
   tuple[dis109] = DIS2[0][8];
   tuple[dis110] = DIS2[0][9];
   tuple[dis201] = DIS2[1][0];
   tuple[dis202] = DIS2[1][1];
   tuple[dis203] = DIS2[1][2];
   tuple[dis204] = DIS2[1][3];
   tuple[dis205] = DIS2[1][4];
   tuple[dis206] = DIS2[1][5];
   tuple[dis207] = DIS2[1][6];
   tuple[dis208] = DIS2[1][7];
   tuple[dis209] = DIS2[1][8];
   tuple[dis210] = DIS2[1][9];

   tuple[me101] = ME2[0][0];
   tuple[me102] = ME2[0][1];
   tuple[me103] = ME2[0][2];
   tuple[me104] = ME2[0][3];
   tuple[me105] = ME2[0][4];
   tuple[me106] = ME2[0][5];
   tuple[me107] = ME2[0][6];
   tuple[me108] = ME2[0][7];
   tuple[me109] = ME2[0][8];
   tuple[me110] = ME2[0][9];
   tuple[me201] = ME2[1][0];
   tuple[me202] = ME2[1][1];
   tuple[me203] = ME2[1][2];
   tuple[me204] = ME2[1][3];
   tuple[me205] = ME2[1][4];
   tuple[me206] = ME2[1][5];
   tuple[me207] = ME2[1][6];
   tuple[me208] = ME2[1][7];
   tuple[me209] = ME2[1][8];
   tuple[me210] = ME2[1][9];

   tuple[conme101] = CONME2[0][0];
   tuple[conme102] = CONME2[0][1];
   tuple[conme103] = CONME2[0][2];
   tuple[conme104] = CONME2[0][3];
   tuple[conme105] = CONME2[0][4];
   tuple[conme106] = CONME2[0][5];
   tuple[conme107] = CONME2[0][6];
   tuple[conme108] = CONME2[0][7];
   tuple[conme109] = CONME2[0][8];
   tuple[conme110] = CONME2[0][9];
   tuple[conme201] = CONME2[1][0];
   tuple[conme202] = CONME2[1][1];
   tuple[conme203] = CONME2[1][2];
   tuple[conme204] = CONME2[1][3];
   tuple[conme205] = CONME2[1][4];
   tuple[conme206] = CONME2[1][5];
   tuple[conme207] = CONME2[1][6];
   tuple[conme208] = CONME2[1][7];
   tuple[conme209] = CONME2[1][8];
   tuple[conme210] = CONME2[1][9];


   tuple[pite1] = PE1[0];
   tuple[pipx1] = PX1[0];
   tuple[pipy1] = PY1[0];
   tuple[pipz1] = PZ1[0];
   tuple[mute1] = PE2[0];
   tuple[mupx1] = PX2[0];
   tuple[mupy1] = PY2[0];
   tuple[mupz1] = PZ2[0];
   tuple[kte1] = PE3[0];
   tuple[kpx1] = PX3[0];
   tuple[kpy1] = PY3[0];
   tuple[kpz1] = PZ3[0];

   tuple[pisigma1] = PPISIGMA[0];
   tuple[musigma1] = PMUSIGMA[0];
   tuple[ksigma1] = PKSIGMA[0];

   tuple[pill1] = PPILL[0];
   tuple[mull1] = PMULL[0];
   tuple[kll1] = PKLL[0];

   tuple[pite2] = PE1[1];
   tuple[pipx2] = PX1[1];
   tuple[pipy2] = PY1[1];
   tuple[pipz2] = PZ1[1];
   tuple[mute2] = PE2[1];
   tuple[mupx2] = PX2[1];
   tuple[mupy2] = PY2[1];
   tuple[mupz2] = PZ2[1];
   tuple[kte2] = PE3[1];
   tuple[kpx2] = PX3[1];
   tuple[kpy2] = PY3[1];
   tuple[kpz2] = PZ3[1];

   tuple[pisigma2] = PPISIGMA[1];
   tuple[musigma2] = PMUSIGMA[1];
   tuple[ksigma2] = PKSIGMA[1];

   tuple[pill2] = PPILL[1];
   tuple[mull2] = PMULL[1];
   tuple[kll2] = PKLL[1];


   tuple[fpite1] = FPE1[0];
   tuple[fpipx1] = FPX1[0];
   tuple[fpipy1] = FPY1[0];
   tuple[fpipz1] = FPZ1[0];
   tuple[fmute1] = FPE2[0];
   tuple[fmupx1] = FPX2[0];
   tuple[fmupy1] = FPY2[0];
   tuple[fmupz1] = FPZ2[0];
   tuple[fkte1] = FPE3[0];
   tuple[fkpx1] = FPX3[0];
   tuple[fkpy1] = FPY3[0];
   tuple[fkpz1] = FPZ3[0];

   tuple[fpite2] = FPE1[1];
   tuple[fpipx2] = FPX1[1];
   tuple[fpipy2] = FPY1[1];
   tuple[fpipz2] = FPZ1[1];
   tuple[fmute2] = FPE2[1];
   tuple[fmupx2] = FPX2[1];
   tuple[fmupy2] = FPY2[1];
   tuple[fmupz2] = FPZ2[1];
   tuple[fkte2] = FPE3[1];
   tuple[fkpx2] = FPX3[1];
   tuple[fkpy2] = FPY3[1];
   tuple[fkpz2] = FPZ3[1];

   tuple[f1pite1] = PIF1PE[0];
   tuple[f1pipx1] = PIF1PX[0];
   tuple[f1pipy1] = PIF1PY[0];
   tuple[f1pipz1] = PIF1PZ[0];
   tuple[f1pite2] = PIF1PE[1];
   tuple[f1pipx2] = PIF1PX[1];
   tuple[f1pipy2] = PIF1PY[1];
   tuple[f1pipz2] = PIF1PZ[1];
   tuple[f1mute1] = MUF1PE[0];
   tuple[f1mupx1] = MUF1PX[0];
   tuple[f1mupy1] = MUF1PY[0];
   tuple[f1mupz1] = MUF1PZ[0];
   tuple[f1mute2] = MUF1PE[1];
   tuple[f1mupx2] = MUF1PX[1];
   tuple[f1mupy2] = MUF1PY[1];
   tuple[f1mupz2] = MUF1PZ[1];
   tuple[f1kte1] = KF1PE[0];
   tuple[f1kpx1] = KF1PX[0];
   tuple[f1kpy1] = KF1PY[0];
   tuple[f1kpz1] = KF1PZ[0];
   tuple[f1kte2] = KF1PE[1];
   tuple[f1kpx2] = KF1PX[1];
   tuple[f1kpy2] = KF1PY[1];
   tuple[f1kpz2] = KF1PZ[1];


   tuple[charge1] = CHARGE[0];
   tuple[charge2] = CHARGE[1];

  }

   else if(CHARGE[1]==1 && CHARGE[0]==-1){

   tuple[kntrack1shower] = KN[1];
   tuple[kntrack2shower] = KN[0];

   tuple[ccd1p] = CCD2[0];
   tuple[ccd2p] = CCD2[1];
   tuple[ccd3p] = CCD2[2];
   tuple[ccd4p] = CCD2[3];
   tuple[ccd5p] = CCD2[4];
   tuple[ccd6p] = CCD2[5];
   tuple[ccd7p] = CCD2[6];
   tuple[ccd8p] = CCD2[7];
   tuple[ccd9p] = CCD2[8];
   tuple[ccd10p] = CCD2[9];
   tuple[ccd11p] = CCD2[10];
   tuple[ccd12p] = CCD2[11];
   tuple[ccd13p] = CCD2[12];
   tuple[ccd14p] = CCD2[13];
   tuple[ccd15p] = CCD2[14];
   tuple[ccd16p] = CCD2[15];
   tuple[ccd17p] = CCD2[16];
   tuple[ccd18p] = CCD2[17];
   tuple[ccd19p] = CCD2[18];
   tuple[ccd20p] = CCD2[19];

   tuple[ccd1n] = CCD1[0];
   tuple[ccd2n] = CCD1[1];
   tuple[ccd3n] = CCD1[2];
   tuple[ccd4n] = CCD1[3];
   tuple[ccd5n] = CCD1[4];
   tuple[ccd6n] = CCD1[5];
   tuple[ccd7n] = CCD1[6];
   tuple[ccd8n] = CCD1[7];
   tuple[ccd9n] = CCD1[8];
   tuple[ccd10n] = CCD1[9];
   tuple[ccd11n] = CCD1[10];
   tuple[ccd12n] = CCD1[11];
   tuple[ccd13n] = CCD1[12];
   tuple[ccd14n] = CCD1[13];
   tuple[ccd15n] = CCD1[14];
   tuple[ccd16n] = CCD1[15];
   tuple[ccd17n] = CCD1[16];
   tuple[ccd18n] = CCD1[17];
   tuple[ccd19n] = CCD1[18];
   tuple[ccd20n] = CCD1[19];


   tuple[me1] = ME[1];
   tuple[mme1] = MME[1];
   tuple[eop1] = EOP[1];
   tuple[me2] = ME[0];
   tuple[mme2] = MME[0];
   tuple[eop2] = EOP[0];
   tuple[mudepth1] = MUDEPTH[1];
   tuple[mudepth2] = MUDEPTH[0];
   tuple[meop1] = MEOP[1];
   tuple[meop2] = MEOP[0];
   tuple[dis1] = DISTANCE[1];
   tuple[dis2] = DISTANCE[0];

   tuple[dis101] = DIS2[1][0];
   tuple[dis102] = DIS2[1][1];
   tuple[dis103] = DIS2[1][2];
   tuple[dis104] = DIS2[1][3];
   tuple[dis105] = DIS2[1][4];
   tuple[dis106] = DIS2[1][5];
   tuple[dis107] = DIS2[1][6];
   tuple[dis108] = DIS2[1][7];
   tuple[dis109] = DIS2[1][8];
   tuple[dis110] = DIS2[1][9];
   tuple[dis201] = DIS2[0][0];
   tuple[dis202] = DIS2[0][1];
   tuple[dis203] = DIS2[0][2];
   tuple[dis204] = DIS2[0][3];
   tuple[dis205] = DIS2[0][4];
   tuple[dis206] = DIS2[0][5];
   tuple[dis207] = DIS2[0][6];
   tuple[dis208] = DIS2[0][7];
   tuple[dis209] = DIS2[0][8];
   tuple[dis210] = DIS2[0][9];

   tuple[me101] = ME2[1][0];
   tuple[me102] = ME2[1][1];
   tuple[me103] = ME2[1][2];
   tuple[me104] = ME2[1][3];
   tuple[me105] = ME2[1][4];
   tuple[me106] = ME2[1][5];
   tuple[me107] = ME2[1][6];
   tuple[me108] = ME2[1][7];
   tuple[me109] = ME2[1][8];
   tuple[me110] = ME2[1][9];
   tuple[me201] = ME2[0][0];
   tuple[me202] = ME2[0][1];
   tuple[me203] = ME2[0][2];
   tuple[me204] = ME2[0][3];
   tuple[me205] = ME2[0][4];
   tuple[me206] = ME2[0][5];
   tuple[me207] = ME2[0][6];
   tuple[me208] = ME2[0][7];
   tuple[me209] = ME2[0][8];
   tuple[me210] = ME2[0][9];

   tuple[conme101] = CONME2[1][0];
   tuple[conme102] = CONME2[1][1];
   tuple[conme103] = CONME2[1][2];
   tuple[conme104] = CONME2[1][3];
   tuple[conme105] = CONME2[1][4];
   tuple[conme106] = CONME2[1][5];
   tuple[conme107] = CONME2[1][6];
   tuple[conme108] = CONME2[1][7];
   tuple[conme109] = CONME2[1][8];
   tuple[conme110] = CONME2[1][9];
   tuple[conme201] = CONME2[0][0];
   tuple[conme202] = CONME2[0][1];
   tuple[conme203] = CONME2[0][2];
   tuple[conme204] = CONME2[0][3];
   tuple[conme205] = CONME2[0][4];
   tuple[conme206] = CONME2[0][5];
   tuple[conme207] = CONME2[0][6];
   tuple[conme208] = CONME2[0][7];
   tuple[conme209] = CONME2[0][8];
   tuple[conme210] = CONME2[0][9];

   tuple[pite1] = PE1[1];
   tuple[pipx1] = PX1[1];
   tuple[pipy1] = PY1[1];
   tuple[pipz1] = PZ1[1];
   tuple[mute1] = PE2[1];
   tuple[mupx1] = PX2[1];
   tuple[mupy1] = PY2[1];
   tuple[mupz1] = PZ2[1];
   tuple[kte1] = PE3[1];
   tuple[kpx1] = PX3[1];
   tuple[kpy1] = PY3[1];
   tuple[kpz1] = PZ3[1];

   tuple[pisigma1] = PPISIGMA[1];
   tuple[musigma1] = PMUSIGMA[1];
   tuple[ksigma1] = PKSIGMA[1];

   tuple[pill1] = PPILL[1];
   tuple[mull1] = PMULL[1];
   tuple[kll1] = PKLL[1];

   tuple[pite2] = PE1[0];
   tuple[pipx2] = PX1[0];
   tuple[pipy2] = PY1[0];
   tuple[pipz2] = PZ1[0];
   tuple[mute2] = PE2[0];
   tuple[mupx2] = PX2[0];
   tuple[mupy2] = PY2[0];
   tuple[mupz2] = PZ2[0];
   tuple[kte2] = PE3[0];
   tuple[kpx2] = PX3[0];
   tuple[kpy2] = PY3[0];
   tuple[kpz2] = PZ3[0];

   tuple[pisigma2] = PPISIGMA[0];
   tuple[musigma2] = PMUSIGMA[0];
   tuple[ksigma2] = PKSIGMA[0];

   tuple[pill2] = PPILL[0];
   tuple[mull2] = PMULL[0];
   tuple[kll2] = PKLL[0];

   tuple[fpite1] = FPE1[1];
   tuple[fpipx1] = FPX1[1];
   tuple[fpipy1] = FPY1[1];
   tuple[fpipz1] = FPZ1[1];
   tuple[fmute1] = FPE2[1];
   tuple[fmupx1] = FPX2[1];
   tuple[fmupy1] = FPY2[1];
   tuple[fmupz1] = FPZ2[1];
   tuple[fkte1] = FPE3[1];
   tuple[fkpx1] = FPX3[1];
   tuple[fkpy1] = FPY3[1];
   tuple[fkpz1] = FPZ3[1];

   tuple[fpite2] = FPE1[0];
   tuple[fpipx2] = FPX1[0];
   tuple[fpipy2] = FPY1[0];
   tuple[fpipz2] = FPZ1[0];
   tuple[fmute2] = FPE2[0];
   tuple[fmupx2] = FPX2[0];
   tuple[fmupy2] = FPY2[0];
   tuple[fmupz2] = FPZ2[0];
   tuple[fkte2] = FPE3[0];
   tuple[fkpx2] = FPX3[0];
   tuple[fkpy2] = FPY3[0];
   tuple[fkpz2] = FPZ3[0];

   tuple[f1pite1] = PIF1PE[1];
   tuple[f1pipx1] = PIF1PX[1];
   tuple[f1pipy1] = PIF1PY[1];
   tuple[f1pipz1] = PIF1PZ[1];
   tuple[f1pite2] = PIF1PE[0];
   tuple[f1pipx2] = PIF1PX[0];
   tuple[f1pipy2] = PIF1PY[0];
   tuple[f1pipz2] = PIF1PZ[0];
   tuple[f1mute1] = MUF1PE[1];
   tuple[f1mupx1] = MUF1PX[1];
   tuple[f1mupy1] = MUF1PY[1];
   tuple[f1mupz1] = MUF1PZ[1];
   tuple[f1mute2] = MUF1PE[0];
   tuple[f1mupx2] = MUF1PX[0];
   tuple[f1mupy2] = MUF1PY[0];
   tuple[f1mupz2] = MUF1PZ[0];
   tuple[f1kte1] = KF1PE[1];
   tuple[f1kpx1] = KF1PX[1];
   tuple[f1kpy1] = KF1PY[1];
   tuple[f1kpz1] = KF1PZ[1];
   tuple[f1kte2] = KF1PE[0];
   tuple[f1kpx2] = KF1PX[0];
   tuple[f1kpy2] = KF1PY[0];
   tuple[f1kpz2] = KF1PZ[0];


   tuple[charge1] = CHARGE[1];
   tuple[charge2] = CHARGE[0];

  }
   tuple[beam_e] = BEAME;
   tuple[m_trk] = TRKM;
   tuple[mmpi_gen] = MMPI_GEN;


   (*m_showerTuple).fill(tuple);




   return ActionBase::kPassed;
}


ActionBase::ActionResult
TwoTracksISRProc::beginRun( Frame& iFrame )       // anal2 equiv.
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


   return ActionBase::kPassed;
}


/*
ActionBase::ActionResult
TwoTracksISRProc::endRun( Frame& iFrame )         // anal4 equiv.
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
