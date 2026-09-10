#ifndef _CFGDLGH__
#define _CFGDLGH__

class MIDI;
class MidiConfigDlg;
class Proph2000;
class Audio;
class MyFrame;

class CfgDlg : public ConfigDlg
{
public:
   CfgDlg(wxWindow* parent, wxWindowID id, MIDI *midi, Proph2000 *proph, Audio *audio, MyFrame *mainframe, const wxPoint& pos = wxDefaultPosition);
   ~CfgDlg();

   void updateGUI(void);

private:
   MIDI *m_midi;
   Proph2000 *m_proph;
   Audio *m_audio;
   MyFrame *m_mainframe;

   void OnInDevice(wxCommandEvent &event);
   void OnOutDevice(wxCommandEvent &event);
   void OnChannel(wxSpinEvent &event);
   void OnUseBurstHW(wxCommandEvent &event);
   void OnBurstAlways(wxCommandEvent &event);
   void OnBurstHWCOM(wxCommandEvent &event);
   void OnBurstSpeed(wxScrollEvent &event);
   void OnAudioDevice(wxCommandEvent &event);

   void OnFontSize(wxCommandEvent &event);

};


#endif
