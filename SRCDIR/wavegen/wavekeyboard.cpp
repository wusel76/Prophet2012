#include "types.h"

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/file.h>
#include <wx/spinctrl.h>
#ifndef __WXMAC__
   #include <windows.h>
#endif
#include <list>
#include <map>
using namespace std;


extern "C" 
{
   #include "types.h"
   #include "wavegenconfig.h"
   #include "synthengine.h"
   #include "dco.h"
   #include "dadsr.h"
};

#include "stopwatch.h"
#include "proph2000.h"
#include "wavegen.h"
#include "clickarea.h"
#include "keyboard.h"
#include "wavekeyboard.h"


WaveKeyboard::WaveKeyboard(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, WaveGen *wavegen) :
   Keyboard(parent, id, pos, size), m_wavegen(wavegen)
{
}


WaveKeyboard::~WaveKeyboard()
{
}


void WaveKeyboard::noteOn(void)
{
   m_wavegen->setNote(m_note);
}


void WaveKeyboard::noteOff(void)
{
}

void WaveKeyboard::highlightNotes(uint8_t main_note_u8, uint8_t lower_keys, uint8_t upper_keys)
{
   uint8_t i;
   resetKeyboardColours();
   wxColour keyct;
   wxColour keycol;

   keycol = *wxRED;
   keyct = keycol;
   setUpperNoteColour(main_note_u8, keycol);
   setLowerNoteColour(main_note_u8, keycol);

   keycol = wxColour(255, 210, 210);

   for(i = 0; i < lower_keys; i++)
   {
      int16_t i2;
      i2 = (int16_t)main_note_u8 - i - 1;

      if(i2 > 0)
      {
         keyct = keycol;
         if(isBlackKey(main_note_u8 - i - 1))
         {
            wxColour dim = SOUND_BLACK_KEY_COLOUR_DIM;
            int16_t r;
            int16_t g;
            int16_t b;
      
            r = keyct.Red() - dim.Red();
            g = keyct.Green() - dim.Green();
            b = keyct.Blue() - dim.Blue();

            if(r < 0)
            {
               r = 0;
            }
            if(g < 0)
            {
               g = 0;
            }
            if(b < 0)
            {
               b = 0;
            }

            keyct = wxColour(r, g, b);
         }
         setUpperNoteColour(main_note_u8 - i - 1, keyct);
         setLowerNoteColour(main_note_u8 - i - 1, keyct);
      }
   }

   for(i = 0; i < upper_keys; i++)
   {
      keyct = keycol;
      if(isBlackKey(main_note_u8 + i + 1))
      {
         wxColour dim = SOUND_BLACK_KEY_COLOUR_DIM;
         int16_t r;
         int16_t g;
         int16_t b;
   
         r = keyct.Red() - dim.Red();
         g = keyct.Green() - dim.Green();
         b = keyct.Blue() - dim.Blue();

         if(r < 0)
         {
            r = 0;
         }
         if(g < 0)
         {
            g = 0;
         }
         if(b < 0)
         {
            b = 0;
         }

         keyct = wxColour(r, g, b);
      }
      setUpperNoteColour(main_note_u8 + i + 1, keyct);
      setLowerNoteColour(main_note_u8 + i + 1, keyct);
   }

   Refresh(true);
}


