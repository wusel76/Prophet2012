#ifndef __DCOSETH__
#define __DCOSETH__

class WaveGen;
class DCOSetPanel;

class DCOSet : public DCOSetPanel
{
public:
    DCOSet(wxWindow *parent, int dco_nr, WaveGen *wavegen);
    ~DCOSet();
        
   void OnLevelChange(wxScrollEvent& event);
   void OnVolLevelChange(wxScrollEvent& event);
   void OnVolFreqChange(wxScrollEvent& event);
   void OnDCOChange(wxCommandEvent& event);
   void OnVolWaveChange(wxCommandEvent& event);
   void OnADSRChange(wxScrollEvent& event);
   void OnTranspose(wxScrollEvent& event);
   void OnFine(wxScrollEvent& event);
   void OnSubOSC(wxScrollEvent& event);

   void updateGUI(void);

private:
   int m_dco_nr;
   WaveGen *m_wavegen;
};


#endif
