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
 * CharInfos.h
 *
 *  Created on: 26 nov. 2013
 *   Copyright: 2013 , WoW Model Viewer (http://wowmodelviewer.net)
 */

#ifndef _CHARINFOS_H_
#define _CHARINFOS_H_

// Includes / class Declarations
//--------------------------------------------------------------------
// STL
#include <string>
#include <vector>

// Qt

// Externals

// Other libraries

// Current library


// Namespaces used
//--------------------------------------------------------------------


// Class Declaration
//--------------------------------------------------------------------
class CharInfos
{
  public :
    // Constants / Enums

    // Constructors
    CharInfos();

    // Destructors

    // Methods

    // Members
    bool valid;
    bool customTabard;

    unsigned int raceId;
    std::string gender;
    bool hasTransmogGear;

    unsigned int eyeGlowType;

    int tabardIcon;
    int iconColor;
    int tabardBorder;
    int borderColor;
    int background;

    std::vector< std::pair<unsigned int, unsigned int> > customizations; // vector<pair<optionId, choiceId>>

    // TODO refactor this part to associate slots, id and level
    std::vector<int> equipment;
    std::vector<int> itemModifierIds;

  protected :
    // Constants / Enums

    // Constructors

    // Destructors

    // Methods

    // Members

  private :
    // Constants / Enums

    // Constructors

    // Destructors

    // Methods

    // Members

    // friend class declarations

};

// static members definition
#ifdef _CHARINFOS_CPP_

#endif

#endif /* _CHARINFOS_H_ */
