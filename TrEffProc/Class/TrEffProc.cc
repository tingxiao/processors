// -*- C++ -*-
//
// Package:     TrEffProc
// Module:      TrEffProc
// 
// Description: <one line class summary>
//
// Implementation:
//     <Notes on implementation>
//
// Author:      pts/1
// Created:     Wed May 12 17:15:38 CDT 2010
// $Id$
//
// Revision history
//
// $Log$
//

#include "Experiment/Experiment.h"

// system include files

// user include files
#include "TrEffProc/TrEffProc.h"
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

#include "MCBeam/MCTrueRfBucket.h"
#include "MCTriggerPhase/MCTriggerPhase.h"

#include "CleoDB/DBEventHeader.h"
#include "TriggerData/TriggerData.h"
#include "TriggerL1Data/TriggerL1Data.h"
#include "Level4Proc/Level4Decision.h"

#include "LabNet4Momentum/LabNet4Momentum.h"

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
static const char* const kFacilityString = "Processor.TrEffProc" ;

// ---- cvs-based strings (Id and Tag with which file was checked out)
static const char* const kIdString  = "$Id: processor.cc,v 1.41 2006/02/08 19:38:02 wsun Exp $";
static const char* const kTagString = "$Name: v07_03_00 $";

//
// static data member definitions
//



//
// constructors and destructor
//
TrEffProc::TrEffProc( void )               // anal1
   : Processor( "TrEffProc" )
{
   report( DEBUG, kFacilityString ) << "here in ctor()" << endl;

   // ---- bind a method to a stream -----
   // These lines ARE VERY IMPORTANT! If you don't bind the 
   // code you've just written (the "action") to a stream, 
   // your code won't get executed!

   bind( &TrEffProc::event,    Stream::kEvent );
   bind( &TrEffProc::beginRun, Stream::kBeginRun );
   //bind( &TrEffProc::endRun,   Stream::kEndRun );

   // do anything here that needs to be done at creation time
   // (e.g. allocate resources etc.)


}

TrEffProc::~TrEffProc()                    // anal5
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
TrEffProc::init( void )          // anal1 "Interactive"
{
   report( DEBUG, kFacilityString ) << "here in init()" << endl;

   // do any initialization here based on Parameter Input by User
   // (e.g. run expensive algorithms that are based on parameters
   //  specified by user at run-time)

}

// -------------------- terminate method ----------------------------
void
TrEffProc::terminate( void )     // anal5 "Interactive"
{
   report( DEBUG, kFacilityString ) << "here in terminate()" << endl;

   // do anything here BEFORE New Parameter Change
   // (e.g. write out result based on parameters from user-input)
 
}


// ---------------- standard place to book histograms ---------------

enum {
knshower,
e1,th1,phi1,e9251, // ISR shower
e2,th2,phi2,e9252,
e3,th3,phi3,e9253,
e4,th4,phi4,e9254,
e5,th5,phi5,e9255,
e6,th6,phi6,e9256,
e7,th7,phi7,e9257,
e8,th8,phi8,e9258,
e9,th9,phi9,e9259,
e10,th10,phi10,e92510,

kntrack, // number of primary track
fkntrack, // number of secondary track
pite1,pipx1,pipy1,pipz1,pite2,pipx2,pipy2,pipz2,
pite3,pipx3,pipy3,pipz3,pite4,pipx4,pipy4,pipz4,
fpite1,fpipx1,fpipy1,fpipz1,fpite2,fpipx2,fpipy2,fpipz2,
fpite3,fpipx3,fpipy3,fpipz3,fpite4,fpipx4,fpipy4,fpipz4,
me1,me2,
me3,me4,
eop1,eop2,
eop3,eop4,
mudepth1,mudepth2,
mudepth3,mudepth4,
charge1,charge2,
charge3,charge4,
chisqvtxpi1,chisqfitpi1,
chisqvtxpi2,chisqfitpi2,
chisqvtxpi3,chisqfitpi3,
chisqvtxpi4,chisqfitpi4,
pisigma1,musigma1,ksigma1,pill1,mull1,kll1,
pisigma2,musigma2,ksigma2,pill2,mull2,kll2,
pisigma3,musigma3,ksigma3,pill3,mull3,kll3,
pisigma4,musigma4,ksigma4,pill4,mull4,kll4,
dx1,dy1,dz1,dx2,dy2,dz2,
dx3,dy3,dz3,dx4,dy4,dz4, // distance to the center of beam

kVarNum};

void
TrEffProc::hist_book( HIHistoManager& iHistoManager )
{
   report( DEBUG, kFacilityString ) << "here in hist_book()" << endl;

   // book your histograms here

   HINtupleVarNames ntupleNames(kVarNum);

   ntupleNames.addVar(knshower,       "knshower");

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
   ntupleNames.addVar(phi4,	  "phi4");
   ntupleNames.addVar(e9254,    "e9254");

   ntupleNames.addVar(e5,          "e5");
   ntupleNames.addVar(th5,        "th5");
   ntupleNames.addVar(phi5,       "phi5");
   ntupleNames.addVar(e9255,    "e9255");

   ntupleNames.addVar(e6,          "e6");
   ntupleNames.addVar(th6,        "th6");
   ntupleNames.addVar(phi6,	  "phi6");
   ntupleNames.addVar(e9256,    "e9256");

   ntupleNames.addVar(e7,          "e7");
   ntupleNames.addVar(th7,        "th7");
   ntupleNames.addVar(phi7,	  "phi7");
   ntupleNames.addVar(e9257,    "e9257");

   ntupleNames.addVar(e8,          "e8");
   ntupleNames.addVar(th8,        "th8");
   ntupleNames.addVar(phi8,	  "phi8");
   ntupleNames.addVar(e9258,    "e9258");

   ntupleNames.addVar(e9,          "e9");
   ntupleNames.addVar(th9,        "th9");
   ntupleNames.addVar(phi9,	  "phi9");
   ntupleNames.addVar(e9259,    "e9259");

   ntupleNames.addVar(e10,          "e10");
   ntupleNames.addVar(th10,        "th10");
   ntupleNames.addVar(phi10,	   "phi10");
   ntupleNames.addVar(e92510,    "e92510");

   ntupleNames.addVar(kntrack,     "kntrack");
   ntupleNames.addVar(fkntrack,     "fkntrack");

   ntupleNames.addVar(pisigma1, "pisigma1");
   ntupleNames.addVar(musigma1, "musigma1");
   ntupleNames.addVar(ksigma1, "ksigma1");
   ntupleNames.addVar(pill1,    "pill1");
   ntupleNames.addVar(mull1,    "mull1");
   ntupleNames.addVar(kll1,    "kll1");

   ntupleNames.addVar(pisigma2, "pisigma2");
   ntupleNames.addVar(musigma2, "musigma2");
   ntupleNames.addVar(ksigma2, "ksigma2");
   ntupleNames.addVar(pill2,    "pill2");
   ntupleNames.addVar(mull2,    "mull2");
   ntupleNames.addVar(kll2,    "kll2");

   ntupleNames.addVar(pisigma3, "pisigma3");
   ntupleNames.addVar(musigma3, "musigma3");
   ntupleNames.addVar(ksigma3, "ksigma3");
   ntupleNames.addVar(pill3,    "pill3");
   ntupleNames.addVar(mull3,    "mull3");
   ntupleNames.addVar(kll3,    "kll3");

   ntupleNames.addVar(pisigma4, "pisigma4");
   ntupleNames.addVar(musigma4, "musigma4");
   ntupleNames.addVar(ksigma4, "ksigma4");
   ntupleNames.addVar(pill4,    "pill4");
   ntupleNames.addVar(mull4,    "mull4");
   ntupleNames.addVar(kll4,    "kll4");

   ntupleNames.addVar(pite1,    "pite1");
   ntupleNames.addVar(pipx1,    "pipx1");
   ntupleNames.addVar(pipy1,    "pipy1");
   ntupleNames.addVar(pipz1,    "pipz1");
   ntupleNames.addVar(pite2,    "pite2");
   ntupleNames.addVar(pipx2,    "pipx2");
   ntupleNames.addVar(pipy2,    "pipy2");
   ntupleNames.addVar(pipz2,    "pipz2");
   ntupleNames.addVar(pite3,    "pite3");
   ntupleNames.addVar(pipx3,    "pipx3");
   ntupleNames.addVar(pipy3,    "pipy3");
   ntupleNames.addVar(pipz3,    "pipz3");
   ntupleNames.addVar(pite4,    "pite4");
   ntupleNames.addVar(pipx4,    "pipx4");
   ntupleNames.addVar(pipy4,    "pipy4");
   ntupleNames.addVar(pipz4,    "pipz4");

   ntupleNames.addVar(fpite1,    "fpite1");
   ntupleNames.addVar(fpipx1,    "fpipx1");
   ntupleNames.addVar(fpipy1,    "fpipy1");
   ntupleNames.addVar(fpipz1,    "fpipz1");
   ntupleNames.addVar(fpite2,    "fpite2");
   ntupleNames.addVar(fpipx2,    "fpipx2");
   ntupleNames.addVar(fpipy2,    "fpipy2");
   ntupleNames.addVar(fpipz2,    "fpipz2");
   ntupleNames.addVar(fpite3,    "fpite3");
   ntupleNames.addVar(fpipx3,    "fpipx3");
   ntupleNames.addVar(fpipy3,    "fpipy3");
   ntupleNames.addVar(fpipz3,    "fpipz3");
   ntupleNames.addVar(fpite4,    "fpite4");
   ntupleNames.addVar(fpipx4,    "fpipx4");
   ntupleNames.addVar(fpipy4,    "fpipy4");
   ntupleNames.addVar(fpipz4,    "fpipz4");

   ntupleNames.addVar(me1,    "me1");
   ntupleNames.addVar(me2,    "me2");
   ntupleNames.addVar(me3,    "me3");
   ntupleNames.addVar(me4,    "me4");

   ntupleNames.addVar(eop1,    "eop1");
   ntupleNames.addVar(eop2,    "eop2");
   ntupleNames.addVar(eop3,    "eop3");
   ntupleNames.addVar(eop4,    "eop4");

   ntupleNames.addVar(mudepth1,    "mudepth1");
   ntupleNames.addVar(mudepth2,    "mudepth2");
   ntupleNames.addVar(mudepth3,    "mudepth3");
   ntupleNames.addVar(mudepth4,    "mudepth4");

   ntupleNames.addVar(charge1,    "charge1");
   ntupleNames.addVar(charge2,    "charge2");
   ntupleNames.addVar(charge3,    "charge3");
   ntupleNames.addVar(charge4,    "charge4");

   ntupleNames.addVar(dx1,    "dx1");
   ntupleNames.addVar(dx2,    "dx2");
   ntupleNames.addVar(dx3,    "dx3");
   ntupleNames.addVar(dx4,    "dx4");

   ntupleNames.addVar(dy1,    "dy1");
   ntupleNames.addVar(dy2,    "dy2");
   ntupleNames.addVar(dy3,    "dy3");
   ntupleNames.addVar(dy4,    "dy4");

   ntupleNames.addVar(dz1,    "dz1");
   ntupleNames.addVar(dz2,    "dz2");
   ntupleNames.addVar(dz3,    "dz3");
   ntupleNames.addVar(dz4,    "dz4");


   ntupleNames.addVar(chisqvtxpi1,          "chisqvtxpi1");
   ntupleNames.addVar(chisqfitpi1,          "chisqfitpi1");
   ntupleNames.addVar(chisqvtxpi2,          "chisqvtxpi2");
   ntupleNames.addVar(chisqfitpi2,          "chisqfitpi2");
   ntupleNames.addVar(chisqvtxpi3,          "chisqvtxpi3");
   ntupleNames.addVar(chisqfitpi3,          "chisqfitpi3");
   ntupleNames.addVar(chisqvtxpi4,          "chisqvtxpi4");
   ntupleNames.addVar(chisqfitpi4,          "chisqfitpi4");

   m_showerTuple = iHistoManager.ntuple(10,"tuple", kVarNum, 10000, ntupleNames.names());
}

// --------------------- methods bound to streams -------------------
ActionBase::ActionResult
TrEffProc::event( Frame& iFrame )          // anal3 equiv.
{
   report( DEBUG, kFacilityString ) << "here in event()" << endl;

   const int kMaxShower = 10;
   const int kMaxTrack = 4;

   double E[kMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};
   double Theta[kMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};
   double Phi[kMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};
   double E925[kMaxShower] = {-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.,-10.};

   int FKNTRACK = 0;

   double PIPE[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};
   double PIPX[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};
   double PIPY[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};
   double PIPZ[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};

   double FPIPE[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};
   double FPIPX[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};
   double FPIPY[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};
   double FPIPZ[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};

   double DX[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};
   double DY[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};
   double DZ[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};

   double ME[kMaxTrack] = {0.,0.,0.,0.};
   double EOP[kMaxTrack] = {0.,0.,0.,0.};
   double MUDEPTH[kMaxTrack] = {0.,0.,0.,0.};

   double PPISIGMA[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};
   double PMUSIGMA[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};
   double PKSIGMA[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};

   double PPILL[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};
   double PMULL[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};
   double PKLL[kMaxTrack] = {-1000.,-1000.,-1000.,-1000};

   int CHARGE[kMaxTrack] = {0,0,0,0};

   double CHISQVTXPI[kMaxTrack] = {10000000.,10000000.,10000000.,10000000.};
   double CHISQFITPI[kMaxTrack] = {10000000.,10000000.,10000000.,10000000.};


   double LABPE = -1000.;
   double LABPX = -1000.;
   double LABPY = -1000.;
   double LABPZ = -1000.;


   float tuple[kVarNum];


// if we're in MC, check to see if the trigger fired
   FAItem< DBEventHeader > eventHeader ;
   extract( iFrame.record( Stream::kEvent ), eventHeader ) ;

   double the_run = eventHeader->run();
   double the_evt = eventHeader->number();

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

      if(trigDat->isHadron()) {
        L1Trigger = 1.;
      }
      if(trigDat->isRadTau()) {
        L1Trigger = 2.;
      }
      if(trigDat->isElTrack()) {
        L1Trigger = 3.;
      }
      if(trigDat->isBarrelBhabha()) {
        L1Trigger = 4.;
      }
      if(trigDat->isEndcapBhabha()) {
        L1Trigger = 5.;
      }
      if(trigDat->isMuPair()) {
        L1Trigger = 6.;
      }
      if(trigDat->isCBSingleHi()) {
        L1Trigger = 7.;
      }
      if(trigDat->isCESingleHi()) {
        L1Trigger = 8.;
      }
      if(trigDat->isPulser()) {
        L1Trigger = 9.;
      }
      if(trigDat->isRandom()) {
        L1Trigger = 10.;
      }
      if(trigDat->isTwoTrack()) {
        L1Trigger = 11.;
      }
      if(trigDat->isMinBias()) {
        L1Trigger = 12.;
      }
      if(trigDat->isPhotonA()) {
        L1Trigger = 13.;
      }
      if(trigDat->isPhotonB()) {
        L1Trigger = 14.;
      }
      if(trigDat->isPhotonC()) {
        L1Trigger = 15.;
      }
      if(trigDat->isPhotonD()) {
        L1Trigger = 16.;
      }

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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   FATable< NavShower > showerTable;
   extract( iFrame.record( Stream::kEvent ) , showerTable, "GoodThings" );

   if(showerTable.size()==0)
      return ActionBase::kFailed;

   FATable< NavShower >::const_iterator showerBegin = showerTable.begin();
   FATable< NavShower >::const_iterator showerEnd = showerTable.end();

   if(showerBegin->attributes().x925() < 1)
      return ActionBase::kFailed;
   if(showerBegin->attributes().energy() < 0.5)
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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   // Extract total 4-momentum to get CM energy
   FAItem< LabNet4Momentum > labMomentum ;
   extract( iFrame.record( Stream::kStartRun ), labMomentum ) ;

   LABPE= (*labMomentum).e();
   LABPX= (*labMomentum).px();
   LABPY= (*labMomentum).py();
   LABPZ= (*labMomentum).pz();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   FAItem< BeamSpot > spot;
   extract( iFrame.record( Stream::kBeginRun ), spot );

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   FATable< NavTrack > trackTable;
   extract( iFrame.record( Stream::kEvent ) , trackTable, "GoodThings"  );

   if( trackTable.size() == 0)
      return ActionBase::kFailed;

   if( trackTable.size() > 4)
      return ActionBase::kFailed;

   FATable< NavTrack >::const_iterator trackBegin = trackTable.begin();
   FATable< NavTrack >::const_iterator trackEnd = trackTable.end();

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

      double cos_theta01 = pionFit->pz() / pionFit->pmag();
      double cos_theta02 = muonFit->pz() / muonFit->pmag();
      double cos_theta03 = kaonFit->pz() / kaonFit->pmag();

      FAItem<TDKinematicFit> pi1 = (*trackItr).pionFit();
      double cos_theta1 = pi1->pz() / pi1->pmag();
      double pt1 = sqrt(pow(pi1->px(),2)+pow(pi1->py(),2));

      FAItem<TRHelixFit> pionHelix = trackItr->pionHelix();

      int j = unmatchedTracks++;

      if(j<kMaxTrack){

        DX[j] =  -pionHelix->d0()*sin(pionHelix->phi0()) - spot->center().x();
        DY[j] =  pionHelix->d0()*cos(pionHelix->phi0()) - spot->center().x();
        DZ[j] = pionHelix->z0() - spot->center().z();

        PIPE[j]=pi1->lorentzMomentum().e();
        PIPX[j]=pi1->px();
        PIPY[j]=pi1->py();
        PIPZ[j]=pi1->pz();
        CHARGE[j]=pi1->charge();

        if(CHARGE[j] == 0)
          return ActionBase::kFailed;

        if(trackItr->trackShowerMatch().valid())
          ME[j] = trackItr->trackShowerMatch()->matchedEnergy();

        EOP[j] = trackItr->elecId().eOverP();
        MUDEPTH[j] = trackItr->muonId().depth();

        if(EOP[j]>0.8)
          return ActionBase::kFailed;

        if( (pionFit.valid()) && (the_dedx.valid()) )  {

          PPISIGMA[j]=the_dedx->piSigma();

          if ((the_rich.valid()) && (fabs(cos_theta01) < 0.8) && (the_rich->pionHypWasAnalyzed()))
            PPILL[j]=the_rich->pionLogLikelihood();
        }

        if( (muonFit.valid()) && (the_dedx.valid()) )  {

          PMUSIGMA[j]=the_dedx->muSigma();

          if ((the_rich.valid()) && (fabs(cos_theta02) < 0.8) && (the_rich->muonHypWasAnalyzed()))
            PMULL[j]=the_rich->muonLogLikelihood();
        }

        if( (kaonFit.valid()) && (the_dedx.valid()) )  {

          PKSIGMA[j]=the_dedx->kSigma();

          if ((the_rich.valid()) && (fabs(cos_theta03) < 0.8) && (the_rich->kaonHypWasAnalyzed()))
            PKLL[j]=the_rich->kaonLogLikelihood();
        }


        if (pi1.valid()) {
//      if ((pi1.valid()) && (pt1>0.16) && (fabs(cos_theta1) < 0.75)) {

          FitEvt pipi( "Pipi", FitEvt::k_MissingMass,0,0.13957018 );
          pipi.newPhoton(*showerBegin);
          pipi.newTrack(*pi1);
          pipi.doTheFit();

          if( (pipi.chisqVtx()>0) && (pipi.chisqFit()>0) ) {
//          if( (pipi.chisqVtx()>0) && (pipi.chisqFit()>0) && (pipi.chisqVtx()<15) && (pipi.chisqFit()<10)) {

            CHISQVTXPI[j] = pipi.chisqVtx();
            CHISQFITPI[j] = pipi.chisqFit();

            double cos_theta_predicted = FPIPZ[j] / sqrt(pow(FPIPX[j],2) + pow(FPIPY[j],2) + pow(FPIPZ[j],2));
            double pt_predicted = sqrt(pow(FPIPX[j],2)+pow(FPIPY[j],2));

//            if((pt_predicted>0.16) && (fabs(cos_theta_predicted) < 0.75) && (CHARGE[j] != 0)) {

            for ( FATable< NavTrack >::const_iterator trackItr2 = trackBegin;
                    trackItr2 != trackEnd ;
                    ++trackItr2 )  {

              FAItem<TDKinematicFit> pi2 = (*trackItr2).pionFit();
              double cos_theta2 = pi2->pz() / pi2->pmag();
              double pt2 = sqrt(pow(pi2->px(),2)+pow(pi2->py(),2));

              FPIPE[j] = LABPE - PIPE[j] - E[0];
              FPIPX[j] = LABPX - PIPX[j] - E[0]*sin(Theta[0])*cos(Phi[0]);
              FPIPY[j] = LABPY - PIPY[j] - E[0]*sin(Theta[0])*sin(Phi[0]);
              FPIPZ[j] = LABPZ - PIPZ[j] - E[0]*cos(Theta[0]);


              if( (pi2.valid()) && (pi2->charge()==-pi1->charge()) ) {
//                if((pi2.valid()) && (pi2->charge()==-pi1->charge()) && (pt2>0.16) && (fabs(cos_theta2) < 0.75)) {

                FKNTRACK++;
                break;
              }
              else
                continue;
            }
          }
        }
      }
   }


   tuple[knshower] = unmatchedShowers;
   tuple[kntrack] = unmatchedTracks;

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

   tuple[fkntrack] = FKNTRACK;

   tuple[charge1] =CHARGE[0];
   tuple[charge2] =CHARGE[1];
   tuple[charge3] =CHARGE[2];
   tuple[charge4] =CHARGE[3];

   tuple[me1] =ME[0];
   tuple[me2] =ME[1];
   tuple[me3] =ME[2];
   tuple[me4] =ME[3];

   tuple[eop1] =EOP[0];
   tuple[eop2] =EOP[1];
   tuple[eop3] =EOP[2];
   tuple[eop4] =EOP[3];

   tuple[mudepth1] =MUDEPTH[0];
   tuple[mudepth2] =MUDEPTH[1];
   tuple[mudepth3] =MUDEPTH[2];
   tuple[mudepth4] =MUDEPTH[3];

   tuple[pite1] = PIPE[0];
   tuple[pipx1] = PIPX[0];
   tuple[pipy1] = PIPY[0];
   tuple[pipz1] = PIPZ[0];
   tuple[pite2] = PIPE[1];
   tuple[pipx2] = PIPX[1];
   tuple[pipy2] = PIPY[1];
   tuple[pipz2] = PIPZ[1];
   tuple[pite3] = PIPE[2];
   tuple[pipx3] = PIPX[2];
   tuple[pipy3] = PIPY[2];
   tuple[pipz3] = PIPZ[2];
   tuple[pite4] = PIPE[3];
   tuple[pipx4] = PIPX[3];
   tuple[pipy4] = PIPY[3];
   tuple[pipz4] = PIPZ[3];

   tuple[fpite1] = FPIPE[0];
   tuple[fpipx1] = FPIPX[0];
   tuple[fpipy1] = FPIPY[0];
   tuple[fpipz1] = FPIPZ[0];
   tuple[fpite2] = FPIPE[1];
   tuple[fpipx2] = FPIPX[1];
   tuple[fpipy2] = FPIPY[1];
   tuple[fpipz2] = FPIPZ[1];
   tuple[fpite3] = FPIPE[2];
   tuple[fpipx3] = FPIPX[2];
   tuple[fpipy3] = FPIPY[2];
   tuple[fpipz3] = FPIPZ[2];
   tuple[fpite4] = FPIPE[3];
   tuple[fpipx4] = FPIPX[3];
   tuple[fpipy4] = FPIPY[3];
   tuple[fpipz4] = FPIPZ[3];

   tuple[dx1] = DX[0];
   tuple[dy1] = DY[0];
   tuple[dz1] = DZ[0];
   tuple[dx2] = DX[1];
   tuple[dy2] = DY[1];
   tuple[dz2] = DZ[1];
   tuple[dx3] = DX[2];
   tuple[dy3] = DY[2];
   tuple[dz3] = DZ[2];
   tuple[dx4] = DX[3];
   tuple[dy4] = DY[3];
   tuple[dz4] = DZ[3];

   tuple[chisqvtxpi1] = CHISQVTXPI[0];
   tuple[chisqfitpi1] = CHISQFITPI[0];
   tuple[chisqvtxpi2] = CHISQVTXPI[1];
   tuple[chisqfitpi2] = CHISQFITPI[1];
   tuple[chisqvtxpi3] = CHISQVTXPI[2];
   tuple[chisqfitpi3] = CHISQFITPI[2];
   tuple[chisqvtxpi4] = CHISQVTXPI[3];
   tuple[chisqfitpi4] = CHISQFITPI[3];

   tuple[pisigma1] = PPISIGMA[0];
   tuple[musigma1] = PMUSIGMA[0];
   tuple[ksigma1] = PKSIGMA[0];
   tuple[pill1] = PPILL[0];
   tuple[mull1] = PMULL[0];
   tuple[kll1] = PKLL[0];

   tuple[pisigma2] = PPISIGMA[1];
   tuple[musigma2] = PMUSIGMA[1];
   tuple[ksigma2] = PKSIGMA[1];
   tuple[pill2] = PPILL[1];
   tuple[mull2] = PMULL[1];
   tuple[kll2] = PKLL[1];


   tuple[pisigma3] = PPISIGMA[2];
   tuple[musigma3] = PMUSIGMA[2];
   tuple[ksigma3] = PKSIGMA[2];
   tuple[pill3] = PPILL[2];
   tuple[mull3] = PMULL[2];
   tuple[kll3] = PKLL[2];

   tuple[pisigma4] = PPISIGMA[3];
   tuple[musigma4] = PMUSIGMA[3];
   tuple[ksigma4] = PKSIGMA[3];
   tuple[pill4] = PPILL[3];
   tuple[mull4] = PMULL[3];
   tuple[kll4] = PKLL[3];

   (*m_showerTuple).fill(tuple);

   return ActionBase::kPassed;
}


ActionBase::ActionResult
TrEffProc::beginRun( Frame& iFrame )       // anal2 equiv.
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
TrEffProc::endRun( Frame& iFrame )         // anal4 equiv.
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
