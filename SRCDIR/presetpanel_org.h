#ifndef __PRESETPANELH__
#define __PRESETPANELH__


class MyPanel;
class Proph2000;
class PresetKeyboard;
class MyPresetParam;


class PresetPanelImpl : public PresetPanel
{
public:
    PresetPanelImpl(wxWindow *parent, MyPanel *topframe, Proph2000 *proph, uint8_t preset_u8, MyPresetParam *presetparam);
    ~PresetPanelImpl();

    void updateGUI(void);
    void activatePreset(uint8_t preset_u8);

    //void transferStarted(void);
    //void transferEnded(void);

private:
   MyPanel *m_topframe;
   Proph2000 *m_proph;
   uint8_t m_preset_u8;
   MyPresetParam *m_presetparam;

   void OnParamChange(wxCommandEvent &event);
   void OnLeftMapSelect(wxCommandEvent &event);
   void OnRightMapSelect(wxCommandEvent &event);
   void OnSliderChange(wxScrollEvent &event);
   void OnComboChanged(wxCommandEvent &event);
   void OnCheckboxChange(wxCommandEvent& event);
   void OnSplitPoint(wxCommandEvent& event);
   void OnArpSplitPoint(wxCommandEvent& event);

   bool convertStringToNote(wxString str, uint8_t *note_pu8);

   void OnLoad(wxCommandEvent& event);
   void OnSave(wxCommandEvent& event);
   void OnCopy(wxCommandEvent& event);

   void OnSelectPreset1(wxCommandEvent& event);
   void OnSelectPreset2(wxCommandEvent& event);
   void OnSelectPreset3(wxCommandEvent& event);
   void OnSelectPreset4(wxCommandEvent& event);
   void OnSelectPreset5(wxCommandEvent& event);
   void OnSelectPreset6(wxCommandEvent& event);
   void OnSelectPreset7(wxCommandEvent& event);
   void OnSelectPreset8(wxCommandEvent& event);
   void OnSelectPreset9(wxCommandEvent& event);
   void OnSelectPreset10(wxCommandEvent& event);
   void OnSelectPreset11(wxCommandEvent& event);
   void OnSelectPreset12(wxCommandEvent& event);

   void deselectAllPresetButtons(void);

   wxString getNote(uint8_t number_u8);
};


#endif
