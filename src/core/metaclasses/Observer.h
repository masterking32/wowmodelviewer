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
 * Observer.h
 *
 *  Created on: 3 january 2015
 *   Copyright: 2015 , WoW Model Viewer (http://wowmodelviewer.net)
 */

#ifndef _OBSERVER_H_
#define _OBSERVER_H_

// Includes / class Declarations
//--------------------------------------------------------------------
// STL
#include <list>
// Qt

// Externals

// Other libraries

// Current library
class Event;
class Observable;

// Namespaces used
//--------------------------------------------------------------------


// Class Declaration
//--------------------------------------------------------------------
class Observer
{
  public :
    // Constants / Enums

    // Constructors
    Observer();

    // Destructors
    virtual ~Observer();

    // Methods
    virtual void onDestroyEvent() {}
    virtual void onEvent(Event *) = 0;

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
    void treatEvent(Event *);
    void addObservable(Observable *);
    void removeObservable(Observable *);

    std::list<Observable *>::iterator findObservable(Observable *);

    // Members
    std::list<Observable *> m_observableList;

    // friend class declarations
    friend class Observable;

};

// static members definition
#ifdef _OBSERVER_CPP_

#endif

#endif /* _OBSERVER_H_ */
