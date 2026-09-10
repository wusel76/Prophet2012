#include "types.h"


#include <wx/wx.h>
#include <wx/string.h>
#include <wx/file.h>
#include <wx/spinctrl.h>

#include "GUILayout.h"
#include "main.h"
#include "presetpanel.h"
#include "proph2000.h"
#include <map>
#include <list>
using namespace std;
#include "presetcopydlg.h"
#include "presetparam.h"


PresetPanelImpl::PresetPanelImpl(wxWindow *parent, MyPanel *topframe, Proph2000 *proph, uint8_t preset_u8, MyPresetParam *presetparam) :
   PresetPanel(parent),
   m_topframe(topframe), m_proph(proph), m_preset_u8(preset_u8), m_presetparam(presetparam)
{
#if 0
   m_menu = new wxMenu(wxT("Preset"));
   m_menu->Append(GUI_LOAD_PRESET, "Load");
   m_menu->Append(GUI_SAVE_PRESET, "Save");
   m_menu->AppendSeparator();
   m_menu->Append(GUI_COPY, "Copy");
#endif

   m_left_map_selection->AppendString(wxT("map 1"));
   m_left_map_selection->AppendString(wxT("map 2"));
   m_left_map_selection->AppendString(wxT("map 3"));
   m_left_map_selection->AppendString(wxT("map 4"));
   m_left_map_selection->AppendString(wxT("map 5"));
   m_left_map_selection->AppendString(wxT("map 6"));
   m_left_map_selection->AppendString(wxT("map 7"));
   m_left_map_selection->AppendString(wxT("map 8"));
   m_left_map_selection->AppendString(wxT("map 9"));
   m_left_map_selection->AppendString(wxT("map a"));
   m_left_map_selection->AppendString(wxT("map b"));
   m_left_map_selection->AppendString(wxT("map c"));
   m_left_map_selection->AppendString(wxT("map d"));
   m_left_map_selection->AppendString(wxT("map e"));
   m_left_map_selection->AppendString(wxT("map f"));
   m_left_map_selection->AppendString(wxT("map g"));

   m_right_map_selection->AppendString(wxT("map 1"));
   m_right_map_selection->AppendString(wxT("map 2"));
   m_right_map_selection->AppendString(wxT("map 3"));
   m_right_map_selection->AppendString(wxT("map 4"));
   m_right_map_selection->AppendString(wxT("map 5"));
   m_right_map_selection->AppendString(wxT("map 6"));
   m_right_map_selection->AppendString(wxT("map 7"));
   m_right_map_selection->AppendString(wxT("map 8"));
   m_right_map_selection->AppendString(wxT("map 9"));
   m_right_map_selection->AppendString(wxT("map a"));
   m_right_map_selection->AppendString(wxT("map b"));
   m_right_map_selection->AppendString(wxT("map c"));
   m_right_map_selection->AppendString(wxT("map d"));
   m_right_map_selection->AppendString(wxT("map e"));
   m_right_map_selection->AppendString(wxT("map f"));
   m_right_map_selection->AppendString(wxT("map g"));

   m_vibrato->AppendString(wxT("off"));
   m_vibrato->AppendString(wxT("LFO"));
   m_vibrato->AppendString(wxT("inv. LFO"));
   m_lfo_filt->AppendString(wxT("off"));
   m_lfo_filt->AppendString(wxT("LFO"));
   m_lfo_filt->AppendString(wxT("inv. LFO"));

   m_lfo_amp->AppendString(wxT("off"));
   m_lfo_amp->AppendString(wxT("LFO"));
   m_lfo_amp->AppendString(wxT("inv. LFO"));

   m_keyboard_mode->AppendString(wxT("merge"));
   m_keyboard_mode->AppendString(wxT("split"));
   m_keyboard_mode->AppendString(wxT("right only"));
   m_keyboard_mode->AppendString(wxT("left only"));
   m_keyboard_mode->AppendString(wxT("layer"));
   m_keyboard_mode->AppendString(wxT("positional crossfade"));
   m_keyboard_mode->AppendString(wxT("velocity switch"));
   m_keyboard_mode->AppendString(wxT("velocity crossfade"));
   m_keyboard_mode->AppendString(wxT("mod wheel crossfade"));

   m_stack_voices->AppendString(wxT("1"));
   m_stack_voices->AppendString(wxT("2"));
   m_stack_voices->AppendString(wxT("4"));
   m_stack_voices->AppendString(wxT("8"));

   m_arp_mode->AppendString(wxT("up/down"));
   m_arp_mode->AppendString(wxT("up"));
   m_arp_mode->AppendString(wxT("down"));
   m_arp_mode->AppendString(wxT("assign"));
   m_arp_mode->AppendString(wxT("random"));

   m_arp_repeats->AppendString(wxT("1"));
   m_arp_repeats->AppendString(wxT("2"));
   m_arp_repeats->AppendString(wxT("3"));
   m_arp_repeats->AppendString(wxT("4"));
   m_arp_repeats->AppendString(wxT("8"));
   m_arp_repeats->AppendString(wxT("16"));
   m_arp_repeats->AppendString(wxT("random"));

   m_arp_oct->AppendString(wxT("1"));
   m_arp_oct->AppendString(wxT("2"));
   m_arp_oct->AppendString(wxT("3"));

   m_arp_latch_mode->AppendString(wxT("normal"));
   m_arp_latch_mode->AppendString(wxT("auto latch"));
   m_arp_latch_mode->AppendString(wxT("extend"));

   m_arp_split_type->AppendString(wxT("off"));
   m_arp_split_type->AppendString(wxT("above split"));
   m_arp_split_type->AppendString(wxT("below split"));

   m_midi_mode->AppendString(wxT("mode 0"));
   m_midi_mode->AppendString(wxT("mode 1"));
   m_midi_mode->AppendString(wxT("mode 3A"));
   m_midi_mode->AppendString(wxT("mode 3B"));
   m_midi_mode->AppendString(wxT("mode 4"));
   m_midi_mode->AppendString(wxT("mode 1."));
   m_midi_mode->AppendString(wxT("mode 3.A"));
   m_midi_mode->AppendString(wxT("mode 3.B"));
   m_midi_mode->AppendString(wxT("mode 4."));

   uint8_t i;
   for(i = 0; i < 16; i++)
   {
      wxString str;
      str.sprintf(wxT("%d"), i + 1);
      m_midi_left_ch->AppendString(str);
   }
   for(i = 0; i < 16; i++)
   {
      wxString str;
      str.sprintf(wxT("%d"), i + 1);
      m_midi_right_ch->AppendString(str);
   }
   updateGUI();

   deselectAllPresetButtons();
   m_select_preset1->SetValue(true);
   activatePreset(0);
}


PresetPanelImpl::~PresetPanelImpl()
{
}


void PresetPanelImpl::activatePreset(uint8_t preset_u8)
{
   m_preset_u8 = preset_u8;

   deselectAllPresetButtons();
   switch(preset_u8)
   {
      case 0:
         m_select_preset1->SetValue(true);
      break;
      case 1:
         m_select_preset2->SetValue(true);
      break;
      case 2:
         m_select_preset3->SetValue(true);
      break;
      case 3:
         m_select_preset4->SetValue(true);
      break;
      case 4:
         m_select_preset5->SetValue(true);
      break;
      case 5:
         m_select_preset6->SetValue(true);
      break;
      case 6:
         m_select_preset7->SetValue(true);
      break;
      case 7:
         m_select_preset8->SetValue(true);
      break;
      case 8:
         m_select_preset9->SetValue(true);
      break;
      case 9:
         m_select_preset10->SetValue(true);
      break;
      case 10:
         m_select_preset11->SetValue(true);
      break;
      case 11:
         m_select_preset12->SetValue(true);
      break;
   }

   updateGUI();
}

wxString PresetPanelImpl::getNote(uint8_t number_u8)
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


void PresetPanelImpl::OnSplitPoint(wxCommandEvent& event)
{
   uint8_t note;
   if(convertStringToNote(m_split_point->GetValue(), &note))
   {
      m_proph->m_preset_param_as[m_preset_u8].val_s.split_point = note;
      m_proph->updateParamBytesFromStruct();
      m_topframe->updateGUI();
      if(m_topframe->getOnlineUpdateActive())
      {
         if(m_proph->transferPreset2Proph(m_preset_u8))
         {
            m_topframe->transferStarted();
         }
      }
   }
}

void PresetPanelImpl::OnArpSplitPoint(wxCommandEvent& event)
{
   uint8_t note;
   if(convertStringToNote(m_arp_split_point->GetValue(), &note))
   {
      m_proph->m_preset_param_as[m_preset_u8].val_s.arp_split_point = note;
      m_proph->updateParamBytesFromStruct();
      m_topframe->updateGUI();
      if(m_topframe->getOnlineUpdateActive())
      {
         if(m_proph->transferPreset2Proph(m_preset_u8))
         {
            m_topframe->transferStarted();
         }
      }
   }
}


bool PresetPanelImpl::convertStringToNote(wxString str, uint8_t *note_pu8)
{
   uint8_t note = 0;
   bool retval = true;
   uint8_t pos = 0;
   str.MakeUpper();

   if(str.find(wxT("C#")) == 0)
   {
      note = 1;
      pos = 2;
   }else
   if(str.find(wxT("C")) == 0)
   {
      note = 0;
      pos = 1;
   }else
   if(str.find(wxT("D#")) == 0)
   {
      note = 3;
      pos = 2;
   }else
   if(str.find(wxT("D")) == 0)
   {
      note = 2;
      pos = 1;
   }else
   if(str.find(wxT("E")) == 0)
   {
      note = 4;
      pos = 1;
   }else
   if(str.find(wxT("F#")) == 0)
   {
      note = 6;
      pos = 2;
   }else
   if(str.find(wxT("F")) == 0)
   {
      note = 5;
      pos = 1;
   }else
   if(str.find(wxT("G#")) == 0)
   {
      note = 8;
      pos = 2;
   }else
   if(str.find(wxT("G")) == 0)
   {
      note = 7;
      pos = 1;
   }else
   if(str.find(wxT("A#")) == 0)
   {
      note = 10;
      pos = 2;
   }else
   if(str.find(wxT("A")) == 0)
   {
      note = 9;
      pos = 1;
   }else
   if(str.find(wxT("B")) == 0)
   {
      note = 11;
      pos = 1;
   }
   else
   {
      retval = false;
   }

   str = str.substr(pos);
   if(str != wxT(""))
   {
      long octave;
      str.ToLong(&octave);
      if((octave >= -3) && (octave <= 8))
      {
         note += ((octave * 12) + 24);
      }
      else
      {
         retval = false;
      }
   }
   else
   {
      retval = false;
   }

   *note_pu8 = note;

   return retval;
}

void PresetPanelImpl::updateGUI(void)
{
   wxString str;
   uint8_t t_u8;

   switch(m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side)
   {
      case 0:
         m_left_map_selection->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number);
         break;
      case 1:
         m_left_map_selection->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number + 8);
         break;
   }

   switch(m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side)
   {
      case 0:
         m_right_map_selection->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number);
         break;
      case 1:
         m_right_map_selection->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number + 8);
         break;
   }

   m_lfo_freq->SetValue(m_proph->m_preset_param_as[m_preset_u8].val_s.lfo_freq);
   m_lfo_init_amount->SetValue(m_proph->m_preset_param_as[m_preset_u8].val_s.lfo_init_amount);
   m_lfo_vel_sensitivity->SetValue((int8_t)(m_proph->m_preset_param_as[m_preset_u8].val_s.lfo_vel_sensitivity - 0x80));
   m_vibrato->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.vibrato);
   m_lfo_filt->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.lfo_filt);
   m_lfo_amp->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.lfo_amp);
   m_keyboard_mode->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.keyboard_mode);
   str = getNote(m_proph->m_preset_param_as[m_preset_u8].val_s.split_point);
   m_split_point->SetValue(str);
   m_transpose->SetValue((int8_t)(m_proph->m_preset_param_as[m_preset_u8].val_s.transpose));
   if(m_proph->m_preset_param_as[m_preset_u8].val_s.dyn_alloc)
   {
      m_dyn_alloc->SetValue(true);
   }
   else
   {
      m_dyn_alloc->SetValue(false);
   }
   m_vel_switch_threshold->SetValue(m_proph->m_preset_param_as[m_preset_u8].val_s.vel_switch_threshold);
   m_stack_voices->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.stack_voices & 0x03);
   if(m_proph->m_preset_param_as[m_preset_u8].val_s.stack_voices & 0x20)
   {
      m_stack_voices_preset_on->SetValue(true);
   }
   else
   {
      m_stack_voices_preset_on->SetValue(false);
   }
   m_stack_delay->SetValue(m_proph->m_preset_param_as[m_preset_u8].val_s.stack_delay);
   m_stack_detune->SetValue(m_proph->m_preset_param_as[m_preset_u8].val_s.stack_detune);
   m_arp_mode->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.arp_mode & 0x07);
   if(m_proph->m_preset_param_as[m_preset_u8].val_s.arp_mode & 0x10)
   {
      m_arp_preset_on->SetValue(true);
   }
   else
   {
      m_arp_preset_on->SetValue(false);
   }

   m_arp_oct->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.arp_oct - 1);
   m_arp_repeats->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.arp_repeats);
   m_arp_rate->SetValue(m_proph->m_preset_param_as[m_preset_u8].val_s.arp_rate);
   m_arp_latch_mode->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.arp_latch_mode);
   m_arp_split_point->SetValue(getNote(m_proph->m_preset_param_as[m_preset_u8].val_s.arp_split_point));
   m_arp_split_type->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.arp_split_type);
   m_pitchwheel_range->SetValue(m_proph->m_preset_param_as[m_preset_u8].val_s.pitchwheel_range);
   m_midi_mode->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.midi_mode);

   t_u8 = m_proph->m_preset_param_as[m_preset_u8].val_s.midi_opts;
   if(t_u8 & 0x01)
   {
      m_rx_mod_wheel->SetValue(true);
   }
   else
   {
      m_rx_mod_wheel->SetValue(false);
   }
   if(t_u8 & 0x02)
   {
      m_rx_pitch_wheel->SetValue(true);
   }
   else
   {
      m_rx_pitch_wheel->SetValue(false);
   }
   if(t_u8 & 0x04)
   {
      m_rx_program_chg->SetValue(true);
   }
   else
   {
      m_rx_program_chg->SetValue(false);
   }
   if(t_u8 & 0x08)
   {
      m_rx_pressure_as_mod_wheel->SetValue(true);
   }
   else
   {
      m_rx_pressure_as_mod_wheel->SetValue(false);
   }
   if(t_u8 & 0x10)
   {
      m_tx_mod_wheel->SetValue(true);
   }
   else
   {
      m_tx_mod_wheel->SetValue(false);
   }
   if(t_u8 & 0x20)
   {
      m_tx_pitch_wheel->SetValue(true);
   }
   else
   {
      m_tx_pitch_wheel->SetValue(false);
   }
   if(t_u8 & 0x40)
   {
      m_tx_program_chg->SetValue(true);
   }
   else
   {
      m_tx_program_chg->SetValue(false);
   }
   if(t_u8 & 0x80)
   {
      m_tx_mod_wheel_as_pressure->SetValue(true);
   }
   else
   {
      m_tx_mod_wheel_as_pressure->SetValue(false);
   }

   m_midi_right_ch->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.midi_right_ch);
   m_midi_left_ch->SetSelection(m_proph->m_preset_param_as[m_preset_u8].val_s.midi_left_ch);
}


void PresetPanelImpl::OnParamChange(wxCommandEvent &event)
{
}


void PresetPanelImpl::OnLeftMapSelect(wxCommandEvent &event)
{
   switch(m_left_map_selection->GetSelection())
   {
   case 0:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 0;
      break;
   case 1:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 1;
      break;
   case 2:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 2;
      break;
   case 3:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 3;
      break;
   case 4:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 4;
      break;
   case 5:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 5;
      break;
   case 6:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 6;
      break;
   case 7:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 7;
      break;
   case 8:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 0;
      break;
   case 9:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 1;
      break;
   case 10:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 2;
      break;
   case 11:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 3;
      break;
   case 12:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 4;
      break;
   case 13:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 5;
      break;
   case 14:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 6;
      break;
   case 15:
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.left_x_map_number = 7;
      break;
   }
   m_proph->updateParamBytesFromStruct();
   m_topframe->updateGUI();
   if(m_topframe->getOnlineUpdateActive())
   {
      if(m_proph->transferPreset2Proph(m_preset_u8))
      {
         m_topframe->transferStarted();
      }
   }
}

void PresetPanelImpl::OnRightMapSelect(wxCommandEvent &event)
{
   switch(m_right_map_selection->GetSelection())
   {
   case 0:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 0;
      break;
   case 1:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 1;
      break;
   case 2:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 2;
      break;
   case 3:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 3;
      break;
   case 4:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 4;
      break;
   case 5:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 5;
      break;
   case 6:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 6;
      break;
   case 7:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 0;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 7;
      break;
   case 8:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 0;
      break;
   case 9:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 1;
      break;
   case 10:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 2;
      break;
   case 11:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 3;
      break;
   case 12:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 4;
      break;
   case 13:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 5;
      break;
   case 14:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 6;
      break;
   case 15:
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_side = 1;
      m_proph->m_preset_param_as[m_preset_u8].val_s.right_y_map_number = 7;
      break;
   }
   m_proph->updateParamBytesFromStruct();
   m_topframe->updateGUI();
   if(m_topframe->getOnlineUpdateActive())
   {
      if(m_proph->transferPreset2Proph(m_preset_u8))
      {
         m_topframe->transferStarted();
      }
   }
}

void PresetPanelImpl::OnSliderChange(wxScrollEvent &event)
{
   m_proph->m_preset_param_as[m_preset_u8].val_s.lfo_freq = m_lfo_freq->GetValue();
   m_proph->m_preset_param_as[m_preset_u8].val_s.lfo_init_amount = m_lfo_init_amount->GetValue();
   m_proph->m_preset_param_as[m_preset_u8].val_s.lfo_vel_sensitivity = (int8_t)m_lfo_vel_sensitivity->GetValue() + 0x80;
   m_proph->m_preset_param_as[m_preset_u8].val_s.transpose = (int8_t)m_transpose->GetValue();
   m_proph->m_preset_param_as[m_preset_u8].val_s.vel_switch_threshold = m_vel_switch_threshold->GetValue();
   m_proph->m_preset_param_as[m_preset_u8].val_s.stack_delay = m_stack_delay->GetValue();
   m_proph->m_preset_param_as[m_preset_u8].val_s.stack_detune = m_stack_detune->GetValue();
   m_proph->m_preset_param_as[m_preset_u8].val_s.arp_rate = m_arp_rate->GetValue();
   m_proph->m_preset_param_as[m_preset_u8].val_s.pitchwheel_range = m_pitchwheel_range->GetValue();
   m_proph->m_preset_param_as[m_preset_u8].val_s.midi_left_ch = m_midi_left_ch->GetSelection();
   m_proph->m_preset_param_as[m_preset_u8].val_s.midi_right_ch = m_midi_right_ch->GetSelection();

   m_proph->updateParamBytesFromStruct();
   m_topframe->updateGUI();
   if(m_topframe->getOnlineUpdateActive())
   {
      if(m_proph->transferPreset2Proph(m_preset_u8))
      {
         m_topframe->transferStarted();
      }
   }
}

void PresetPanelImpl::OnComboChanged(wxCommandEvent &event)
{
   m_proph->m_preset_param_as[m_preset_u8].val_s.arp_repeats = m_arp_repeats->GetSelection();
   m_proph->m_preset_param_as[m_preset_u8].val_s.vibrato = m_vibrato->GetSelection();
   m_proph->m_preset_param_as[m_preset_u8].val_s.lfo_filt = m_lfo_filt->GetSelection();
   m_proph->m_preset_param_as[m_preset_u8].val_s.lfo_amp = m_lfo_amp->GetSelection();
   m_proph->m_preset_param_as[m_preset_u8].val_s.keyboard_mode = m_keyboard_mode->GetSelection();
   m_proph->m_preset_param_as[m_preset_u8].val_s.stack_voices = m_stack_voices->GetSelection();
   if(m_stack_voices_preset_on->GetValue())
   {
      m_proph->m_preset_param_as[m_preset_u8].val_s.stack_voices = (m_proph->m_preset_param_as[m_preset_u8].val_s.stack_voices & 0x03) + 0x20;
   }
   else
   {
      m_proph->m_preset_param_as[m_preset_u8].val_s.stack_voices = (m_proph->m_preset_param_as[m_preset_u8].val_s.stack_voices & 0x03);
   }


   m_proph->m_preset_param_as[m_preset_u8].val_s.arp_mode = m_arp_mode->GetSelection();
   if(m_arp_preset_on->GetValue())
   {
      m_proph->m_preset_param_as[m_preset_u8].val_s.arp_mode = (m_proph->m_preset_param_as[m_preset_u8].val_s.arp_mode & 0x07) + 0x10;
   }
   else
   {
      m_proph->m_preset_param_as[m_preset_u8].val_s.arp_mode = (m_proph->m_preset_param_as[m_preset_u8].val_s.arp_mode & 0x07);
   }

   m_proph->m_preset_param_as[m_preset_u8].val_s.arp_oct = m_arp_oct->GetSelection() + 1;
   m_proph->m_preset_param_as[m_preset_u8].val_s.arp_split_type = m_arp_split_type->GetSelection();
   m_proph->m_preset_param_as[m_preset_u8].val_s.midi_mode = m_midi_mode->GetSelection();
   m_proph->m_preset_param_as[m_preset_u8].val_s.arp_latch_mode = m_arp_latch_mode->GetSelection();

   m_proph->updateParamBytesFromStruct();
   if(m_topframe->getOnlineUpdateActive())
   {
      if(m_proph->transferPreset2Proph(m_preset_u8))
      {
         m_topframe->transferStarted();
      }
   }
   m_topframe->updateGUI();
}

void PresetPanelImpl::OnCheckboxChange(wxCommandEvent& event)
{
   uint8_t t_u8;
   if(m_dyn_alloc->GetValue())
   {
      m_proph->m_preset_param_as[m_preset_u8].val_s.dyn_alloc = 1;
   }
   else
   {
      m_proph->m_preset_param_as[m_preset_u8].val_s.dyn_alloc = 0;
   }

   if(m_stack_voices_preset_on->GetValue())
   {
      m_proph->m_preset_param_as[m_preset_u8].val_s.stack_voices = (m_proph->m_preset_param_as[m_preset_u8].val_s.stack_voices & 0x03) + 0x20;
   }
   else
   {
      m_proph->m_preset_param_as[m_preset_u8].val_s.stack_voices = (m_proph->m_preset_param_as[m_preset_u8].val_s.stack_voices & 0x03);
   }

   if(m_arp_preset_on->GetValue())
   {
      m_proph->m_preset_param_as[m_preset_u8].val_s.arp_mode = (m_proph->m_preset_param_as[m_preset_u8].val_s.arp_mode & 0x07) + 0x10;
   }
   else
   {
      m_proph->m_preset_param_as[m_preset_u8].val_s.arp_mode = (m_proph->m_preset_param_as[m_preset_u8].val_s.arp_mode & 0x07);
   }


   t_u8 = 0;
   if(m_rx_mod_wheel->GetValue())
   {
      t_u8 |= 0x01;
   } 
   if(m_rx_pitch_wheel->GetValue())
   {
      t_u8 |= 0x02;
   }
   if(m_rx_program_chg->GetValue())
   {
      t_u8 |= 0x04;
   }
   if(m_rx_pressure_as_mod_wheel->GetValue())
   {
      t_u8 |= 0x08;
   }
   if(m_tx_mod_wheel->GetValue())
   {
      t_u8 |= 0x10;
   }
   if(m_tx_pitch_wheel->GetValue())
   {
      t_u8 |= 0x20;
   }
   if(m_tx_program_chg->GetValue())
   {
      t_u8 |= 0x40;
   }
   if(m_tx_mod_wheel_as_pressure->GetValue())
   {
      t_u8 |= 0x80;
   }
   m_proph->m_preset_param_as[m_preset_u8].val_s.midi_opts = t_u8;

   m_proph->updateParamBytesFromStruct();
   m_topframe->updateGUI();
   if(m_topframe->getOnlineUpdateActive())
   {
      if(m_proph->transferPreset2Proph(m_preset_u8))
      {
         m_topframe->transferStarted();
      }
   }
}

#if 0
void PresetPanelImpl::transferStarted(void)
{
   m_menu->Enable(GUI_LOAD_PRESET, false);
   m_menu->Enable(GUI_SAVE_PRESET, false);
   m_menu->Enable(GUI_COPY, false);
}
void PresetPanelImpl::transferEnded(void)
{
   m_menu->Enable(GUI_LOAD_PRESET, true);
   m_menu->Enable(GUI_SAVE_PRESET, true);
   m_menu->Enable(GUI_COPY, true);
}
#endif

void PresetPanelImpl::OnLoad(wxCommandEvent& event)
{
   m_presetparam->OnLoad(event);
}

void PresetPanelImpl::OnSave(wxCommandEvent& event)
{
   m_presetparam->OnSave(event);
}

void PresetPanelImpl::OnCopy(wxCommandEvent& event)
{
   m_presetparam->OnCopy(event);
}


void PresetPanelImpl::OnSelectPreset1(wxCommandEvent& event)
{
   m_topframe->activatePreset(0);
}

void PresetPanelImpl::OnSelectPreset2(wxCommandEvent& event)
{
   m_topframe->activatePreset(1);
}

void PresetPanelImpl::OnSelectPreset3(wxCommandEvent& event)
{
   m_topframe->activatePreset(2);
}

void PresetPanelImpl::OnSelectPreset4(wxCommandEvent& event)
{
   m_topframe->activatePreset(3);
}

void PresetPanelImpl::OnSelectPreset5(wxCommandEvent& event)
{
   m_topframe->activatePreset(4);
}

void PresetPanelImpl::OnSelectPreset6(wxCommandEvent& event)
{
   m_topframe->activatePreset(5);
}

void PresetPanelImpl::OnSelectPreset7(wxCommandEvent& event)
{
   m_topframe->activatePreset(6);
}

void PresetPanelImpl::OnSelectPreset8(wxCommandEvent& event)
{
   m_topframe->activatePreset(7);
}

void PresetPanelImpl::OnSelectPreset9(wxCommandEvent& event)
{
   m_topframe->activatePreset(8);
}

void PresetPanelImpl::OnSelectPreset10(wxCommandEvent& event)
{
   m_topframe->activatePreset(9);
}

void PresetPanelImpl::OnSelectPreset11(wxCommandEvent& event)
{
   m_topframe->activatePreset(10);
}

void PresetPanelImpl::OnSelectPreset12(wxCommandEvent& event)
{
   m_topframe->activatePreset(11);
}

void PresetPanelImpl::deselectAllPresetButtons(void)
{
   m_select_preset1->SetValue(false);
   m_select_preset2->SetValue(false);
   m_select_preset3->SetValue(false);
   m_select_preset4->SetValue(false);
   m_select_preset5->SetValue(false);
   m_select_preset6->SetValue(false);
   m_select_preset7->SetValue(false);
   m_select_preset8->SetValue(false);
   m_select_preset9->SetValue(false);
   m_select_preset10->SetValue(false);
   m_select_preset11->SetValue(false);
   m_select_preset12->SetValue(false);
}
