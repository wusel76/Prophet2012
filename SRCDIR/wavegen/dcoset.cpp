#include <wx/wx.h>
#include <wx/slider.h>
#include "GUILayout.h"
#include "types.h"
#include "stopwatch.h"
#include "proph2000.h"
#include "dcoset.h"



extern "C" 
{
   #include "types.h"
   #include "wavegenconfig.h"
   #include "synthengine.h"
   #include "dco.h"
   #include "dadsr.h"
};

#include "wavegen.h"


DCOSet::DCOSet(wxWindow *parent, int dco_nr, WaveGen *wavegen)
       :  DCOSetPanel( parent ), m_dco_nr(dco_nr), m_wavegen(wavegen)
{
   m_wave->SetSelection(DCO_dco_set_as[m_dco_nr].wave_e);
   m_vol_wave->SetSelection(DCO_volmod_set_as[m_dco_nr].wave_e);
}

DCOSet::~DCOSet()
{
}

void DCOSet::OnLevelChange(wxScrollEvent& event)
{
   DCO_dco_set_as[m_dco_nr].level_u8 = m_level_slider->GetValue();
   m_wavegen->renderWaveMemory();
}

void DCOSet::OnVolLevelChange(wxScrollEvent& event)
{
   DCO_volmod_set_as[m_dco_nr].level_u8 = m_vol_level_slider->GetValue();
   m_wavegen->renderWaveMemory();
}


void DCOSet::OnVolFreqChange(wxScrollEvent& event)
{
   wxString str;
   DCO_vSetFreq(&DCO_volmod_rt_as[m_dco_nr], &DCO_volmod_set_as[m_dco_nr], m_vol_freq_slider->GetValue(), SAMPLEFREQ);   
   DCO_volmod_rt_as[m_dco_nr].cnt_u32 = 0;
   DCO_dco_rt_as[m_dco_nr].cnt_u32 = 0;
   m_wavegen->renderWaveMemory();
}


void DCOSet::OnDCOChange(wxCommandEvent& event)
{
   DCO_dco_set_as[m_dco_nr].wave_e = (DCO_wave_te)m_wave->GetSelection();
   m_wavegen->renderWaveMemory();
}


void DCOSet::OnVolWaveChange(wxCommandEvent& event)
{
   DCO_volmod_set_as[m_dco_nr].wave_e = (DCO_wave_te)m_vol_wave->GetSelection();
   m_wavegen->renderWaveMemory();
}


void DCOSet::OnTranspose(wxScrollEvent& event)
{
   SYNENG_dco_set_as[m_dco_nr].transpose_i8 = m_transpose_slider->GetValue();
   m_wavegen->renderWaveMemory();
}

void DCOSet::OnFine(wxScrollEvent& event)
{
   SYNENG_dco_set_as[m_dco_nr].tune_i8 = m_fine_slider->GetValue();
   m_wavegen->renderWaveMemory();
}

void DCOSet::OnADSRChange(wxScrollEvent& event)
{
   DADSR_dcoout_data_as[m_dco_nr].attack_time_u16 = m_a_slider->GetValue();
   DADSR_dcoout_data_as[m_dco_nr].decay_time_u16 = m_d_slider->GetValue();
   DADSR_dcoout_data_as[m_dco_nr].sustain_level_u16 = m_s_slider->GetValue();
   DADSR_dcoout_data_as[m_dco_nr].output_level_u8 = m_dadsrout_slider->GetValue();
   m_wavegen->renderWaveMemory();
}


void DCOSet::OnSubOSC(wxScrollEvent& event)
{
   DCO_dco_set_as[m_dco_nr].sub_osc_level_u8 = m_subosc_slider->GetValue();
   m_wavegen->renderWaveMemory();
}


void DCOSet::updateGUI(void)
{
   m_subosc_slider->SetValue(DCO_dco_set_as[m_dco_nr].sub_osc_level_u8);
   m_wave->SetSelection(DCO_dco_set_as[m_dco_nr].wave_e);
   m_vol_wave->SetSelection(DCO_volmod_set_as[m_dco_nr].wave_e);
   m_vol_freq_slider->SetValue(DCO_volmod_set_as[m_dco_nr].freq_mHz_u32);
   m_vol_level_slider->SetValue(DCO_volmod_set_as[m_dco_nr].level_u8);
   m_level_slider->SetValue(DCO_dco_set_as[m_dco_nr].level_u8);
   m_transpose_slider->SetValue(SYNENG_dco_set_as[m_dco_nr].transpose_i8);
   m_fine_slider->SetValue(SYNENG_dco_set_as[m_dco_nr].tune_i8);

   m_a_slider->SetValue(DADSR_dcoout_data_as[m_dco_nr].attack_time_u16);
   m_d_slider->SetValue(DADSR_dcoout_data_as[m_dco_nr].decay_time_u16);
   m_s_slider->SetValue(DADSR_dcoout_data_as[m_dco_nr].sustain_level_u16);
   m_dadsrout_slider->SetValue(DADSR_dcoout_data_as[m_dco_nr].output_level_u8);
}


