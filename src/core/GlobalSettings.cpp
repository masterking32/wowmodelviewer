/*----------------------------------------------------------------------*\
| This file is part of WoW Model Viewer                                  |
|                                                                        |
| WoW Model Viewer is free software: you can redistribute it and/or      |
| modify it under the terms of the GNU General Public License as         |
| published by the Free Software Foundation, either version 3 of the     |
| License, or (at your option) any later version.                        |
|                                                                        |
| WoW Model Viewer is distributed in the hope that it will be useful,    |
| but WITHOUT ANY WARRANTY; without even the implied warranty of         |
| MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          |
| GNU General Public License for more details.                           |
|                                                                        |
| You should have received a copy of the GNU General Public License      |
| along with WoW Model Viewer.                                           |
| If not, see <http://www.gnu.org/licenses/>.                            |
\*----------------------------------------------------------------------*/

/*
 * GlobalSettings.cpp
 *
 *  Created on: 26 nov. 2011
 *   Copyright: 2013 , WoW Model Viewer (http://wowmodelviewer.net)
 */

#define _GLOBALSETTINGS_CPP_
#include "GlobalSettings.h"
#undef _GLOBALSETTINGS_CPP_

// Includes / class Declarations
//--------------------------------------------------------------------
// STL
#include <sstream>

// Wx

// Externals

// Other libraries

// Current library
#include "Plugin.h"

// if you need extra qualification on software version, move this to 1
# define _BETAVERSION 0
# define _ALPHAVERSION 1
#ifndef _BUILDNUMBER
  #define _BUILDNUMBER 0
#endif


// Namespaces used
//--------------------------------------------------------------------


// Beginning of implementation
//====================================================================
core::GlobalSettings * core::GlobalSettings::m_instance = 0;

// Constructors 
//--------------------------------------------------------------------
core::GlobalSettings::GlobalSettings()
{
  m_versionMajorNumber = 0;
  m_versionMinorNumber = 10;
  m_versionRevNumber = 0;

  m_appName = L"WoW Model Viewer";
  m_buildName = L"Oribos";

 
  /*
    --==List of Build Name ideas==--  (Feel free to add!)
    Bouncing Baracuda
    Hoppin Jalapeno
    Stealthed Rogue
    Deadly Druid
    Killer Krakken
    Crazy Kaelthas
    Lonely Mastiff
    Cold Kelthuzad
    Jiggly Jaina
    Vashj's Folly
    Epic Win
    Epic Lose
    Lord Kezzak
    Perky Pug
    Great-father Winter


    --== Used Build Names ==--      (So we don't repeat...)
    Wascally Wabbit
    Gnome Punter
    Fickle Felguard
    Demented Deathwing
    Pickled Herring
    Windrunner's Lament
    Lost Lich King
    Skeer the Bloodseeker
    Thrall's revenge
    Trip in Draenor
    Christmas Edition ;)
    Archimonde will survive
    Wain's edition
    Bilgewhizzle
    Oribos

   */

  // platform 
  m_platform = L"Windows 32 bits";

#if _BETAVERSION > 0
  m_isBetaVersion = true;
#else
  m_isBetaVersion = false;
#endif

#if _ALPHAVERSION > 0
  m_isAlphaVersion = true;
#else
  m_isAlphaVersion = false;
#endif

  bShowParticle = false;
  bZeroParticle = false;
  bInitPoseOnlyExport = false;
}

// Destructor
//--------------------------------------------------------------------
core::GlobalSettings::~GlobalSettings()
{

}

// Public methods
//--------------------------------------------------------------------
std::wstring core::GlobalSettings::appVersion(std::wstring a_prefix)
{
  std::wstring l_result = a_prefix;
  std::wstringstream l_oss;
  l_oss.precision(0);

  l_oss << m_versionMajorNumber << "." << m_versionMinorNumber << "."
    << m_versionRevNumber << "." << _BUILDNUMBER;
  l_result += l_oss.str();

  return l_result;
}

std::wstring core::GlobalSettings::appName()
{
  return m_appName;
}

std::wstring core::GlobalSettings::buildName()
{
  return m_buildName;
}

std::wstring core::GlobalSettings::appTitle()
{
  std::wstring title = appName() + appVersion(std::wstring(L" v")) + L" " + m_platform;
  if(m_isBetaVersion)
    title += L" - BETA VERSION";
  if(m_isAlphaVersion)
    title += L" - ALPHA VERSION";
  return title;
}

// Protected methods
//--------------------------------------------------------------------


// Private methods
//--------------------------------------------------------------------
