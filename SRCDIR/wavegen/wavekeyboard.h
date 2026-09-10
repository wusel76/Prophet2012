#ifndef __WAVEKEYBOARDH__
#define __WAVEKEYBOARDH__

class Keyboard;
class WaveGen;

class WaveKeyboard : public Keyboard
{
public:
    WaveKeyboard(wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, WaveGen *wavegen);
    ~WaveKeyboard();

    void highlightNotes(uint8_t main_note_u8, uint8_t lower_keys, uint8_t upper_keys);

private:
   WaveGen *m_wavegen;
   bool m_inhibit;

   virtual void noteOn(void);
   virtual void noteOff(void);
};


#endif
