#include <wx/wx.h>
#include <wx/slider.h>
#include <wx/spinbutt.h>
#include <wx/spinctrl.h>

#include "GUILayout.h"


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
#include "synmodel.h"



SynModel::SynModel(wxWindow *parent, WaveGen *wavegen)
       :  SynthModel( parent ), m_parent(parent), m_wavegen(wavegen)
{
#ifdef __WXMAC__ // 130205
    m_fm_alg->ToggleWindowStyle(wxBORDER_SUNKEN);
    m_fm_feedback_slider->ToggleWindowStyle(wxBORDER_SUNKEN);
    m_sample_words->ToggleWindowStyle(wxBORDER_SUNKEN);  
#endif
   m_model->SetSelection(DCO_synmode_e);

   m_dest_sample_rate->SetSelection(2);
   m_length_mode->AppendString(wxT("fixed length"));
   m_length_mode->AppendString(wxT("DCO1 interval"));
   m_length_mode->AppendString(wxT("DCO2 interval"));
   m_length_mode->AppendString(wxT("DCO3 interval"));
   m_length_mode->AppendString(wxT("DCO4 interval"));
   m_length_mode->SetSelection(0);
}


SynModel::~SynModel()
{
}


void SynModel::OnDestSampleRate(wxCommandEvent &event)
{
   m_wavegen->setSampleRate((Proph2000::sample_rate_te)m_dest_sample_rate->GetSelection());
}


void SynModel::setSampleRate(Proph2000::sample_rate_te rate)
{
   m_dest_sample_rate->SetSelection(rate);
}


Proph2000::sample_rate_te SynModel::getSampleRate(void) const
{
   return (Proph2000::sample_rate_te)m_dest_sample_rate->GetSelection();
}


void SynModel::OnLengthMode(wxCommandEvent &event)
{
   m_wavegen->setLengthMode((WaveGen::length_mode_te)m_length_mode->GetSelection());
   if(m_length_mode->GetSelection() == WaveGen::FIXED_LENGTH)
   {
      m_sample_words->Enable();
   }
   else
   {
      m_sample_words->Disable();
   }
}


WaveGen::length_mode_te SynModel::getLengthMode(void) const
{ 
   return (WaveGen::length_mode_te)m_length_mode->GetSelection(); 
}


void SynModel::setLengthMode(WaveGen::length_mode_te mode)
{
   m_length_mode->SetSelection(mode); 
   if(mode == WaveGen::FIXED_LENGTH)
   {
      m_sample_words->Enable();
   }
   else
   {
      m_sample_words->Disable();
   }
}


void SynModel::OnLength(wxScrollEvent& event)
{
   m_wavegen->setSampleWords(m_sample_words->GetValue());
}


void SynModel::OnModelChange(wxCommandEvent& event)
{
   DCO_synmode_e = (DCO_synmode_te)m_model->GetSelection();
   m_wavegen->renderWaveMemory();
}

void SynModel::OnFMAlgChange(wxScrollEvent& event)
{
   DCO_fm_algorithm_u8 = m_fm_alg->GetValue();
   m_wavegen->renderWaveMemory();
}

void SynModel::OnFMFeedbackChange(wxScrollEvent& event)
{
   DCO_fm_feedback_u8 = m_fm_feedback_slider->GetValue();
   m_wavegen->renderWaveMemory();
}


void SynModel::updateGUI(void)
{
   m_model->SetSelection(DCO_synmode_e);
   m_fm_alg->SetValue(DCO_fm_algorithm_u8);
   m_fm_feedback_slider->SetValue(DCO_fm_feedback_u8);
   m_sample_words->SetValue(m_wavegen->getSampleWords());
}

