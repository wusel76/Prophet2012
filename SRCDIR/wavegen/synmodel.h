#ifndef __SYNMODELH__
#define __SYNMODELH__

class WaveGen;
class SynthModel;

class SynModel : public SynthModel
{
public:
    SynModel(wxWindow *parent, WaveGen *wavegen);
    ~SynModel();
        
    void OnModelChange(wxCommandEvent& event);
    void OnFMAlgChange(wxScrollEvent& event);
    void OnFMFeedbackChange(wxScrollEvent& event);
    void updateGUI(void);

    WaveGen::length_mode_te getLengthMode(void) const;
    void setLengthMode(WaveGen::length_mode_te mode);

    void setSampleRate(Proph2000::sample_rate_te rate);
    Proph2000::sample_rate_te getSampleRate(void) const;
 
private:
   wxWindow *m_parent;   
   WaveGen *m_wavegen;

   void OnLength(wxScrollEvent& event);
   void OnLengthMode(wxCommandEvent &event);
   void OnDestSampleRate(wxCommandEvent &event);

};


#endif
