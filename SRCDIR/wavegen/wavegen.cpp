#include <iostream>
#include <string>
#include <map>
#include <list>
using namespace std;

#include <stdio.h>


#include "wx/wx.h"
#include "wx/string.h"
#include "wx/file.h"
#include <wx/notebook.h>
#include <wx/tglbtn.h>

extern "C" 
{
   #include "types.h"
   #include "wavegenconfig.h"
   #include "synthengine.h"
   #include "dco.h"
   #include "dadsr.h"
};

#include "GUILayout.h"

#include "main.h"
#include "clickarea.h"

#include "wavedisp.h"
#include "stopwatch.h"
#include "proph2000.h"
#include "wavegen.h"
#include "DCOset.h"
#include "synmodel.h"
#include "keyboard.h"
#include "wavekeyboard.h"
#include "audio.h"

typedef enum
{
   GUI_EXPORT = 200000,
   GUI_PLAY
} GUI_ID_te;


BEGIN_EVENT_TABLE(WaveGen, wxDialog)
   EVT_BUTTON(GUI_EXPORT, WaveGen::OnExport)
   EVT_TOGGLEBUTTON(GUI_PLAY, WaveGen::OnPlay)
END_EVENT_TABLE()

uint16_t wavegen_sample_rate;


WaveGen::WaveGen(wxWindow* parent, wxWindowID id, uint8_t sound, Proph2000 *proph, MyPanel *main, Audio *audio, const wxPoint& pos, 
                 const wxSize& size, long style, const wxString& name) :
   wxDialog(parent, id, "", pos, size, style, name),
   m_sample_words(1000), m_note_u8(36), m_sound(sound), m_proph(proph), m_main(main), m_audio(audio),
   m_length_mode(FIXED_LENGTH), m_sample_rate(Proph2000::SR_41667HZ)
{
   wavegen_sample_rate = 41667;

   setSound(sound);

   wxBoxSizer *vsizer = new wxBoxSizer( wxVERTICAL );

   vsizer->AddSpacer(10);

   wxBoxSizer *hsizer = new wxBoxSizer( wxHORIZONTAL );

   hsizer->AddSpacer(5);

   m_sample_data_pau16 = new(uint16_t[MAX_SAMPLE_BANK_WORDS_EXTENDED]);

   m_wavedisp = new WaveDisp(this, wxID_ANY, wxDefaultPosition, wxSize(size.GetWidth() - 15, 200));
   m_wavedisp->setSampleData(m_sample_data_pau16);
   hsizer->Add(m_wavedisp);

   vsizer->Add(hsizer);
   vsizer->AddSpacer(10);

   DADSRH_vInit();
   DCO_vInit();
   SYNENG_vInit();

   hsizer = new wxBoxSizer( wxHORIZONTAL );
   hsizer->AddSpacer(5);

#ifdef __WXMAC__
   m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(parent->GetSize().GetWidth(), 240 + 30));
#else
   m_notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxSize(parent->GetSize().GetWidth(), 240));
#endif
   m_notebook->AddPage(createSynModelPanel(m_notebook), wxT("SynModel"));
   m_notebook->AddPage(createDCOPanel(m_notebook, 0), wxT("DCO 1"));
   m_notebook->AddPage(createDCOPanel(m_notebook, 1), wxT("DCO 2"));
   m_notebook->AddPage(createDCOPanel(m_notebook, 2), wxT("DCO 3"));
   m_notebook->AddPage(createDCOPanel(m_notebook, 3), wxT("DCO 4"));
   
   hsizer->Add(m_notebook);
   hsizer->AddSpacer(5);
   vsizer->Add(hsizer);
   vsizer->AddSpacer(10);

   hsizer = new wxBoxSizer( wxHORIZONTAL );
   hsizer->AddSpacer(5);
   m_play_button = new wxToggleButton(this, GUI_PLAY, wxT("Play"), wxDefaultPosition, wxSize(90, wxDefaultSize.GetHeight()));
   hsizer->Add(m_play_button);
   hsizer->AddSpacer(10);
   wxButton *m_export_button = new wxButton(this, GUI_EXPORT, wxT("Export"), wxDefaultPosition, wxSize(90, wxDefaultSize.GetHeight()));
   hsizer->Add(m_export_button);
   vsizer->Add(hsizer);
   vsizer->AddSpacer(10);

   hsizer = new wxBoxSizer( wxHORIZONTAL );
   hsizer->AddSpacer(5);

   m_wavekeyboard = new WaveKeyboard(this, wxID_ANY, wxDefaultPosition, wxSize(size.GetWidth() - 10, 150), this);
   hsizer->Add(m_wavekeyboard);
   vsizer->Add(hsizer);

   SetSizer(vsizer);

   setNote(m_note_u8);

   //DCO_vSetFreq(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], 20000 * 1000, DCO_SAMPLE_FREQ);
   //DCO_freqmod_set_as[0].level_u8 = DCO_MAX_LEVEL;
}


WaveGen::~WaveGen()
{
   for(int i = 0; i < DCO_MAX_DCOS; i++)
   {
      delete m_dco_set[i];
   }

   delete m_wavedisp;

   delete m_sample_data_pau16;
}


void WaveGen::OnExport(wxCommandEvent& event)
{
   uint16_t samplerate;
   stopPlayback();

   switch(m_sample_rate)
   {
      case Proph2000::SR_15625HZ:
         samplerate = 15625;
         break;
      case Proph2000::SR_31250HZ:
         samplerate = 31250;
         break;
      case Proph2000::SR_41667HZ:
         samplerate = 41667;
         break;
      default:
         samplerate = 41667;
         break;
   }

   wxString name;
   name = "WaveGen ";
   name += getNote(m_note_u8);

   m_proph->setSampleName(name.char_str(), m_sound);

   m_proph->setSampleData(m_sound, m_sample_data_pau16, m_sample_words, samplerate);
   m_proph->m_sound_param_as[m_sound].val_s.sample_status |= 0x02;
   m_proph->m_sound_param_as[m_sound].val_s.root_key = m_note_u8;
   m_proph->setReleaseLoopStart(m_sound, 1);
   m_proph->setReleaseLoopEnd(m_sound, m_sample_words - 1);
   
   m_proph->updateStructsFromParamBytes();

   m_main->updateGUI();
   m_main->resetWaveViewZoom();
   m_main->updateWaveView();
   Close();
}

wxString WaveGen::getNote(uint8_t number_u8)
{
   wxString retval;
   wxString t;

   switch(number_u8 % 12)
   {
      case 0:
         retval = wxT("C");
         break;
      case 1:
         retval = wxT("C#");
         break;
      case 2:
         retval = wxT("D");
         break;
      case 3:
         retval = wxT("D#");
         break;
      case 4:
         retval = wxT("E");
         break;
      case 5:
         retval = wxT("F");
         break;
      case 6:
         retval = wxT("F#");
         break;
      case 7:
         retval = wxT("G");
         break;
      case 8:
         retval = wxT("G#");
         break;
      case 9:
         retval = wxT("A");
         break;
      case 10:
         retval = wxT("A#");
         break;
      case 11:
         retval = wxT("B");
         break;
   }

   t.sprintf(wxT("%d"), (number_u8 - 24) / 12);
   retval += t;

   return retval;
}

void WaveGen::OnPlay(wxCommandEvent& event)
{
   if(m_play_button->GetValue())
   {
      startPlayback();
   }
   else
   {
      stopPlayback();
   }
}


void WaveGen::startPlayback(void)
{
   uint16_t samplerate;
   switch(m_sample_rate)
   {
      case Proph2000::SR_15625HZ:
         samplerate = 15625;
         break;
      case Proph2000::SR_31250HZ:
         samplerate = 31250;
         break;
      case Proph2000::SR_41667HZ:
         samplerate = 41667;
         break;
      default:
         samplerate = 41667;
         break;
   }

   m_audio->setAudioData(m_sample_data_pau16, m_sample_words);
   m_audio->play(true, samplerate);
}


void WaveGen::stopPlayback(void)
{
   m_play_button->SetValue(false);
   m_audio->stop();
}


void WaveGen::setSound(uint8_t sound)
{
   wxString str;
   str.sprintf(wxT("Wave Generator (Sound %d)"), sound + 1);
   SetTitle(str);
   m_sound = sound;
}


void WaveGen::setSampleRate(Proph2000::sample_rate_te rate)
{
   m_sample_rate = rate;

   switch(m_sample_rate)
   {
      case Proph2000::SR_15625HZ:
         wavegen_sample_rate = 15625;
         break;
      case Proph2000::SR_31250HZ:
         wavegen_sample_rate = 31250;
         break;
      case Proph2000::SR_41667HZ:
         wavegen_sample_rate = 41667;
         break;
      default:
         wavegen_sample_rate = 41667;
         break;
   }

   setNote(m_note_u8);
}


void WaveGen::setNote(uint8_t note_u8)
{
   m_note_u8 = note_u8;

   switch(m_sample_rate)
   {
      case Proph2000::SR_15625HZ:
         m_wavekeyboard->highlightNotes(m_note_u8, 18, 24);
         break;
      case Proph2000::SR_31250HZ:
         m_wavekeyboard->highlightNotes(m_note_u8, 18, 12);
         break;
      case Proph2000::SR_41667HZ:
         m_wavekeyboard->highlightNotes(m_note_u8, 23, 7);
         break;
      default:
         break;
   }

   renderWaveMemory();
}


wxPanel* WaveGen::createSynModelPanel(wxWindow *parent)
{
   wxPanel *retval;
   wxWindow *child_parent;

	retval = new wxPanel(parent , wxID_ANY /*, wxDefaultPosition,wxSize(240, 320) */ );
   child_parent = retval;

   wxBoxSizer *vsizer = new wxBoxSizer( wxVERTICAL );

   m_syn_model = new SynModel(child_parent, this);
   vsizer->Add(m_syn_model);
 
   retval->SetSizer( vsizer );

   return retval;
}


wxPanel* WaveGen::createDCOPanel(wxWindow *parent, uint8_t dco)
{
   wxPanel *retval;
   wxWindow *child_parent;

	retval = new wxPanel(parent , wxID_ANY /*, wxDefaultPosition,wxSize(240, 320) */ );
   child_parent = retval;

   wxBoxSizer *vsizer = new wxBoxSizer( wxVERTICAL );

   m_dco_set[dco] = new DCOSet(child_parent, dco, this);
   vsizer->Add(m_dco_set[dco]);

   retval->SetSizer( vsizer );

   return retval;
}


void WaveGen::updateGUI(void)
{
   m_syn_model->updateGUI();
   for(int i = 0; i < DCO_MAX_DCOS; i++)
   {
      m_dco_set[i]->updateGUI();
   }
}

void WaveGen::normalize(void)
{
   uint32_t i;
   int16_t t;
   int16_t min;
   int16_t max;
   float scale;
   
   min = 0;
   max = 0;

   for(i = 0; i < m_sample_words; i++)
   {
      t = m_sample_data_pau16[i] - (4096 / 2);
      if(t < min)
      {
         min = t;
      }
      if(t > max)
      {
         max = t;
      }
   }
   //wxLogMessage("min:%d max:%d", min, max);
   min = -min;
   if(min > max)
   {
      max = min;
   }
   if(max)
   {
      scale = ((float)4095 / 2) / (float)max;
      //wxLogMessage("normalize scale: %f", scale);
      for(i = 0; i < m_sample_words; i++)
      {
         t = m_sample_data_pau16[i] - (4096 / 2);
         t = t * scale;
         m_sample_data_pau16[i] = t + (4096 / 2);
      }
   }
}

void WaveGen::setSampleWords(uint32_t words)
{
   m_sample_words = words;
   renderWaveMemory();
}


void WaveGen::setLengthMode(length_mode_te mode)
{
   m_length_mode = mode;
   renderWaveMemory();
}


void WaveGen::resetSynth(void)
{
   uint32_t i;
   for(i = 0; i < DCO_MAX_DCOS; i++)
   {
      DCO_dco_rt_as[i].cnt_u32 = 0; // restart all DCOs
      DCO_dco_rt_as[i].period_cnt_u32 = 0;
      DCO_volmod_rt_as[i].cnt_u32 = 0;
      DCO_volmod_rt_as[i].period_cnt_u32 = 0;

      DADSR_dcoout_rt_as[i].DADSR_time_1ms_u16 = 0;
      DADSR_dcoout_rt_as[i].state_e = DADSR_ATTACK;
      //DADSR_dcoout_rt_as[i].DADSR_output_u16 = 0;

      DADSR_pwmmod_rt_as[i].DADSR_time_1ms_u16 = 0;
      DADSR_pwmmod_rt_as[i].state_e = DADSR_ATTACK;
      //DADSR_pwmmod_rt_as[i].DADSR_output_u16 = 0;

      DADSR_freqmod_rt_as[i].DADSR_time_1ms_u16 = 0;
      DADSR_freqmod_rt_as[i].state_e = DADSR_ATTACK;
      //DADSR_freqmod_rt_as[i].DADSR_output_u16 = 0;
   }

   DADSR_vcamod_rt_s.DADSR_time_1ms_u16 = 0;
   DADSR_vcamod_rt_s.state_e = DADSR_ATTACK;
   //DADSR_vcamod_rt_s.DADSR_output_u16 = 0;

   SYNENG_sample_in_idx_u16 = 0;
   SYNENG_sample_out_idx_u16 = 0;

   SYNENG_sample_in_idx_u16 = 0;
   SYNENG_sample_out_idx_u16 = 0;
   SYNENG_calc_ctrl_cnt_u16 = 0;
}


void WaveGen::renderWaveMemory(void)
{
   uint32_t i;

#if 0
   SYNENG_vKeyOff(m_note_u8);

   for(i = 0; i < 100000; i++)
   {
      SYNENG_vHandler();
   }
#endif

   resetSynth();

//   float d;
   float o = 0;
//   float p;
   uint8_t l = 1;

   switch(m_length_mode)
   {
      case FIXED_LENGTH:
         break;
      case DCO1_INTERVAL:
            SYNENG_vKeyOn(m_note_u8, 127);
            //wxLogMessage("cnt:%f", DCO_dco_rt_as[0].new_period_cnt_u32);
            l = 1;

#if 0
            while(abs( ((DCO_dco_rt_as[0].new_period_cnt_u32 + 0) * l) - (uint32_t)((DCO_dco_rt_as[0].new_period_cnt_u32 + 0) * l) ) >= 0.1)
            {
               l++;
            }
            //wxLogMessage("l:%d", l);
#endif


#if 0
            p = (DCO_dco_rt_as[0].new_period_cnt_u32 + 0) * l;

            d = p - (uint32_t)p;
            wxLogMessage("%f", d);
            if(d >= 0.5)
            {
               //o = 4;
            }
            else
            {
               //o = -1;
            }
            //o = d * 2;
            //l = 4;
#endif


            m_sample_words = (uint32_t)((((DCO_dco_rt_as[0].new_period_cnt_u32)) * l) + o) + 2;
         break;
      case DCO2_INTERVAL:
            SYNENG_vKeyOn(m_note_u8, 127);
            //wxLogMessage("cnt:%f", DCO_dco_rt_as[1].new_period_cnt_u32);
            m_sample_words = ((uint32_t)DCO_dco_rt_as[1].new_period_cnt_u32) * 4;
         break;
      case DCO3_INTERVAL:
            SYNENG_vKeyOn(m_note_u8, 127);
            //wxLogMessage("cnt:%f", DCO_dco_rt_as[2].new_period_cnt_u32);
            m_sample_words = ((uint32_t)DCO_dco_rt_as[2].new_period_cnt_u32) * 4;
         break;
      case DCO4_INTERVAL:
            SYNENG_vKeyOn(m_note_u8, 127);
            //wxLogMessage("cnt:%f", DCO_dco_rt_as[3].new_period_cnt_u32);
            m_sample_words = ((uint32_t)DCO_dco_rt_as[3].new_period_cnt_u32) * 4;
         break;
   }

   resetSynth();


   SYNENG_vKeyOn(m_note_u8, 127);
   SYNENG_vHandler();
   SYNENG_sample_in_idx_u16 = 0;
   SYNENG_sample_out_idx_u16 = 0;

   for(i = 0; i < m_sample_words; i++)
   {
      SYNENG_vHandler();
      if(SYNENG_sample_in_idx_u16 != SYNENG_sample_out_idx_u16)
      {
         m_sample_data_pau16[i] = (uint16_t)((( ((int32_t)SYNENG_buf_au32[SYNENG_sample_out_idx_u16] / 4)  ) / 16));
         SYNENG_sample_out_idx_u16++;
         if(SYNENG_sample_out_idx_u16 >= OUTBUF_SIZE)
         {
            SYNENG_sample_out_idx_u16 = 0;
         }   
      }
   }

   normalize();

   m_wavedisp->setSampleWords(m_sample_words);
   m_wavedisp->Refresh(true);

   if(m_audio->isPlaying())
   {
      startPlayback();
   }
}


void WaveGen::loadConfig(FILE *file)
{
   if(file)
   {     
      fread((uint8_t*)&DCO_dco_set_as, 1, sizeof(DCO_dco_set_as), file);
      fread((uint8_t*)&DCO_pwmmod_set_as, 1, sizeof(DCO_pwmmod_set_as), file);
      fread((uint8_t*)&DCO_freqmod_set_as, 1, sizeof(DCO_freqmod_set_as), file);
      fread((uint8_t*)&DADSR_pwmmod_data_as, 1, sizeof(DADSR_pwmmod_data_as), file);
      fread((uint8_t*)&DADSR_freqmod_data_as, 1, sizeof(DADSR_freqmod_data_as), file);
      fread((uint8_t*)&DADSR_filtmod_data_s, 1, sizeof(DADSR_dadsr_data_ts), file);
      fread((uint8_t*)&DADSR_resmod_data_s, 1, sizeof(DADSR_dadsr_data_ts), file);
      fread((uint8_t*)&DADSR_vcamod_data_s, 1, sizeof(DADSR_dadsr_data_ts), file);
      fread((uint8_t*)&DCO_vcf_lfo_s, 1, sizeof(DCO_vcf_lfo_ts), file);
      fread((uint8_t*)&DCO_synmode_e, 1, sizeof(DCO_synmode_te), file);
      fread((uint8_t*)&DCO_fm_algorithm_u8, 1, sizeof(DCO_fm_algorithm_u8), file);
      fread((uint8_t*)&DADSR_dcoout_data_as, 1, sizeof(DADSR_dcoout_data_as), file);
      fread((uint8_t*)&DCO_volmod_set_as, 1, sizeof(DCO_volmod_set_as), file);
      fread((uint8_t*)&SYNENG_dco_set_as, 1, sizeof(SYNENG_dco_set_as), file);
      fread(&m_note_u8, 1, 1, file);
      fread(&m_sample_words, 1, 4, file);
      fread(&m_length_mode, 1, sizeof(m_length_mode), file);
      fread(&m_sample_rate, 1, sizeof(m_sample_rate), file);

      m_syn_model->setLengthMode(m_length_mode);
      m_syn_model->setSampleRate(m_sample_rate);

      DCO_vUpdateRuntime();
      updateGUI();
      setNote(m_note_u8);     
   }   
}


void WaveGen::saveConfig(FILE *file)
{
   if(file)
   {     
      fwrite((uint8_t*)&DCO_dco_set_as, 1, sizeof(DCO_dco_set_as), file);
      fwrite((uint8_t*)&DCO_pwmmod_set_as, 1, sizeof(DCO_pwmmod_set_as), file);
      fwrite((uint8_t*)&DCO_freqmod_set_as, 1, sizeof(DCO_freqmod_set_as), file);
      fwrite((uint8_t*)&DADSR_pwmmod_data_as, 1, sizeof(DADSR_pwmmod_data_as), file);
      fwrite((uint8_t*)&DADSR_freqmod_data_as, 1, sizeof(DADSR_freqmod_data_as), file);
      fwrite((uint8_t*)&DADSR_filtmod_data_s, 1, sizeof(DADSR_dadsr_data_ts), file);
      fwrite((uint8_t*)&DADSR_resmod_data_s, 1, sizeof(DADSR_dadsr_data_ts), file);
      fwrite((uint8_t*)&DADSR_vcamod_data_s, 1, sizeof(DADSR_dadsr_data_ts), file);
      fwrite((uint8_t*)&DCO_vcf_lfo_s, 1, sizeof(DCO_vcf_lfo_ts), file);
      fwrite((uint8_t*)&DCO_synmode_e, 1, sizeof(DCO_synmode_te), file);
      fwrite((uint8_t*)&DCO_fm_algorithm_u8, 1, sizeof(DCO_fm_algorithm_u8), file);
      fwrite((uint8_t*)&DADSR_dcoout_data_as, 1, sizeof(DADSR_dcoout_data_as), file);
      fwrite((uint8_t*)&DCO_volmod_set_as, 1, sizeof(DCO_volmod_set_as), file);
      fwrite((uint8_t*)&SYNENG_dco_set_as, 1, sizeof(SYNENG_dco_set_as), file);
      fwrite(&m_note_u8, 1, 1, file);
      fwrite(&m_sample_words, 1, 4, file);
      fwrite(&m_length_mode, 1, sizeof(m_length_mode), file);
      fwrite(&m_sample_rate, 1, sizeof(m_sample_rate), file);
   }   
}

