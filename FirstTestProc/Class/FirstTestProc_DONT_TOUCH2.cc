// -*- C++ -*-
//
// Package:     FirstTestProc
// Module:      FirstTestProc_DONT_TOUCH2
// 
// Description: DONT TOUCH THIS FILE!
//
//              Factory method to create processor FirstTestProc:
//              creates a new FirstTestProc instance each time 
//              it is called; it is used by Suez 
//              to create a Processor after loading in 
//              the code from a shared library.
//
// Implementation:
//
// Author:      tty7
// Created:     Wed Jun  3 16:52:23 CDT 2009
// $Id$
//
// Revision history
//
// $Log$
 
#include "Experiment/Experiment.h"

// system include files
#if defined(AMBIGUOUS_STRING_FUNCTIONS_BUG)
#include <string>
#endif             

// user include files
#include "FirstTestProc/FirstTestProc.h"

// STL classes
#include <string>

//
// constants, enums and typedefs
//
// ---- cvs-based strings (Id and Tag with which file was checked out)
static const char* const kIdString  = "$Id: processor_DONT_TOUCH2.cc,v 1.4 1998/12/01 21:11:59 mkl Exp $";
static const char* const kTagString = "$Name: v07_03_00 $";

//
// function definitions
//

extern "C" {
   Processor* makeProcessor( void );
   const char* versionString( void );
}

Processor*
makeProcessor( void )
{
   return new FirstTestProc;
}

const char*
versionString( void )
{
   return kTagString;
}

