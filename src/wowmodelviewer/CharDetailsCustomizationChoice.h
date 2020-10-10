/*
* CharDetailsCustomizationChoice.h
*
*  Created on: 27 sep. 2020
*      Author: Jeromnimo
*/

#ifndef _CHARDETAILSCUSTOMIZATIONCHOICE_H_
#define _CHARDETAILSCUSTOMIZATIONCHOICE_H_

#ifndef WX_PRECOMP
  #include <wx/wx.h>
#endif

#include <wx/choice.h>
#include <wx/window.h>

#include "CharDetails.h"

#include "metaclasses/Observer.h"

class CharDetailsCustomizationChoice : public wxWindow, public Observer
{
  public:
    CharDetailsCustomizationChoice(wxWindow* parent, CharDetails & details, uint chrCustomizationChoiceID);

  private:
    DECLARE_CLASS(CharDetailsCustomizationChoice)
    DECLARE_EVENT_TABLE()

    void onChoice(wxCommandEvent& event);
    void onEvent(Event *) override;

    void refresh();
    void buildList();

    uint m_ID;
    std::vector<uint> m_values;

    CharDetails & m_details;
    

    wxChoice * m_choice;
};


#endif // _CHARDETAILSCUSTOMIZATIONCHOICE_H_