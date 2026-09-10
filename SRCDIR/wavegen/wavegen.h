#ifndef __WAVEGENH__
#define __WAVEGENH__

class WaveDisp;
class DCOSet;
class SynModel;
class wxNotebook;
class WaveKeyboard;
class Proph2000;
class MyPanel;
class Audio;
class wxToggleButton;

// Define a new frame type
class WaveGen: public wxDialog
{
public:
   typedef enum { FIXED_LENGTH = 0, DCO1_INTERVAL, DCO2_INTERVAL, DCO3_INTERVAL, DCO4_INTERVAL } length_mode_te;
    WaveGen(wxWindow* parent, wxWindowID id, uint8_t sound, Proph2000 *proph, MyPanel *main, Audio *audio, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE, const wxString& name = "frame");
   ~WaveGen();


   void loadConfig(FILE *file);
   void saveConfig(FILE *file);

   void setSound(uint8_t sound);

   void setNote(uint8_t note_u8);

   uint32_t getSampleWords(void) const { return m_sample_words; };
   void setSampleWords(uint32_t words);

   void setSampleRate(Proph2000::sample_rate_te rate);

   void setLengthMode(length_mode_te mode);

   void renderWaveMemory(void);

   void updateGUI(void);

private:
   WaveDisp *m_wavedisp;
   uint16_t *m_sample_data_pau16;
   uint32_t m_sample_words;
   Proph2000 *m_proph;
   wxNotebook *m_notebook;
   uint8_t m_note_u8;
   MyPanel *m_main;
   WaveKeyboard *m_wavekeyboard;
   uint8_t m_sound;
   Audio *m_audio;
   wxToggleButton *m_play_button;
   length_mode_te m_length_mode;
   Proph2000::sample_rate_te m_sample_rate;

   SynModel *m_syn_model;
   DCOSet *m_dco_set[DCO_MAX_DCOS];

   void normalize(void);

   void resetSynth(void);

   wxPanel *createDCOPanel(wxWindow *parent, uint8_t dco);
   wxPanel *createSynModelPanel(wxWindow *parent);

   void OnExport(wxCommandEvent& event);
   void OnPlay(wxCommandEvent& event);

   void startPlayback(void);
   void stopPlayback(void);

   wxString getNote(uint8_t number_u8);

   DECLARE_EVENT_TABLE()
};


#endif

