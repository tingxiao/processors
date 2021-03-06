// -*- C++ -*-
//
// Package:     PipiPsi2SProc
// Module:      PipiPsi2SProc
// 
// Description: <one line class summary>
//
// Implementation:
//     <Notes on implementation>
//
// Author:      
// Created:     Fri Apr  5 17:39:24 CDT 2013
// $Id$
//
// Revision history
//
// $Log$
//

#include "Experiment/Experiment.h"

// system include files

// user include files
#include "PipiPsi2SProc/PipiPsi2SProc.h"
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
#include "Navigation/NavTkShMatch.h"

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
static const char* const kFacilityString = "Processor.PipiPsi2SProc" ;

// ---- cvs-based strings (Id and Tag with which file was checked out)
static const char* const kIdString  = "$Id: processor.cc,v 1.41 2006/02/08 19:38:02 wsun Exp $";
static const char* const kTagString = "$Name: v07_03_00 $";

//
// static data member definitions
//



//
// constructors and destructor
//
PipiPsi2SProc::PipiPsi2SProc( void )               // anal1
   : Processor( "PipiPsi2SProc" )
{
   report( DEBUG, kFacilityString ) << "here in ctor()" << endl;

   // ---- bind a method to a stream -----
   // These lines ARE VERY IMPORTANT! If you don't bind the 
   // code you've just written (the "action") to a stream, 
   // your code won't get executed!

   bind( &PipiPsi2SProc::event,    Stream::kEvent );
   bind( &PipiPsi2SProc::beginRun, Stream::kBeginRun );
   //bind( &PipiPsi2SProc::endRun,   Stream::kEndRun );

   // do anything here that needs to be done at creation time
   // (e.g. allocate resources etc.)


}

PipiPsi2SProc::~PipiPsi2SProc()                    // anal5
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
PipiPsi2SProc::init( void )          // anal1 "Interactive"
{
   report( DEBUG, kFacilityString ) << "here in init()" << endl;

   // do any initialization here based on Parameter Input by User
   // (e.g. run expensive algorithms that are based on parameters
   //  specified by user at run-time)

}

// -------------------- terminate method ----------------------------
void
PipiPsi2SProc::terminate( void )     // anal5 "Interactive"
{
   report( DEBUG, kFacilityString ) << "here in terminate()" << endl;

   // do anything here BEFORE New Parameter Change
   // (e.g. write out result based on parameters from user-input)
 
}


// ---------------- standard place to book histograms ---------------
enum {
chisqv1,chisqf1,         //chisq of 4C fit, normal 4C fit
eop1,eop2,eop3,eop4,eop5,eop6,
me1,me2,me3,me4,me5,me6,
dedx1,dedx2,dedx3,dedx4,dedx5,dedx6,
trte1,trpx1,trpy1,trpz1,trte2,trpx2,trpy2,trpz2,
trte3,trpx3,trpy3,trpz3,trte4,trpx4,trpy4,trpz4,
trte5,trpx5,trpy5,trpz5,trte6,trpx6,trpy6,trpz6,
f1trte1,f1trpx1,f1trpy1,f1trpz1,f1trte2,f1trpx2,f1trpy2,f1trpz2,
f1trte3,f1trpx3,f1trpy3,f1trpz3,f1trte4,f1trpx4,f1trpy4,f1trpz4,  // 4C fit
f1trte5,f1trpx5,f1trpy5,f1trpz5,f1trte6,f1trpx6,f1trpy6,f1trpz6,
id1,id2,id3,id4,id5,id6,charge1,charge2,charge3,charge4,charge5,charge6,
passed1,passed2,passed3,
peak1,peak2,peak3,
kVarNum};


void
PipiPsi2SProc::hist_book( HIHistoManager& iHistoManager )
{
   report( DEBUG, kFacilityString ) << "here in hist_book()" << endl;

   // book your histograms here

   HINtupleVarNames ntupleNames(kVarNum);

   ntupleNames.addVar(chisqv1,       "chisqv1");
   ntupleNames.addVar(chisqf1,       "chisqf1");
   ntupleNames.addVar(eop1,         "eop1");
   ntupleNames.addVar(eop2,         "eop2");
   ntupleNames.addVar(eop3,         "eop3");
   ntupleNames.addVar(eop4,         "eop4");
   ntupleNames.addVar(eop5,         "eop5");
   ntupleNames.addVar(eop6,         "eop6");
   ntupleNames.addVar(me1,          "me1");
   ntupleNames.addVar(me2,          "me2");
   ntupleNames.addVar(me3,          "me3");
   ntupleNames.addVar(me4,          "me4");
   ntupleNames.addVar(me5,          "me5");
   ntupleNames.addVar(me6,          "me6");
   ntupleNames.addVar(dedx1,        "dedx1");
   ntupleNames.addVar(dedx2,        "dedx2");
   ntupleNames.addVar(dedx3,        "dedx3");
   ntupleNames.addVar(dedx4,        "dedx4");
   ntupleNames.addVar(dedx5,        "dedx5");
   ntupleNames.addVar(dedx6,        "dedx6");
   ntupleNames.addVar(trte1,        "trte1");
   ntupleNames.addVar(trpx1,        "trpx1");
   ntupleNames.addVar(trpy1,        "trpy1");
   ntupleNames.addVar(trpz1,        "trpz1");
   ntupleNames.addVar(trte2,        "trte2");
   ntupleNames.addVar(trpx2,        "trpx2");
   ntupleNames.addVar(trpy2,        "trpy2");
   ntupleNames.addVar(trpz2,        "trpz2");
   ntupleNames.addVar(trte3,        "trte3");
   ntupleNames.addVar(trpx3,        "trpx3");
   ntupleNames.addVar(trpy3,        "trpy3");
   ntupleNames.addVar(trpz3,        "trpz3");
   ntupleNames.addVar(trte4,        "trte4");
   ntupleNames.addVar(trpx4,        "trpx4");
   ntupleNames.addVar(trpy4,        "trpy4");
   ntupleNames.addVar(trpz4,        "trpz4");
   ntupleNames.addVar(trte5,        "trte5");
   ntupleNames.addVar(trpx5,        "trpx5");
   ntupleNames.addVar(trpy5,        "trpy5");
   ntupleNames.addVar(trpz5,        "trpz5");
   ntupleNames.addVar(trte6,        "trte6");
   ntupleNames.addVar(trpx6,        "trpx6");
   ntupleNames.addVar(trpy6,        "trpy6");
   ntupleNames.addVar(trpz6,        "trpz6");
   ntupleNames.addVar(f1trte1,       "f1trte1");
   ntupleNames.addVar(f1trpx1,       "f1trpx1");
   ntupleNames.addVar(f1trpy1,       "f1trpy1");
   ntupleNames.addVar(f1trpz1,       "f1trpz1");
   ntupleNames.addVar(f1trte2,       "f1trte2");
   ntupleNames.addVar(f1trpx2,       "f1trpx2");
   ntupleNames.addVar(f1trpy2,       "f1trpy2");
   ntupleNames.addVar(f1trpz2,       "f1trpz2");
   ntupleNames.addVar(f1trte3,       "f1trte3");
   ntupleNames.addVar(f1trpx3,       "f1trpx3");
   ntupleNames.addVar(f1trpy3,       "f1trpy3");
   ntupleNames.addVar(f1trpz3,       "f1trpz3");
   ntupleNames.addVar(f1trte4,       "f1trte4");
   ntupleNames.addVar(f1trpx4,       "f1trpx4");
   ntupleNames.addVar(f1trpy4,       "f1trpy4");
   ntupleNames.addVar(f1trpz4,       "f1trpz4");
   ntupleNames.addVar(f1trte5,       "f1trte5");
   ntupleNames.addVar(f1trpx5,       "f1trpx5");
   ntupleNames.addVar(f1trpy5,       "f1trpy5");
   ntupleNames.addVar(f1trpz5,       "f1trpz5");
   ntupleNames.addVar(f1trte6,       "f1trte6");
   ntupleNames.addVar(f1trpx6,       "f1trpx6");
   ntupleNames.addVar(f1trpy6,       "f1trpy6");
   ntupleNames.addVar(f1trpz6,       "f1trpz6");
   ntupleNames.addVar(id1,          "id1");
   ntupleNames.addVar(id2,          "id2");
   ntupleNames.addVar(id3,          "id3");
   ntupleNames.addVar(id4,          "id4");
   ntupleNames.addVar(id5,          "id5");
   ntupleNames.addVar(id6,          "id6");
   ntupleNames.addVar(charge1,      "charge1");
   ntupleNames.addVar(charge2,      "charge2");
   ntupleNames.addVar(charge3,      "charge3");
   ntupleNames.addVar(charge4,      "charge4");
   ntupleNames.addVar(charge5,      "charge5");
   ntupleNames.addVar(charge6,      "charge6");
   ntupleNames.addVar(passed1,      "passed1");
   ntupleNames.addVar(passed2,      "passed2");
   ntupleNames.addVar(passed3,      "passed3");
   ntupleNames.addVar(peak1,        "peak1");
   ntupleNames.addVar(peak2,        "peak2");
   ntupleNames.addVar(peak3,        "peak3");

   m_showerTuple = iHistoManager.ntuple(10,"tuple", kVarNum, 10000, ntupleNames.names());

}

// --------------------- methods bound to streams -------------------
ActionBase::ActionResult
PipiPsi2SProc::event( Frame& iFrame )          // anal3 equiv.
{
   report( DEBUG, kFacilityString ) << "here in event()" << endl;

   const int kMaxTrack = 6;

   double ChisqV1 = 1000000.;
   double ChisqF1 = 1000000.;
   double ME[kMaxTrack] = {10000.,10000.,10000.,10000.,10000.,10000.};
   double EOP[kMaxTrack] = {10000.,10000.,10000.,10000,10000.,10000.};
   double DEDX[kMaxTrack] = {10000.,10000.,10000.,10000.,10000.,10000.};
   double PE[kMaxTrack] = {0.,0.,0.,0.,0.,0.};
   double PX[kMaxTrack] = {0.,0.,0.,0.,0.,0.};
   double PY[kMaxTrack] = {0.,0.,0.,0.,0.,0.};
   double PZ[kMaxTrack] = {0.,0.,0.,0.,0.,0.};
   double F1PE[kMaxTrack] = {0.,0.,0.,0.,0.,0.};
   double F1PX[kMaxTrack] = {0.,0.,0.,0.,0.,0.};
   double F1PY[kMaxTrack] = {0.,0.,0.,0.,0.,0.};
   double F1PZ[kMaxTrack] = {0.,0.,0.,0.,0.,0.};
   int CHARGE[kMaxTrack] = {100,100,100,100,100,100};
   int ID[kMaxTrack] = {-100,-100,-100,-100,-100,-100};  //0:pi, 1:e, 2: mu
   double PASSED1 = 0;
   double PASSED2 = 0;
   double PASSED3 = 0;
   double PEAK1 = 0;
   double PEAK2 = 0;
   double PEAK3 = 0;


   float tuple[kVarNum];

//////////////////////////////////// # of tracks == 6 /////////////////////////////////


   FATable< NavTrack > trackTable;
   extract( iFrame.record( Stream::kEvent ) , trackTable, "GoodThings" );

   if(trackTable.size() != kMaxTrack)
       return ActionBase::kFailed;

   FATable< NavTrack >::const_iterator trackTableBegin = trackTable.begin();
   FATable< NavTrack >::const_iterator trackTableEnd = trackTable.end();

/////////////////////////////////// net charge == 0 ////////////////////////////////////

   int unmatchedtracks = 0;
   int charge_sum = 0;

   for ( FATable< NavTrack >::const_iterator trackItr = trackTableBegin;
         trackItr != trackTableEnd ;
         ++trackItr )
   {
      int i = unmatchedtracks++;
      CHARGE[i]=(*trackItr).pionFit()->charge();
      PE[i]=(*trackItr).pionFit()->energy();
      PX[i]=(*trackItr).pionFit()->px();
      PY[i]=(*trackItr).pionFit()->py();
      PZ[i]=(*trackItr).pionFit()->pz();
      charge_sum += CHARGE[i];
   }

   if(charge_sum != 0)
       return ActionBase::kFailed;

   PASSED1 = 1;
   PEAK1 = sqrt(pow((PE[0]+PE[1]+PE[2]+PE[3]+PE[4]+PE[5]),2)-pow((PX[0]+PX[1]+PX[2]+PX[3]+PX[4]+PX[5]),2)-
                pow((PY[0]+PY[1]+PY[2]+PY[3]+PY[4]+PY[5]),2)-pow((PZ[0]+PZ[1]+PZ[2]+PZ[3]+PZ[4]+PZ[5]),2));

//////////////////////////////////////////////////////////////////////////////////////////
/////////////////// tell pipi from ee or mumu using their energy  ////////////////////////
/////////////////// pipi: |e| < 1.0 , mumu or ee: |e| > 1.0 //////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

   unmatchedtracks = 0;
   int id_sum = 0;

   for ( FATable< NavTrack >::const_iterator trackItr = trackTableBegin;
         trackItr != trackTableEnd ;
         ++trackItr )
   {
      int i = unmatchedtracks++;
      PE[i]=(*trackItr).pionFit()->lorentzMomentum().e();

      if(trackItr->trackShowerMatch().valid())
         ME[i] = trackItr->trackShowerMatch()->matchedEnergy();

      EOP[i] = trackItr->elecId().eOverP();
      FAItem<DedxInfo> the_dedx = (*trackItr).dedxInfo();
      if(PE[i] < 1.0) {
        FAItem<TDKinematicFit> pionFit = (*trackItr).pionFit();
        if( (pionFit.valid()) && (the_dedx.valid()) )  {
          ID[i] = 0;
          PE[i]=pionFit->energy();
          PX[i]=pionFit->px();
          PY[i]=pionFit->py();
          PZ[i]=pionFit->pz();
          DEDX[i]=the_dedx->piSigma();
        }
      }
      else if(PE[i]>= 1.0){
        if(EOP[i] > 0.8 && EOP[i] < 1.2) {
          FAItem<TDKinematicFit> electronFit = (*trackItr).electronFit();

          FATable<NavShower> brem(trackItr->bremShowers());
 
            HepDouble gamsx = 0., gamsy = 0., gamsz = 0.;

            for(FATable<NavShower>::const_iterator iGam=brem.begin();
                iGam != brem.end(); ++iGam) {
               gamsx += iGam->attributes().momentum().x();
               gamsy += iGam->attributes().momentum().y();
               gamsz += iGam->attributes().momentum().z();
            }
          if( (electronFit.valid()) && (the_dedx.valid()) )  {
            ID[i] = 1;
            PE[i]=sqrt(fabs( 0.000511*0.000511
                        + (electronFit->px()+gamsx)*(electronFit->px()+gamsx)
                        + (electronFit->py()+gamsy)*(electronFit->py()+gamsy)
                        + (electronFit->pz()+gamsz)*(electronFit->pz()+gamsz) )); 
            PX[i]=electronFit->px()+gamsx;
            PY[i]=electronFit->py()+gamsy;
            PZ[i]=electronFit->pz()+gamsz;
            DEDX[i]=the_dedx->eSigma();
          }
        }
        else if(EOP[i] < 0.8) {
          FAItem<TDKinematicFit> muonFit = (*trackItr).muonFit();
          if( (muonFit.valid()) && (the_dedx.valid()) )  {
            ID[i] = 2;
            PE[i]=muonFit->energy();
            PX[i]=muonFit->px();
            PY[i]=muonFit->py();
            PZ[i]=muonFit->pz();
            DEDX[i]=the_dedx->muSigma();
          }
        }
      }
      id_sum += ID[i];
   }
   if(id_sum > 0) {
     PASSED2 = 1;
     PEAK2 = sqrt(pow((PE[0]+PE[1]+PE[2]+PE[3]+PE[4]+PE[5]),2)-pow((PX[0]+PX[1]+PX[2]+PX[3]+PX[4]+PX[5]),2)-
                pow((PY[0]+PY[1]+PY[2]+PY[3]+PY[4]+PY[5]),2)-pow((PZ[0]+PZ[1]+PZ[2]+PZ[3]+PZ[4]+PZ[5]),2));
   }

////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////// kinematic fitting  ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

   if(PASSED2) {

     FAItem<TDKinematicFit> tr[6];
     KTKinematicData track[6];
     unmatchedtracks = 0;

     for ( FATable< NavTrack >::const_iterator trackItr = trackTableBegin;
           trackItr != trackTableEnd ;
           ++trackItr )
       {
          int i = unmatchedtracks++;

          if(ID[i] == 0) {
            tr[i] = (*trackItr).pionFit();
            track[i] = *tr[i];
          }
          else if(ID[i] == 1) {
            tr[i] = (*trackItr).electronFit();
            track[i] = *tr[i];

            FATable<NavShower> brem(trackItr->bremShowers());
 
            HepDouble gamsx = 0., gamsy = 0., gamsz = 0.;
            for(FATable<NavShower>::const_iterator iGam=brem.begin();
                iGam != brem.end(); ++iGam) {
               gamsx += iGam->attributes().momentum().x();
               gamsy += iGam->attributes().momentum().y();
               gamsz += iGam->attributes().momentum().z();
            }
            track[i].setMomentum(HepVector3D(tr[i]->px()+gamsx,tr[i]->py()+gamsy,tr[i]->pz()+gamsz));
          }
          else if(ID[i] == 2) {
            tr[i] = (*trackItr).muonFit();
            track[i] = *tr[i];
          }
       }

////////////////////////////////// 4C fit /////////////////////////////////////////////////////////////////////


   if(tr[0].valid() && tr[1].valid() && tr[2].valid() && tr[3].valid() && tr[4].valid() && tr[5].valid()) {

      // do the fit

      FitEvt pipitrtr1( "Pipitrtr1", FitEvt::k_P4VecCM );
      pipitrtr1.newTrack(track[0]);
      pipitrtr1.newTrack(track[1]);
      pipitrtr1.newTrack(track[2]);
      pipitrtr1.newTrack(track[3]);
      pipitrtr1.newTrack(track[4]);
      pipitrtr1.newTrack(track[5]);
      pipitrtr1.doTheFit();

      if( (pipitrtr1.chisqVtx()>0) && (pipitrtr1.chisqFit()>0) ) {
        
                 HepLorentzVector v1 (pipitrtr1.kdFitVec()[0]->lorentzMomentum());
                 HepLorentzVector v2 (pipitrtr1.kdFitVec()[1]->lorentzMomentum());
                 HepLorentzVector v3 (pipitrtr1.kdFitVec()[2]->lorentzMomentum());
                 HepLorentzVector v4 (pipitrtr1.kdFitVec()[3]->lorentzMomentum());
                 HepLorentzVector v5 (pipitrtr1.kdFitVec()[4]->lorentzMomentum());
                 HepLorentzVector v6 (pipitrtr1.kdFitVec()[5]->lorentzMomentum());

                    ChisqV1 = pipitrtr1.chisqVtx();
                    ChisqF1 = pipitrtr1.chisqFit();


                    F1PE[0]=v1.e();
                    F1PX[0]=v1.px();
                    F1PY[0]=v1.py();
                    F1PZ[0]=v1.pz();
                    F1PE[1]=v2.e();
                    F1PX[1]=v2.px();
                    F1PY[1]=v2.py();
                    F1PZ[1]=v2.pz();
                    F1PE[2]=v3.e();
                    F1PX[2]=v3.px();
                    F1PY[2]=v3.py();
                    F1PZ[2]=v3.pz();
                    F1PE[3]=v4.e();
                    F1PX[3]=v4.px();
                    F1PY[3]=v4.py();
                    F1PZ[3]=v4.pz();
                    F1PE[4]=v5.e();
                    F1PX[4]=v5.px();
                    F1PY[4]=v5.py();
                    F1PZ[4]=v5.pz();
                    F1PE[5]=v6.e();
                    F1PX[5]=v6.px();
                    F1PY[5]=v6.py();
                    F1PZ[5]=v6.pz();

                      }                 // if fit converge ends
                    }                   // if FAItem.valid ends

      if(ChisqV1 < 1000000 && ChisqF1 < 1000000) {
         PASSED3 = 1;
         PEAK3 = sqrt(pow((F1PE[0]+F1PE[1]+F1PE[2]+F1PE[3]+F1PE[4]+F1PE[5]),2)
                     -pow((F1PX[0]+F1PX[1]+F1PX[2]+F1PX[3]+F1PX[4]+F1PX[5]),2)
                     -pow((F1PY[0]+F1PY[1]+F1PY[2]+F1PY[3]+F1PY[4]+F1PY[5]),2)
                     -pow((F1PZ[0]+F1PZ[1]+F1PZ[2]+F1PZ[3]+F1PZ[4]+F1PZ[5]),2));
      }

   }                                    //  if(PASSED2) ends

   tuple[chisqv1] = ChisqV1;
   tuple[chisqf1] = ChisqF1;
   tuple[eop1] = EOP[0];
   tuple[eop2] = EOP[1];
   tuple[eop3] = EOP[2];
   tuple[eop4] = EOP[3];
   tuple[eop5] = EOP[4];
   tuple[eop6] = EOP[5];
   tuple[me1] = ME[0];
   tuple[me2] = ME[1];
   tuple[me3] = ME[2];
   tuple[me4] = ME[3];
   tuple[me5] = ME[4];
   tuple[me6] = ME[5];
   tuple[dedx1] = DEDX[0];
   tuple[dedx2] = DEDX[1];
   tuple[dedx3] = DEDX[2];
   tuple[dedx4] = DEDX[3];
   tuple[dedx5] = DEDX[4];
   tuple[dedx6] = DEDX[5];
   tuple[trte1] = PE[0];
   tuple[trpx1] = PX[0];
   tuple[trpy1] = PY[0];
   tuple[trpz1] = PZ[0];
   tuple[trte2] = PE[1];
   tuple[trpx2] = PX[1];
   tuple[trpy2] = PY[1];
   tuple[trpz2] = PZ[1];
   tuple[trte3] = PE[2];
   tuple[trpx3] = PX[2];
   tuple[trpy3] = PY[2];
   tuple[trpz3] = PZ[2];
   tuple[trte4] = PE[3];
   tuple[trpx4] = PX[3];
   tuple[trpy4] = PY[3];
   tuple[trpz4] = PZ[3];
   tuple[trte5] = PE[4];
   tuple[trpx5] = PX[4];
   tuple[trpy5] = PY[4];
   tuple[trpz5] = PZ[4];
   tuple[trte6] = PE[5];
   tuple[trpx6] = PX[5];
   tuple[trpy6] = PY[5];
   tuple[trpz6] = PZ[5];
   tuple[f1trte1] = F1PE[0];
   tuple[f1trpx1] = F1PX[0];
   tuple[f1trpy1] = F1PY[0];
   tuple[f1trpz1] = F1PZ[0];
   tuple[f1trte2] = F1PE[1];
   tuple[f1trpx2] = F1PX[1];
   tuple[f1trpy2] = F1PY[1];
   tuple[f1trpz2] = F1PZ[1];
   tuple[f1trte3] = F1PE[2];
   tuple[f1trpx3] = F1PX[2];
   tuple[f1trpy3] = F1PY[2];
   tuple[f1trpz3] = F1PZ[2];
   tuple[f1trte4] = F1PE[3];
   tuple[f1trpx4] = F1PX[3];
   tuple[f1trpy4] = F1PY[3];
   tuple[f1trpz4] = F1PZ[3];
   tuple[f1trte5] = F1PE[4];
   tuple[f1trpx5] = F1PX[4];
   tuple[f1trpy5] = F1PY[4];
   tuple[f1trpz5] = F1PZ[4];
   tuple[f1trte6] = F1PE[5];
   tuple[f1trpx6] = F1PX[5];
   tuple[f1trpy6] = F1PY[5];
   tuple[f1trpz6] = F1PZ[5];
   tuple[id1] = ID[0];
   tuple[id2] = ID[1];
   tuple[id3] = ID[2];
   tuple[id4] = ID[3];
   tuple[id5] = ID[4];
   tuple[id6] = ID[5];
   tuple[charge1] = CHARGE[0];
   tuple[charge2] = CHARGE[1];
   tuple[charge3] = CHARGE[2];
   tuple[charge4] = CHARGE[3];
   tuple[charge5] = CHARGE[4];
   tuple[charge6] = CHARGE[5];
   tuple[passed1] = PASSED1;
   tuple[passed2] = PASSED2;
   tuple[passed3] = PASSED3;
   tuple[peak1] = PEAK1;
   tuple[peak2] = PEAK2;
   tuple[peak3] = PEAK3;

   (*m_showerTuple).fill(tuple);
    





   return ActionBase::kPassed;
}


ActionBase::ActionResult
PipiPsi2SProc::beginRun( Frame& iFrame )       // anal2 equiv.
{
   report( DEBUG, kFacilityString ) << "here in beginRun()" << endl;

   FAItem< BeamSpot > spot;
   extract( iFrame.record( Stream::kBeginRun), spot );

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
PipiPsi2SProc::endRun( Frame& iFrame )         // anal4 equiv.
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
