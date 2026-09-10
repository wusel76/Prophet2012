#include "types.h"
#include "wavegenconfig.h"
#include "synthengine.h"
#include "dco.h"
#include "dadsr.h"
#include "noise.h"

#include <stdio.h>
#include <math.h>


#define M_PI 3.1415926535897932384626433832795


DCO_set_ts DCO_dco_set_as[DCO_MAX_DCOS];
DCO_rt_ts DCO_dco_rt_as[DCO_MAX_DCOS];

DCO_set_ts DCO_pwmmod_set_as[DCO_MAX_DCOS];
DCO_rt_ts DCO_pwmmod_rt_as[DCO_MAX_DCOS];

DCO_set_ts DCO_freqmod_set_as[DCO_MAX_DCOS];
DCO_rt_ts DCO_freqmod_rt_as[DCO_MAX_DCOS];

DCO_set_ts DCO_volmod_set_as[DCO_MAX_DCOS];
DCO_rt_ts DCO_volmod_rt_as[DCO_MAX_DCOS];


float DCO_pwm_mod_output_ai16[DCO_MAX_DCOS];
float DCO_freq_mod_output_ai16[DCO_MAX_DCOS];
uint8_t DCO_mod_calc_cnt_u16;

DCO_vcf_lfo_ts DCO_vcf_lfo_s;
DCO_rt_ts DCO_cutoffmod_dco_rt_s;
DCO_rt_ts DCO_resomod_dco_rt_s;

DCO_synmode_te DCO_synmode_e;

uint8_t DCO_fm_algorithm_u8;
uint8_t DCO_fm_feedback_u8;

float DCO_fm_feedback_sig_i16; 

int8_t DCO_master_tune_i8;

static uint32_t DCO_u32GenerateRandomNumber(void);

float DCO_i16GetNextValue(DCO_rt_ts *rt_ps, DCO_set_ts *set_ps, DADSR_runtime_data_ts *dadsr_volume_ps, DCO_set_ts *volmod_lfo_ps);
float DCO_i16GetNextFMValue(DCO_rt_ts *rt_ps, DCO_set_ts *set_ps, float modulator_i16, DADSR_runtime_data_ts *dadsr_volume_ps, DCO_set_ts *volmod_lfo_ps);


void DCO_vInit(void)
{
   uint8_t i;

   DCO_mod_calc_cnt_u16 = 0;

   DCO_fm_algorithm_u8 = 0;

   DCO_fm_feedback_u8 = DCO_FM_MAX_FEEDBACK;
   
   DCO_synmode_e = DCO_ADD;
   
   DCO_master_tune_i8 = 0;
   
   DCO_fm_feedback_sig_i16 = 0;

   for(i = 0; i < DCO_MAX_DCOS; i++)
   {
      // init sound DCOs
      DCO_dco_set_as[i].wave_e = DCO_SAW;
      DCO_dco_set_as[i].freq_mHz_u32 = 1;
      DCO_dco_set_as[i].level_u8 = 0;
      DCO_dco_set_as[i].sync_e = DCO_NOSYNC;
      DCO_dco_set_as[i].pwm_dadsr_mod_type_e = SYNENG_NOMOD;
      DCO_dco_set_as[i].freq_dadsr_mod_type_e = SYNENG_NOMOD;
      DCO_dco_set_as[i].keysync_u8 = 0;
      DCO_dco_set_as[i].sub_osc_level_u8 = 0;
      DCO_dco_set_as[i].scale_i8 = 1;
      DCO_dco_set_as[i].reserved_2_u8 = 0;
      DCO_dco_set_as[i].reserved_3_u8 = 0;
      DCO_dco_set_as[i].fix_freq_hz_u32 = 440;
      DCO_dco_set_as[i].reserved3 = 0;
      DCO_dco_set_as[i].reserved4 = 0;
      DCO_dco_set_as[i].reserved5 = 0;
      DCO_dco_set_as[i].reserved6 = 0;
      DCO_dco_set_as[i].reserved7 = 0;
      DCO_dco_set_as[i].reserved8 = 0;
      DCO_dco_set_as[i].reserved9 = 0;
      DCO_dco_set_as[i].reserved10 = 0;
      DCO_dco_set_as[i].reserved11 = 0;
      DCO_dco_set_as[i].reserved12 = 0;

      DCO_dco_rt_as[i].zero_crossed_u8 = 0;
      DCO_dco_rt_as[i].lastoutput_i16 = 0;
      DCO_dco_rt_as[i].period_cnt_u32 = 1;
      DCO_dco_rt_as[i].cnt_u32 = 0;
      DCO_dco_rt_as[i].new_period_cnt_u32 = 1;
      DCO_dco_rt_as[i].new_PWM_dc_cnt_u32 = 1;
      DCO_dco_rt_as[i].volume_mod_lfo_i16 = 0;
      DCO_vSetFreq(&DCO_dco_rt_as[i], &DCO_dco_set_as[i], 100 * 1000, DCO_SAMPLE_FREQ);
      DCO_vSetPWM(&DCO_dco_rt_as[i], &DCO_dco_set_as[i], DCO_PWM_RANGE / 2);

      // init PWM mod DCOs
      DCO_pwm_mod_output_ai16[i] = 0;
      DCO_pwmmod_set_as[i].wave_e = DCO_SAW;
      DCO_pwmmod_set_as[i].freq_mHz_u32 = 1;
      DCO_pwmmod_set_as[i].level_u8 = 0;
      DCO_pwmmod_set_as[i].sync_e = DCO_NOSYNC;
      DCO_pwmmod_set_as[i].pwm_dadsr_mod_type_e = SYNENG_NOMOD;
      DCO_pwmmod_set_as[i].freq_dadsr_mod_type_e = SYNENG_NOMOD;
      DCO_pwmmod_set_as[i].keysync_u8 = 0;
      DCO_pwmmod_set_as[i].sub_osc_level_u8 = 0;
      DCO_pwmmod_set_as[i].scale_i8 = 1;
      DCO_pwmmod_set_as[i].reserved_2_u8 = 0;
      DCO_pwmmod_set_as[i].reserved_3_u8 = 0;
      DCO_pwmmod_set_as[i].fix_freq_hz_u32 = 440;
      DCO_pwmmod_set_as[i].reserved3 = 0;
      DCO_pwmmod_set_as[i].reserved4 = 0;
      DCO_pwmmod_set_as[i].reserved5 = 0;
      DCO_pwmmod_set_as[i].reserved6 = 0;
      DCO_pwmmod_set_as[i].reserved7 = 0;
      DCO_pwmmod_set_as[i].reserved8 = 0;
      DCO_pwmmod_set_as[i].reserved9 = 0;
      DCO_pwmmod_set_as[i].reserved10 = 0;
      DCO_pwmmod_set_as[i].reserved11 = 0;
      DCO_pwmmod_set_as[i].reserved12 = 0;

      DCO_pwmmod_rt_as[i].zero_crossed_u8 = 0;
      DCO_pwmmod_rt_as[i].lastoutput_i16 = 0;
      DCO_pwmmod_rt_as[i].period_cnt_u32 = 1;
      DCO_pwmmod_rt_as[i].cnt_u32 = 0;
      DCO_pwmmod_rt_as[i].new_period_cnt_u32 = 1;
      DCO_pwmmod_rt_as[i].new_PWM_dc_cnt_u32 = 1;
      DCO_pwmmod_rt_as[i].volume_mod_lfo_i16 = 0;
      DCO_vSetFreq(&DCO_pwmmod_rt_as[i], &DCO_pwmmod_set_as[i], 1 * 1000, DCO_SAMPLE_FREQ);
      DCO_vSetPWM(&DCO_pwmmod_rt_as[i], &DCO_pwmmod_set_as[i], DCO_PWM_RANGE / 2);

      // init freq mod DCOs
      DCO_freq_mod_output_ai16[i] = 0;
      DCO_freqmod_set_as[i].wave_e = DCO_SAW;
      DCO_freqmod_set_as[i].freq_mHz_u32 = 1;
      DCO_freqmod_set_as[i].level_u8 = 0;
      DCO_freqmod_set_as[i].sync_e = DCO_NOSYNC;
      DCO_freqmod_set_as[i].pwm_dadsr_mod_type_e = SYNENG_NOMOD;
      DCO_freqmod_set_as[i].freq_dadsr_mod_type_e = SYNENG_NOMOD;
      DCO_freqmod_set_as[i].keysync_u8 = 0;
      DCO_freqmod_set_as[i].sub_osc_level_u8 = 0;
      DCO_freqmod_set_as[i].scale_i8 = 1;
      DCO_freqmod_set_as[i].reserved_2_u8 = 0;
      DCO_freqmod_set_as[i].reserved_3_u8 = 0;
      DCO_freqmod_set_as[i].fix_freq_hz_u32 = 440;
      DCO_freqmod_set_as[i].reserved3 = 0;
      DCO_freqmod_set_as[i].reserved4 = 0;
      DCO_freqmod_set_as[i].reserved5 = 0;
      DCO_freqmod_set_as[i].reserved6 = 0;
      DCO_freqmod_set_as[i].reserved7 = 0;
      DCO_freqmod_set_as[i].reserved8 = 0;
      DCO_freqmod_set_as[i].reserved9 = 0;
      DCO_freqmod_set_as[i].reserved10 = 0;
      DCO_freqmod_set_as[i].reserved11 = 0;
      DCO_freqmod_set_as[i].reserved12 = 0;

      DCO_freqmod_rt_as[i].zero_crossed_u8 = 0;
      DCO_freqmod_rt_as[i].lastoutput_i16 = 0;
      DCO_freqmod_rt_as[i].period_cnt_u32 = 1;
      DCO_freqmod_rt_as[i].cnt_u32 = 0;
      DCO_freqmod_rt_as[i].new_period_cnt_u32 = 1;
      DCO_freqmod_rt_as[i].new_PWM_dc_cnt_u32 = 1;
      DCO_freqmod_rt_as[i].volume_mod_lfo_i16 = 0;
      DCO_vSetFreq(&DCO_freqmod_rt_as[i], &DCO_freqmod_set_as[i], 1 * 1000, DCO_SAMPLE_FREQ);
      DCO_vSetPWM(&DCO_freqmod_rt_as[i], &DCO_freqmod_set_as[i], DCO_PWM_RANGE / 2);      

      // init volume mod
      DCO_volmod_set_as[i].wave_e = DCO_SAW;
      DCO_volmod_set_as[i].freq_mHz_u32 = 1;
      DCO_volmod_set_as[i].level_u8 = 0;
      DCO_volmod_set_as[i].sync_e = DCO_NOSYNC;
      DCO_volmod_set_as[i].pwm_dadsr_mod_type_e = SYNENG_NOMOD;
      DCO_volmod_set_as[i].freq_dadsr_mod_type_e = SYNENG_NOMOD;
      DCO_volmod_set_as[i].keysync_u8 = 0;
      DCO_volmod_set_as[i].sub_osc_level_u8 = 0;
      DCO_volmod_set_as[i].scale_i8 = 1;
      DCO_volmod_set_as[i].reserved_2_u8 = 0;
      DCO_volmod_set_as[i].reserved_3_u8 = 0;
      DCO_volmod_set_as[i].fix_freq_hz_u32 = 440;
      DCO_volmod_set_as[i].reserved3 = 0;
      DCO_volmod_set_as[i].reserved4 = 0;
      DCO_volmod_set_as[i].reserved5 = 0;
      DCO_volmod_set_as[i].reserved6 = 0;
      DCO_volmod_set_as[i].reserved7 = 0;
      DCO_volmod_set_as[i].reserved8 = 0;
      DCO_volmod_set_as[i].reserved9 = 0;
      DCO_volmod_set_as[i].reserved10 = 0;
      DCO_volmod_set_as[i].reserved11 = 0;
      DCO_volmod_set_as[i].reserved12 = 0;

      DCO_volmod_rt_as[i].zero_crossed_u8 = 0;
      DCO_volmod_rt_as[i].lastoutput_i16 = 0;
      DCO_volmod_rt_as[i].period_cnt_u32 = 1;
      DCO_volmod_rt_as[i].cnt_u32 = 0;
      DCO_volmod_rt_as[i].new_period_cnt_u32 = 1;
      DCO_volmod_rt_as[i].new_PWM_dc_cnt_u32 = 1;
      DCO_volmod_rt_as[i].volume_mod_lfo_i16 = 0;
      DCO_vSetFreq(&DCO_volmod_rt_as[i], &DCO_volmod_set_as[i], 1 * 1000, DCO_SAMPLE_FREQ);
      DCO_vSetPWM(&DCO_volmod_rt_as[i], &DCO_volmod_set_as[i], DCO_PWM_RANGE / 2);      
   }

   // init cutoff mod DCO
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.wave_e = DCO_SAW;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.freq_mHz_u32 = 1;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.level_u8 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.sync_e = DCO_NOSYNC;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.pwm_dadsr_mod_type_e = SYNENG_NOMOD;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.freq_dadsr_mod_type_e = SYNENG_NOMOD;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.keysync_u8 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.sub_osc_level_u8 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.scale_i8 = 1;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.reserved_2_u8 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.reserved_3_u8 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.fix_freq_hz_u32 = 440;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.reserved3 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.reserved4 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.reserved5 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.reserved6 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.reserved7 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.reserved8 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.reserved9 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.reserved10 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.reserved11 = 0;
   DCO_vcf_lfo_s.cutoffmod_dco_set_s.reserved12 = 0;

   DCO_cutoffmod_dco_rt_s.zero_crossed_u8 = 0;
   DCO_cutoffmod_dco_rt_s.lastoutput_i16 = 0;
   DCO_cutoffmod_dco_rt_s.period_cnt_u32 = 1;
   DCO_cutoffmod_dco_rt_s.cnt_u32 = 0;
   DCO_cutoffmod_dco_rt_s.new_period_cnt_u32 = 1;
   DCO_cutoffmod_dco_rt_s.new_PWM_dc_cnt_u32 = 1;
   DCO_cutoffmod_dco_rt_s.volume_mod_lfo_i16 = 0;
   DCO_vSetFreq(&DCO_cutoffmod_dco_rt_s, &DCO_vcf_lfo_s.cutoffmod_dco_set_s, 1 * 1000, SYNENG_CTRL_FREQ);
   DCO_vSetPWM(&DCO_cutoffmod_dco_rt_s, &DCO_vcf_lfo_s.cutoffmod_dco_set_s, DCO_PWM_RANGE / 2);

   // init resonance mod DCO
   DCO_vcf_lfo_s.resomod_dco_set_s.wave_e = DCO_SAW;
   DCO_vcf_lfo_s.resomod_dco_set_s.freq_mHz_u32 = 1;
   DCO_vcf_lfo_s.resomod_dco_set_s.level_u8 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.sync_e = DCO_NOSYNC;
   DCO_vcf_lfo_s.resomod_dco_set_s.pwm_dadsr_mod_type_e = SYNENG_NOMOD;
   DCO_vcf_lfo_s.resomod_dco_set_s.freq_dadsr_mod_type_e = SYNENG_NOMOD;
   DCO_vcf_lfo_s.resomod_dco_set_s.keysync_u8 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.sub_osc_level_u8 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.scale_i8 = 1;
   DCO_vcf_lfo_s.resomod_dco_set_s.reserved_2_u8 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.reserved_3_u8 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.fix_freq_hz_u32 = 440;
   DCO_vcf_lfo_s.resomod_dco_set_s.reserved3 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.reserved4 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.reserved5 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.reserved6 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.reserved7 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.reserved8 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.reserved9 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.reserved10 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.reserved11 = 0;
   DCO_vcf_lfo_s.resomod_dco_set_s.reserved12 = 0;

   DCO_resomod_dco_rt_s.zero_crossed_u8 = 0;
   DCO_resomod_dco_rt_s.lastoutput_i16 = 0;
   DCO_resomod_dco_rt_s.period_cnt_u32 = 1;
   DCO_resomod_dco_rt_s.cnt_u32 = 0;
   DCO_resomod_dco_rt_s.new_period_cnt_u32 = 1;
   DCO_resomod_dco_rt_s.new_PWM_dc_cnt_u32 = 1;
   DCO_resomod_dco_rt_s.volume_mod_lfo_i16 = 0;
   DCO_vSetFreq(&DCO_resomod_dco_rt_s, &DCO_vcf_lfo_s.resomod_dco_set_s, 1 * 1000, SYNENG_CTRL_FREQ);
   DCO_vSetPWM(&DCO_resomod_dco_rt_s, &DCO_vcf_lfo_s.resomod_dco_set_s, DCO_PWM_RANGE / 2);

   DCO_dco_set_as[0].level_u8 = 127;
#if 0
#if defined(_WIN32) || defined(__WXMAC__)
   srand(time(NULL));
#endif
#endif

}

   
void DCO_vSetFreq(DCO_rt_ts *rt_ps, DCO_set_ts *set_ps, uint32_t mHz_u32, uint32_t sample_freq_hz_u32)
{
   int8_t scale = set_ps->scale_i8;
   if(scale == 1)
   {
      set_ps->freq_mHz_u32 = mHz_u32;
   }
   else
   {
      if(scale == 0)
      {
         set_ps->freq_mHz_u32 = set_ps->fix_freq_hz_u32 * 1000;
      }
      else   
      if(scale >= 1)
      {
         set_ps->freq_mHz_u32 = mHz_u32 * scale;
      }      
      else
      {
         scale = -scale;
         set_ps->freq_mHz_u32 = mHz_u32 / scale;
      }   
 
   }

   if(set_ps->freq_mHz_u32 < 5)
   {
      set_ps->freq_mHz_u32 = 5;
   }
   rt_ps->new_period_cnt_u32 = ((((float)sample_freq_hz_u32 * 1000)) / (float)set_ps->freq_mHz_u32);
   rt_ps->new_sub_osc_period_cnt_u32 = rt_ps->new_period_cnt_u32 * 2;
   if(rt_ps->new_period_cnt_u32 == 0)
   {
      rt_ps->new_period_cnt_u32 = 1;
   }
   

   DCO_vSetPWM(rt_ps, set_ps, set_ps->pwm_dc_u16);
}


void DCO_vSetPWM(DCO_rt_ts *rt_ps, DCO_set_ts *set_ps, uint16_t dc_u16)
{
   if(!dc_u16)
   {
      dc_u16 = (DCO_PWM_RANGE / 95);
   }
   else
   if(dc_u16 >= DCO_PWM_RANGE)
   {
      dc_u16 = DCO_PWM_RANGE - (DCO_PWM_RANGE / 95);
   }      
   set_ps->pwm_dc_u16 = dc_u16;
   rt_ps->new_PWM_dc_cnt_u32 = (rt_ps->new_period_cnt_u32 * set_ps->pwm_dc_u16) / DCO_PWM_RANGE;
}

static uint32_t DCO_u32GenerateRandomNumber(void)
{
   /* Change this for different random sequences. */
   static uint32_t randSeed = 22222;
   randSeed = (randSeed * 196314165) + 907633515;
   return randSeed;
}

float DCO_i16GetNextDCOValue(DCO_rt_ts *rt_ps, DCO_set_ts *set_ps)
{
   return DCO_i16GetNextValue(rt_ps, set_ps, NULL, NULL);
}

float DCO_i16GetNextLFOValue(DCO_rt_ts *rt_ps, DCO_set_ts *set_ps)
{
   float retval = 0;

   if(rt_ps->period_cnt_u32)
   {
      switch(set_ps->wave_e)
      {
         case DCO_SAW:
            retval = (((float)(DCO_OUTPUT_RANGE * rt_ps->cnt_u32)) / rt_ps->period_cnt_u32) - 32768;
         break;
         case DCO_PWM:
            if(rt_ps->cnt_u32 < rt_ps->PWM_dc_cnt_u32)
            {
               retval = 32767;
            }
            else
            {
               retval = -32768;
            }
         break;
         case DCO_TRI:
            if(rt_ps->cnt_u32 < (rt_ps->period_cnt_u32 / 2))
            {
               retval = ((((float)(DCO_OUTPUT_RANGE * 2) * (float)rt_ps->cnt_u32) / rt_ps->period_cnt_u32)) - 32768;
            }
            else
            {
               retval = -((((float)(DCO_OUTPUT_RANGE * 2) * ((float)rt_ps->cnt_u32 - (rt_ps->period_cnt_u32 / 2))) / rt_ps->period_cnt_u32) - 32767);
            }
         break;
         case DCO_SIN:
            retval = (float)sin( (double)((((rt_ps->cnt_u32 * 360) / rt_ps->period_cnt_u32) * M_PI) / 180) ) * (float)DCO_OUTPUT_RANGE;
         break;
         case DCO_NOISE:
            if(rt_ps->noise_cnt_u16 >= (rt_ps->period_cnt_u32 / 512))
            {
               rt_ps->noise_cnt_u16 = 0;
               retval = (float)((int16_t)DCO_u32GenerateRandomNumber());
               rt_ps->lastnoise_i16 = retval;
            }
            else
            {
               retval = rt_ps->lastnoise_i16;
               rt_ps->noise_cnt_u16++;
            }       
         break;
      }
   }
   else
   {
      retval = 0;
   }
   
   rt_ps->cnt_u32 += 1;
   if(rt_ps->cnt_u32 >= rt_ps->period_cnt_u32)
   {
      rt_ps->cnt_u32 = 0;
      //rt_ps->cnt_u32 -= rt_ps->period_cnt_u32;
      rt_ps->period_cnt_u32 = rt_ps->new_period_cnt_u32;
      rt_ps->PWM_dc_cnt_u32 = rt_ps->new_PWM_dc_cnt_u32;
      rt_ps->new_period_u8 = 1;
   }

   return retval;
}


float DCO_i16GetNextValue(DCO_rt_ts *rt_ps, DCO_set_ts *set_ps, DADSR_runtime_data_ts *dadsr_volume_ps, DCO_set_ts *volmod_lfo_ps)
{
   float retval = 0;

   if(rt_ps->period_cnt_u32)
   {
      switch(set_ps->wave_e)
      {
         case DCO_SAW:
            retval = (((float)(DCO_OUTPUT_RANGE * rt_ps->cnt_u32)) / rt_ps->period_cnt_u32) - 32768;
         break;
         case DCO_PWM:
            if(rt_ps->cnt_u32 < rt_ps->PWM_dc_cnt_u32)
            {
               retval = 32767;
            }
            else
            {
               retval = -32768;
            }
         break;
         case DCO_TRI:
            if(rt_ps->cnt_u32 < (rt_ps->period_cnt_u32 / 2))
            {
               retval = ((((float)(DCO_OUTPUT_RANGE * 2) * (float)rt_ps->cnt_u32) / rt_ps->period_cnt_u32)) - 32768;
            }
            else
            {
               retval = -((((float)(DCO_OUTPUT_RANGE * 2) * ((float)rt_ps->cnt_u32 - (rt_ps->period_cnt_u32 / 2))) / rt_ps->period_cnt_u32) - 32767);
            }
         break;
         case DCO_SIN:
            retval = (float)sin( (double)((((rt_ps->cnt_u32 * 360) / rt_ps->period_cnt_u32) * M_PI) / 180) ) * (float)DCO_OUTPUT_RANGE;
         break;
         case DCO_NOISE:
            if(rt_ps->noise_cnt_u16 >= (rt_ps->period_cnt_u32 / 512))
            {
               rt_ps->noise_cnt_u16 = 0;
               retval = (float)((int16_t)DCO_u32GenerateRandomNumber());
               rt_ps->lastnoise_i16 = retval;
            }
            else
            {
               retval = rt_ps->lastnoise_i16;
               rt_ps->noise_cnt_u16++;
            }       
         break;
      }
      if(dadsr_volume_ps)
      {
         retval = ((retval * DADSRH_u16GetOutput(dadsr_volume_ps)) / DADSR_MAX_OUTPUT);
      }
      if(volmod_lfo_ps)
      {
         if(volmod_lfo_ps->level_u8)
         {
            retval = (retval * (rt_ps->volume_mod_lfo_i16 + 32768)) / 65536;            
         }
      }
   }
   else
   {
      retval = 0;
   }
   
   if(set_ps->sub_osc_level_u8)
   {
      float out = ((float)(retval * (255 - set_ps->sub_osc_level_u8))) / 256;
     
      
      if(rt_ps->sub_osc_cnt_u32 < (rt_ps->sub_osc_period_cnt_u32 / 2))
      {
         out += ((float)(32767 * set_ps->sub_osc_level_u8)) / 256;
      }
      else
      {
         out += ((float)(-32768 * set_ps->sub_osc_level_u8)) / 256;
      }      
      
      //retval = (out >> 1);
      retval = out;

      rt_ps->sub_osc_cnt_u32 += 1;
      if(rt_ps->sub_osc_cnt_u32 >= rt_ps->sub_osc_period_cnt_u32)
      {
         rt_ps->sub_osc_cnt_u32 = 0;
         rt_ps->sub_osc_period_cnt_u32 = rt_ps->new_sub_osc_period_cnt_u32;
      }   
   }
   
   rt_ps->cnt_u32 += 1;
   if(rt_ps->cnt_u32 >= rt_ps->period_cnt_u32)
   {
      //rt_ps->cnt_u32 = 0;
      rt_ps->cnt_u32 -= rt_ps->period_cnt_u32;
      rt_ps->period_cnt_u32 = rt_ps->new_period_cnt_u32;
      rt_ps->PWM_dc_cnt_u32 = rt_ps->new_PWM_dc_cnt_u32;
      rt_ps->new_period_u8 = 1;
   }

   return retval;
}

float DCO_i16GetNextFMValue(DCO_rt_ts *rt_ps, DCO_set_ts *set_ps, float modulator_i16, DADSR_runtime_data_ts *dadsr_volume_ps, DCO_set_ts *volmod_lfo_ps)
{
   float retval = 0;

   if(rt_ps->period_cnt_u32)
   {
      switch(set_ps->wave_e)
      {
         case DCO_SAW:
            {
               float pos_i32;
               pos_i32 = (float)rt_ps->cnt_u32 + (((float)modulator_i16 * (float)rt_ps->period_cnt_u32) / 65536);
               while(pos_i32 < 0)
               {
                  pos_i32 += rt_ps->period_cnt_u32;
               }
               while(pos_i32 >= rt_ps->period_cnt_u32)
               {
                  pos_i32 -= rt_ps->period_cnt_u32;
               }      
               retval = (((float)(DCO_OUTPUT_RANGE * pos_i32)) / rt_ps->period_cnt_u32) - 32768;
            }   
         break;
         case DCO_PWM:
           {
               float pos_i32;
               pos_i32 = rt_ps->cnt_u32;
               pos_i32 += (((float)modulator_i16 * (float)rt_ps->period_cnt_u32) / 65536 );
               while(pos_i32 < 0)
               {
                  pos_i32 += rt_ps->period_cnt_u32;
               }
               while(pos_i32 >= rt_ps->period_cnt_u32)
               {
                  pos_i32 -= rt_ps->period_cnt_u32;
               }      
               if(pos_i32 < rt_ps->PWM_dc_cnt_u32)
               {
                  retval = 32767;
               }
               else
               {
                  retval = -32768;
               }
           } 
         break;
         case DCO_TRI:
           {
               float pos_i32;
               pos_i32 = (float)rt_ps->cnt_u32;
               pos_i32 += ((float)modulator_i16 * rt_ps->period_cnt_u32) / 65536;
               while(pos_i32 < 0)
               {
                  pos_i32 += rt_ps->period_cnt_u32;
               }
               while((uint32_t)pos_i32 >= rt_ps->period_cnt_u32)
               {
                  pos_i32 -= rt_ps->period_cnt_u32;
               }      
   
               if((uint32_t)pos_i32 < (rt_ps->period_cnt_u32 / 2))
               {
                  retval = ((((float)(DCO_OUTPUT_RANGE * 2) * pos_i32) / rt_ps->period_cnt_u32)) - 32768;
               }
               else
               {
                  retval = -((((float)(DCO_OUTPUT_RANGE * 2) * (pos_i32 - (rt_ps->period_cnt_u32 / 2))) / rt_ps->period_cnt_u32) - 32767);
               }
           }    
         break;
         case DCO_SIN:
            {
               float pos_i32;

               pos_i32 = ((rt_ps->cnt_u32 * 360) / rt_ps->period_cnt_u32) + ((modulator_i16 * 360) / 32768);
               while(pos_i32 < 0)
               {
                  pos_i32 += 360;
               }
               while(pos_i32 >= 360)
               {
                  pos_i32 -= 360;
               }      

               retval = (float)sin( (double)((pos_i32 * M_PI) / 180) ) * (float)DCO_OUTPUT_RANGE;
               
            }   
         break;
         case DCO_NOISE:
            if(rt_ps->noise_cnt_u16 >= (rt_ps->period_cnt_u32 / 512))
            {
               rt_ps->noise_cnt_u16 = 0;
               retval = (float)((int16_t)DCO_u32GenerateRandomNumber());
               rt_ps->lastnoise_i16 = retval;
            }
            else
            {
               retval = rt_ps->lastnoise_i16;
               rt_ps->noise_cnt_u16++;
            }              
         break;
      }
      if(volmod_lfo_ps)
      {
         if(volmod_lfo_ps->level_u8)
         {
            retval = (retval * (rt_ps->volume_mod_lfo_i16 + 32768)) / 65536;            
         }
      }
      if(dadsr_volume_ps)
      {
         retval = ((retval * DADSRH_u16GetOutput(dadsr_volume_ps)) / DADSR_MAX_OUTPUT );
      }
   }
   else
   {
      retval = 0;
   }
   
   if(set_ps->sub_osc_level_u8)
   {
      float out = (retval * (255 - set_ps->sub_osc_level_u8)) / 256;
     
      
      if(rt_ps->sub_osc_cnt_u32 < (rt_ps->sub_osc_period_cnt_u32 / 2))
      {
         out += (32767 * set_ps->sub_osc_level_u8) / 256;
      }
      else
      {
         out += (-32768 * set_ps->sub_osc_level_u8) / 256;
      }      
      
      //retval = (out >> 1);
      retval = out;

      rt_ps->sub_osc_cnt_u32 += 1;
      if(rt_ps->sub_osc_cnt_u32 >= rt_ps->sub_osc_period_cnt_u32)
      {
         rt_ps->sub_osc_cnt_u32 = 0;
         rt_ps->sub_osc_period_cnt_u32 = rt_ps->new_sub_osc_period_cnt_u32;
      }   
   }

   rt_ps->cnt_u32 += 1;
   if(rt_ps->cnt_u32 >= rt_ps->period_cnt_u32)
   {
      //rt_ps->cnt_u32 = 0;
      rt_ps->cnt_u32 -= rt_ps->period_cnt_u32;
      rt_ps->period_cnt_u32 = rt_ps->new_period_cnt_u32;
      rt_ps->PWM_dc_cnt_u32 = rt_ps->new_PWM_dc_cnt_u32;
      rt_ps->new_period_u8 = 1;
   }

   return retval;
}

float DCO_i32GetDCOsOutput(void)
{
   float retval = 0;
   uint8_t i;
   float t_i16;
   float t_i32;
   
   switch(DCO_synmode_e)
   {
      case DCO_FM:
         switch(DCO_fm_algorithm_u8)
         {
            case 0:
               retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], (DCO_i16GetNextValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]) * DCO_dco_set_as[1].level_u8) / 128, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * DCO_dco_set_as[0].level_u8) * 2) / 128;
               retval += ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[2], &DCO_dco_set_as[2], (DCO_i16GetNextValue(&DCO_dco_rt_as[3], &DCO_dco_set_as[3], &DADSR_dcoout_rt_as[3], &DCO_volmod_set_as[3]) * DCO_dco_set_as[3].level_u8) / 128, &DADSR_dcoout_rt_as[2], &DCO_volmod_set_as[2]) * DCO_dco_set_as[2].level_u8) * 2) / 128;
            break;
            case 1:
               retval = ((DCO_i16GetNextValue(&DCO_dco_rt_as[3], &DCO_dco_set_as[3], &DADSR_dcoout_rt_as[3], &DCO_volmod_set_as[3]) * DCO_dco_set_as[3].level_u8) / 128);
               retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[2], &DCO_dco_set_as[2], retval, &DADSR_dcoout_rt_as[2], &DCO_volmod_set_as[2]) * DCO_dco_set_as[2].level_u8)) / 128;
               retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], retval, &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]) * DCO_dco_set_as[1].level_u8)) / 128;
               retval = (((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], retval, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * 4) * DCO_dco_set_as[0].level_u8)) / 128;              
            break;
            case 2:
               {
                  float t2;
                  DCO_dco_rt_as[0].lastoutput_i16 = DCO_i16GetNextFMValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], ((DCO_dco_rt_as[0].lastoutput_i16 * DCO_fm_feedback_u8) / 128), &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]) / 2;
                  t2 = (DCO_dco_rt_as[0].lastoutput_i16 * DCO_dco_set_as[1].level_u8) / 128;
                  retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], t2, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * DCO_dco_set_as[0].level_u8) * 2) / 128;
                  retval += ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[2], &DCO_dco_set_as[2], (DCO_i16GetNextValue(&DCO_dco_rt_as[3], &DCO_dco_set_as[3], &DADSR_dcoout_rt_as[3], &DCO_volmod_set_as[3]) * DCO_dco_set_as[3].level_u8) / 128, &DADSR_dcoout_rt_as[2], &DCO_volmod_set_as[2]) * DCO_dco_set_as[2].level_u8) * 2) / 128;
               }
            break;
            case 3:
               retval = ((DCO_i16GetNextValue(&DCO_dco_rt_as[2], &DCO_dco_set_as[2], &DADSR_dcoout_rt_as[2], &DCO_volmod_set_as[2]) * DCO_dco_set_as[2].level_u8) / 128);
               retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], retval, &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]) * DCO_dco_set_as[1].level_u8)) / 128;
               retval = (((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], retval, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * 2) * DCO_dco_set_as[0].level_u8)) / 128;              
               retval += ((DCO_i16GetNextValue(&DCO_dco_rt_as[3], &DCO_dco_set_as[3], &DADSR_dcoout_rt_as[3], &DCO_volmod_set_as[3]) * 2) * DCO_dco_set_as[3].level_u8) / 128; 
            break;
            case 4:
               {
                  float t2;
                  DCO_dco_rt_as[0].lastoutput_i16 = DCO_i16GetNextFMValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], ((DCO_dco_rt_as[0].lastoutput_i16 * DCO_fm_feedback_u8) / 128), &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]) / 2;
                  t2 = (DCO_dco_rt_as[0].lastoutput_i16 * DCO_dco_set_as[1].level_u8) / 128;
                  retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], t2, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * DCO_dco_set_as[0].level_u8) * 2) / 128;
                  for(i = 2; i < DCO_MAX_DCOS; i++)
                  {
                     retval += (DCO_i16GetNextValue(&DCO_dco_rt_as[i], &DCO_dco_set_as[i], &DADSR_dcoout_rt_as[i], &DCO_volmod_set_as[i]) * DCO_dco_set_as[i].level_u8) / 128; 
                  }
               }
            break;
            case 5:
               {
                  float t2;
                  DCO_dco_rt_as[3].lastoutput_i16 = DCO_i16GetNextFMValue(&DCO_dco_rt_as[3], &DCO_dco_set_as[3], ((DCO_dco_rt_as[3].lastoutput_i16 * DCO_fm_feedback_u8) / 128), &DADSR_dcoout_rt_as[3], &DCO_volmod_set_as[3]) / 2;
                  t2 = (DCO_dco_rt_as[3].lastoutput_i16 * DCO_dco_set_as[3].level_u8) / 128;
                  retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[2], &DCO_dco_set_as[2], t2, &DADSR_dcoout_rt_as[2], &DCO_volmod_set_as[2]) * DCO_dco_set_as[2].level_u8) * 2) / 128;
                  retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], retval, &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]) * DCO_dco_set_as[1].level_u8)) / 128;
                  retval = (((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], retval, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * 4) * DCO_dco_set_as[0].level_u8)) / 128;              

               }
            break;
            case 6:
               {
                  float t2;
                  DCO_dco_rt_as[2].lastoutput_i16 = DCO_i16GetNextFMValue(&DCO_dco_rt_as[2], &DCO_dco_set_as[2], ((DCO_dco_rt_as[2].lastoutput_i16 * DCO_fm_feedback_u8) / 128), &DADSR_dcoout_rt_as[2], &DCO_volmod_set_as[2]) / 2;
                  t2 = (DCO_dco_rt_as[2].lastoutput_i16 * DCO_dco_set_as[2].level_u8) / 128;
                  retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], t2, &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]) * DCO_dco_set_as[1].level_u8) * 2) / 128;
                  retval = (((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], retval, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * 2) * DCO_dco_set_as[0].level_u8)) / 128;              
                  retval += ((DCO_i16GetNextValue(&DCO_dco_rt_as[3], &DCO_dco_set_as[3], &DADSR_dcoout_rt_as[3], &DCO_volmod_set_as[3]) * 2) * DCO_dco_set_as[3].level_u8) / 128; 
               }
            break;
            case 7:
               {
                  float t2;
                  DCO_dco_rt_as[0].lastoutput_i16 = DCO_i16GetNextFMValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], ((DCO_dco_rt_as[0].lastoutput_i16 * DCO_fm_feedback_u8) / 128), &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]) / 2;
                  t2 = (DCO_dco_rt_as[0].lastoutput_i16 * DCO_dco_set_as[1].level_u8) / 128;
                  retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], t2, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * DCO_dco_set_as[0].level_u8) * 2) / 128;
                  
                  DCO_dco_rt_as[2].lastoutput_i16 = DCO_i16GetNextFMValue(&DCO_dco_rt_as[3], &DCO_dco_set_as[3], ((DCO_dco_rt_as[2].lastoutput_i16 * DCO_fm_feedback_u8) / 128), &DADSR_dcoout_rt_as[3], &DCO_volmod_set_as[3]) / 2;
                  t2 = (DCO_dco_rt_as[2].lastoutput_i16 * DCO_dco_set_as[3].level_u8) / 128;
                  retval += ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[2], &DCO_dco_set_as[2], t2, &DADSR_dcoout_rt_as[2], &DCO_volmod_set_as[2]) * DCO_dco_set_as[2].level_u8) * 2) / 128;
               }
            break;
            case 8:
               retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], (DCO_i16GetNextFMValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], DCO_fm_feedback_sig_i16, &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]) * DCO_dco_set_as[1].level_u8) / 128, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * DCO_dco_set_as[0].level_u8) * 2) / 128;
               DCO_fm_feedback_sig_i16 = ((retval * DCO_fm_feedback_u8) / 128);
               retval += ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[2], &DCO_dco_set_as[2], (DCO_i16GetNextValue(&DCO_dco_rt_as[3], &DCO_dco_set_as[3], &DADSR_dcoout_rt_as[3], &DCO_volmod_set_as[3]) * DCO_dco_set_as[3].level_u8) / 128, &DADSR_dcoout_rt_as[2], &DCO_volmod_set_as[2]) * DCO_dco_set_as[2].level_u8) * 2) / 128;
            break;
            case 9:
               retval = (((DCO_i16GetNextValue(&DCO_dco_rt_as[2], &DCO_dco_set_as[2], &DADSR_dcoout_rt_as[2], &DCO_volmod_set_as[2]) * 2) * DCO_dco_set_as[2].level_u8) / 128);
               retval += ((DCO_i16GetNextValue(&DCO_dco_rt_as[3], &DCO_dco_set_as[3], &DADSR_dcoout_rt_as[3], &DCO_volmod_set_as[3]) * 2) * DCO_dco_set_as[3].level_u8) / 128; 
               retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], retval, &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]) * DCO_dco_set_as[1].level_u8)) / 128;
               retval = (((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], retval, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * 4) * DCO_dco_set_as[0].level_u8)) / 128;              
            break;
            case 10:
               retval = (((DCO_i16GetNextValue(&DCO_dco_rt_as[2], &DCO_dco_set_as[2], &DADSR_dcoout_rt_as[2], &DCO_volmod_set_as[2])) * DCO_dco_set_as[2].level_u8) / 128);
               retval += ((DCO_i16GetNextValue(&DCO_dco_rt_as[3], &DCO_dco_set_as[3], &DADSR_dcoout_rt_as[3], &DCO_volmod_set_as[3])) * DCO_dco_set_as[3].level_u8) / 128; 
               retval += ((DCO_i16GetNextValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1])) * DCO_dco_set_as[1].level_u8) / 128; 
               retval = (((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], retval, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * 4) * DCO_dco_set_as[0].level_u8)) / 128;              
            break;
            case 11:
               {
                  float t2;
                  t2 = DCO_dco_rt_as[0].lastoutput_i16;
                  DCO_dco_rt_as[0].lastoutput_i16 = DCO_i16GetNextFMValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], ((t2 * DCO_fm_feedback_u8) / 128), &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]) / 2;
                  DCO_dco_rt_as[2].lastoutput_i16 = DCO_i16GetNextFMValue(&DCO_dco_rt_as[3], &DCO_dco_set_as[3], ((t2 * DCO_fm_feedback_u8) / 128), &DADSR_dcoout_rt_as[3], &DCO_volmod_set_as[3]) / 2;
                  t2 = (DCO_dco_rt_as[0].lastoutput_i16 * DCO_dco_set_as[1].level_u8) / 128;
                  t2 += (DCO_dco_rt_as[2].lastoutput_i16 * DCO_dco_set_as[3].level_u8) / 128;
 
                  retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], t2, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * DCO_dco_set_as[0].level_u8) * 2) / 128;
                  retval += ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[2], &DCO_dco_set_as[2], t2, &DADSR_dcoout_rt_as[2], &DCO_volmod_set_as[2]) * DCO_dco_set_as[2].level_u8) * 2) / 128;
                  DCO_dco_rt_as[0].lastoutput_i16 = t2;
               }
            break;
            case 12:
               {
                  float t2;
                  t2 = DCO_i16GetNextFMValue(&DCO_dco_rt_as[3], &DCO_dco_set_as[3], ((DCO_dco_rt_as[3].lastoutput_i16 * DCO_fm_feedback_u8) / 128), &DADSR_dcoout_rt_as[3], &DCO_volmod_set_as[3]) / 2;
                  t2 = (t2 * DCO_dco_set_as[3].level_u8) / 128;
                  retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[2], &DCO_dco_set_as[2], t2, &DADSR_dcoout_rt_as[2], &DCO_volmod_set_as[2]) * DCO_dco_set_as[2].level_u8) * 2) / 128;
                  retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], retval, &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]) * DCO_dco_set_as[1].level_u8)) / 128;
                  DCO_dco_rt_as[3].lastoutput_i16 = retval;
                  retval = (((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], retval, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * 4) * DCO_dco_set_as[0].level_u8)) / 128;              
               }
            break;
            case 13:
               retval = (((DCO_i16GetNextValue(&DCO_dco_rt_as[2], &DCO_dco_set_as[2], &DADSR_dcoout_rt_as[2], &DCO_volmod_set_as[2]) * 2) * DCO_dco_set_as[2].level_u8) / 128);
               retval = ((DCO_i16GetNextFMValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], retval, &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]) * DCO_dco_set_as[1].level_u8)) / 128;
               retval += ((DCO_i16GetNextValue(&DCO_dco_rt_as[3], &DCO_dco_set_as[3], &DADSR_dcoout_rt_as[3], &DCO_volmod_set_as[3]) * 2) * DCO_dco_set_as[3].level_u8) / 128; 
               retval = (((DCO_i16GetNextFMValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], retval, &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]) * 4) * DCO_dco_set_as[0].level_u8)) / 128;              
            break;
         }
      break;
      case DCO_12RINGMOD:
         t_i16 = DCO_i16GetNextValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]);
         if(t_i16 < 0)
         {
            if(DCO_dco_rt_as[0].lastoutput_i16 > 0)
            {
               DCO_dco_rt_as[0].zero_crossed_u8 = 1;
            }   
         }   
         DCO_dco_rt_as[0].lastoutput_i16 = t_i16;
         t_i16 = (t_i16 * DCO_dco_set_as[0].level_u8) / 128;
         retval = t_i16;
         if(DCO_dco_set_as[1].level_u8)
         {
            t_i16 = DCO_i16GetNextValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]);
            if(t_i16 < 0)
            {
               if(DCO_dco_rt_as[1].lastoutput_i16 > 0)
               {
                  DCO_dco_rt_as[1].zero_crossed_u8 = 1;
               }   
            }   
            DCO_dco_rt_as[1].lastoutput_i16 = t_i16;
            t_i16 = (DCO_dco_rt_as[1].lastoutput_i16 * DCO_dco_set_as[1].level_u8) / 128;

            //retval = (retval * t) / 32768;
            retval = (retval * t_i16) / 16384;
         }

         for(i = 2; i < DCO_MAX_DCOS; i++)
         {
            t_i16 = DCO_i16GetNextValue(&DCO_dco_rt_as[i], &DCO_dco_set_as[i], &DADSR_dcoout_rt_as[i], &DCO_volmod_set_as[i]);
            if(t_i16 < 0)
            {
               if(DCO_dco_rt_as[i].lastoutput_i16 > 0)
               {
                  DCO_dco_rt_as[i].zero_crossed_u8 = 1;
               }   
            }   
            DCO_dco_rt_as[i].lastoutput_i16 = t_i16;
            retval += (DCO_dco_rt_as[i].lastoutput_i16 * DCO_dco_set_as[i].level_u8) / 128;
         }
      break;
      case DCO_1234RINGMOD:
         t_i16 = DCO_i16GetNextValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]);
         if(t_i16 < 0)
         {
            if(DCO_dco_rt_as[0].lastoutput_i16 > 0)
            {
               DCO_dco_rt_as[0].zero_crossed_u8 = 1;
            }   
         }   
         DCO_dco_rt_as[0].lastoutput_i16 = t_i16;
         t_i16 = (t_i16 * DCO_dco_set_as[0].level_u8) / 128;
         retval = t_i16 * 4;
         for(i = 1; i < DCO_MAX_DCOS; i++)
         {
            if(DCO_dco_set_as[i].level_u8)
            {
               t_i16 = DCO_i16GetNextValue(&DCO_dco_rt_as[i], &DCO_dco_set_as[i], &DADSR_dcoout_rt_as[i], &DCO_volmod_set_as[i]);
               if(t_i16 < 0)
               {
                  if(DCO_dco_rt_as[i].lastoutput_i16 > 0)
                  {
                     DCO_dco_rt_as[i].zero_crossed_u8 = 1;
                  }   
               }   
               DCO_dco_rt_as[i].lastoutput_i16 = t_i16;
               t_i16 = (DCO_dco_rt_as[i].lastoutput_i16 * DCO_dco_set_as[i].level_u8) / 128;
   
               //retval = (retval * t) / 32768;
               retval = (retval * t_i16) / 32768;
            }
        }
      break;
      case DCO_ADD:
         retval = 0;
         for(i = 0; i < DCO_MAX_DCOS; i++)
         {
            t_i16 = DCO_i16GetNextValue(&DCO_dco_rt_as[i], &DCO_dco_set_as[i], &DADSR_dcoout_rt_as[i], &DCO_volmod_set_as[i]);
            if(t_i16 < 0)
            {
               if(DCO_dco_rt_as[i].lastoutput_i16 > 0)
               {
                  DCO_dco_rt_as[i].zero_crossed_u8 = 1;
               }   
            }   
            DCO_dco_rt_as[i].lastoutput_i16 = t_i16;
            retval += (DCO_dco_rt_as[i].lastoutput_i16 * DCO_dco_set_as[i].level_u8) / 128;
         }
      break;
      case DCO_12XOR:
         t_i16 = DCO_i16GetNextValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]);
         if(t_i16 < 0)
         {
            if(DCO_dco_rt_as[0].lastoutput_i16 > 0)
            {
               DCO_dco_rt_as[0].zero_crossed_u8 = 1;
            }   
         }   
         DCO_dco_rt_as[0].lastoutput_i16 = t_i16;
         t_i16 = (t_i16 * DCO_dco_set_as[0].level_u8) / 128;
         retval = t_i16 * 2;
         if(DCO_dco_set_as[1].level_u8)
         {
            t_i16 = DCO_i16GetNextValue(&DCO_dco_rt_as[1], &DCO_dco_set_as[1], &DADSR_dcoout_rt_as[1], &DCO_volmod_set_as[1]);
            if(t_i16 < 0)
            {
               if(DCO_dco_rt_as[1].lastoutput_i16 > 0)
               {
                  DCO_dco_rt_as[1].zero_crossed_u8 = 1;
               }   
            }   
            DCO_dco_rt_as[1].lastoutput_i16 = t_i16;
            t_i16 = (DCO_dco_rt_as[1].lastoutput_i16 * DCO_dco_set_as[1].level_u8) / 128;

            retval = (float)((int16_t)retval ^ (int16_t)t_i16);
         }

         for(i = 2; i < DCO_MAX_DCOS; i++)
         {
            t_i16 = DCO_i16GetNextValue(&DCO_dco_rt_as[i], &DCO_dco_set_as[i], &DADSR_dcoout_rt_as[i], &DCO_volmod_set_as[i]);
            if(t_i16 < 0)
            {
               if(DCO_dco_rt_as[i].lastoutput_i16 > 0)
               {
                  DCO_dco_rt_as[i].zero_crossed_u8 = 1;
               }   
            }   
            DCO_dco_rt_as[i].lastoutput_i16 = t_i16;
            retval += (DCO_dco_rt_as[i].lastoutput_i16 * DCO_dco_set_as[i].level_u8) / 128;
         }
      break;
      case DCO_1234XOR:
         t_i16 = DCO_i16GetNextValue(&DCO_dco_rt_as[0], &DCO_dco_set_as[0], &DADSR_dcoout_rt_as[0], &DCO_volmod_set_as[0]);
         if(t_i16 < 0)
         {
            if(DCO_dco_rt_as[0].lastoutput_i16 > 0)
            {
               DCO_dco_rt_as[0].zero_crossed_u8 = 1;
            }   
         }   
         DCO_dco_rt_as[0].lastoutput_i16 = t_i16;
         t_i16 = (t_i16 * DCO_dco_set_as[0].level_u8) / 128;
         retval = t_i16 * 4;    
         for(i = 1; i < DCO_MAX_DCOS; i++)
         {
            if(DCO_dco_set_as[i].level_u8)
            {
               t_i16 = DCO_i16GetNextValue(&DCO_dco_rt_as[i], &DCO_dco_set_as[i], &DADSR_dcoout_rt_as[i], &DCO_volmod_set_as[i]);
               if(t_i16 < 0)
               {
                  if(DCO_dco_rt_as[i].lastoutput_i16 > 0)
                  {
                     DCO_dco_rt_as[i].zero_crossed_u8 = 1;
                  }   
               }   
               DCO_dco_rt_as[i].lastoutput_i16 = t_i16;
               t_i16 = (DCO_dco_rt_as[i].lastoutput_i16 * DCO_dco_set_as[i].level_u8) / 128;
               retval = (float)((int16_t)retval ^ (int16_t)t_i16);
            }
        }
      break;
      default:
      break;
   }   
   
   if(DCO_synmode_e != DCO_NOSYNC)
   {
      for(i = 0; i < DCO_MAX_DCOS; i++)
      {
         switch(DCO_dco_set_as[i].sync_e)
         {
            case DCO_SYNC1:
               if(DCO_dco_rt_as[0].zero_crossed_u8 == 1)
               {
                  DCO_dco_rt_as[i].cnt_u32 = 0;
                  DCO_dco_rt_as[i].period_cnt_u32 = DCO_dco_rt_as[i].new_period_cnt_u32;
               }   
            break;
            case DCO_SYNC2:
               if(DCO_dco_rt_as[1].zero_crossed_u8 == 1)
               {
                  DCO_dco_rt_as[i].cnt_u32 = 0;
                  DCO_dco_rt_as[i].period_cnt_u32 = DCO_dco_rt_as[i].new_period_cnt_u32;
               }   
            break;
            case DCO_SYNC3:
               if(DCO_dco_rt_as[2].zero_crossed_u8 == 1)
               {
                  DCO_dco_rt_as[i].cnt_u32 = 0;
                  DCO_dco_rt_as[i].period_cnt_u32 = DCO_dco_rt_as[i].new_period_cnt_u32;
               }   
            break;
            case DCO_SYNC4:
               if(DCO_dco_rt_as[3].zero_crossed_u8 == 1)
               {
                  DCO_dco_rt_as[i].cnt_u32 = 0;
                  DCO_dco_rt_as[i].period_cnt_u32 = DCO_dco_rt_as[i].new_period_cnt_u32;
               }            
            break;
            case DCO_NOSYNC:
            break;
            default:
            break;  
         }   
      }
   }
      
   //DCO_mod_calc_cnt_u16++;
   //if(DCO_mod_calc_cnt_u16 >= DCO_LFO_CALC_INTERVAL_DIV)
   {
      DCO_mod_calc_cnt_u16 = 0;
      for(i = 0; i < DCO_MAX_DCOS; i++)
      {
         DCO_pwm_mod_output_ai16[i] = DCO_i16GetNextLFOValue(&DCO_pwmmod_rt_as[i], &DCO_pwmmod_set_as[i]);
         DCO_freq_mod_output_ai16[i] = DCO_i16GetNextLFOValue(&DCO_freqmod_rt_as[i], &DCO_freqmod_set_as[i]);
         if(DCO_volmod_set_as[i].level_u8)
         {
            DCO_dco_rt_as[i].volume_mod_lfo_i16 = (((float)DCO_i16GetNextLFOValue(&DCO_volmod_rt_as[i], &DCO_volmod_set_as[i]) * (float)DCO_volmod_set_as[i].level_u8) / 127);
         }
         else
         {
            DCO_dco_rt_as[i].volume_mod_lfo_i16 = 0;
         }
      }
   }
      
   for(i = 0; i < DCO_MAX_DCOS; i++)
   {
      DCO_dco_rt_as[i].zero_crossed_u8 = 0;
      
      if(DCO_dco_rt_as[i].new_period_u8)
      {  
         DCO_dco_rt_as[i].new_period_u8 = 0;
         
         // do freq mod
         t_i32 = (float)DCO_dco_set_as[i].freq_mHz_u32;

         t_i32 = (t_i32 * (DCO_master_tune_i8 + 1024)) / 1024;

         if(DCO_freqmod_set_as[i].level_u8)
         { 
            t_i32 += ((DCO_freq_mod_output_ai16[i] * (DCO_dco_set_as[i].freq_mHz_u32 / 3) / DCO_PWM_RANGE) * DCO_freqmod_set_as[i].level_u8) / 128;       
         }

         if(DCO_dco_set_as[i].freq_dadsr_mod_type_e != SYNENG_NOMOD)
         {
            switch(DCO_dco_set_as[i].freq_dadsr_mod_type_e)
            {
               case SYNENG_NMOD:
                  t_i32 -= (((DADSRH_u16GetOutput(&DADSR_freqmod_rt_as[i]) * (DCO_dco_set_as[i].freq_mHz_u32) ) / 32768 /* / DADSR_MAX_OUTPUT */ ));
               break;
               case SYNENG_PMOD:
                  t_i32 += (((DADSRH_u16GetOutput(&DADSR_freqmod_rt_as[i]) * (DCO_dco_set_as[i].freq_mHz_u32) ) / 32768 /* / DADSR_MAX_OUTPUT */));
               break;
               case SYNENG_NOMOD:
               break;
            }
         }
 
         if(t_i32 < (10 * 1000))
         {
            t_i32 = (10 * 1000);
         }

         if(t_i32 < (DCO_MAX_FREQ_HZ*1000))
         {
            DCO_dco_rt_as[i].new_period_cnt_u32 = ((((float)DCO_SAMPLE_FREQ * 1000)) / t_i32);
            if(DCO_dco_rt_as[i].new_period_cnt_u32 == 0)
            {
               DCO_dco_rt_as[i].new_period_cnt_u32 = 1;
            }
         }
         
         // do PWM mod
         t_i32 = DCO_dco_set_as[i].pwm_dc_u16;

         if(DCO_pwmmod_set_as[i].level_u8)
         {
            t_i32 += ((DCO_pwm_mod_output_ai16[i] / 2) * DCO_pwmmod_set_as[i].level_u8) / 128; 
         }   

         if(DCO_dco_set_as[i].pwm_dadsr_mod_type_e != SYNENG_NOMOD)     
         {
            switch(DCO_dco_set_as[i].pwm_dadsr_mod_type_e)
            {
               case SYNENG_NMOD:
                  t_i32 -= (((DADSRH_u16GetOutput(&DADSR_pwmmod_rt_as[i]) * DCO_PWM_RANGE ) / 32768 /* / DADSR_MAX_OUTPUT */ ));
               break;
               case SYNENG_PMOD:
                  t_i32 += (((DADSRH_u16GetOutput(&DADSR_pwmmod_rt_as[i]) * DCO_PWM_RANGE ) / 32768 /* / DADSR_MAX_OUTPUT */ ));
               break;
               case SYNENG_NOMOD:
               break;
            }
         }
         if(t_i32 < (DCO_PWM_RANGE / 95))
         {
            t_i32 = (DCO_PWM_RANGE / 95);
         }
         else
         if(t_i32 > DCO_PWM_RANGE)
         {
            t_i32 = DCO_PWM_RANGE - (DCO_PWM_RANGE / 95);
         }
         DCO_dco_rt_as[i].new_PWM_dc_cnt_u32 = (DCO_dco_rt_as[i].new_period_cnt_u32 * (uint16_t)t_i32) / DCO_PWM_RANGE;
      }
   }   

   return retval;
}


void DCO_vUpdateRuntime(void)
{
   uint8_t i;

   DCO_vSetPWM(&DCO_cutoffmod_dco_rt_s, &DCO_vcf_lfo_s.cutoffmod_dco_set_s, DCO_vcf_lfo_s.cutoffmod_dco_set_s.pwm_dc_u16);
   DCO_vSetFreq(&DCO_cutoffmod_dco_rt_s, &DCO_vcf_lfo_s.cutoffmod_dco_set_s, DCO_vcf_lfo_s.cutoffmod_dco_set_s.freq_mHz_u32, SYNENG_CTRL_FREQ);
   DCO_cutoffmod_dco_rt_s.cnt_u32 = 0;

   DCO_vSetPWM(&DCO_resomod_dco_rt_s, &DCO_vcf_lfo_s.resomod_dco_set_s, DCO_vcf_lfo_s.resomod_dco_set_s.pwm_dc_u16);
   DCO_vSetFreq(&DCO_resomod_dco_rt_s, &DCO_vcf_lfo_s.resomod_dco_set_s, DCO_vcf_lfo_s.resomod_dco_set_s.freq_mHz_u32, SYNENG_CTRL_FREQ);
   DCO_resomod_dco_rt_s.cnt_u32 = 0;

   for(i = 0; i < DCO_MAX_DCOS; i++)
   {
      DCO_vSetPWM(&DCO_dco_rt_as[i], &DCO_dco_set_as[i], DCO_dco_set_as[i].pwm_dc_u16);
      DCO_vSetFreq(&DCO_dco_rt_as[i], &DCO_dco_set_as[i], DCO_dco_set_as[i].freq_mHz_u32, DCO_SAMPLE_FREQ);

      DCO_vSetPWM(&DCO_pwmmod_rt_as[i], &DCO_pwmmod_set_as[i], DCO_pwmmod_set_as[i].pwm_dc_u16);
      DCO_vSetFreq(&DCO_pwmmod_rt_as[i], &DCO_pwmmod_set_as[i], DCO_pwmmod_set_as[i].freq_mHz_u32, DCO_LFO_SAMPLE_FREQ);
      DCO_pwmmod_rt_as[i].period_cnt_u32 = DCO_pwmmod_rt_as[i].new_period_cnt_u32;
      DCO_pwmmod_rt_as[i].cnt_u32 = 0;
      
      DCO_vSetPWM(&DCO_freqmod_rt_as[i], &DCO_freqmod_set_as[i], DCO_freqmod_set_as[i].pwm_dc_u16);
      DCO_vSetFreq(&DCO_freqmod_rt_as[i], &DCO_freqmod_set_as[i], DCO_freqmod_set_as[i].freq_mHz_u32, DCO_LFO_SAMPLE_FREQ);
      DCO_freqmod_rt_as[i].period_cnt_u32 = DCO_freqmod_rt_as[i].new_period_cnt_u32;
      DCO_freqmod_rt_as[i].cnt_u32 = 0;

      DCO_vSetPWM(&DCO_volmod_rt_as[i], &DCO_volmod_set_as[i], DCO_volmod_set_as[i].pwm_dc_u16);
      DCO_vSetFreq(&DCO_volmod_rt_as[i], &DCO_volmod_set_as[i], DCO_volmod_set_as[i].freq_mHz_u32, DCO_SAMPLE_FREQ);
      DCO_volmod_rt_as[i].period_cnt_u32 = DCO_volmod_rt_as[i].new_period_cnt_u32;
      DCO_volmod_rt_as[i].cnt_u32 = 0;
   }   
}   

void DCO_vKeyOn(void)
{
   uint8_t i;
      
   for(i = 0; i < DCO_MAX_DCOS; i++)
   {
      if(DCO_freqmod_set_as[i].keysync_u8)
      {
         DCO_freqmod_rt_as[i].cnt_u32 = 0;
      }
      
      if(DCO_pwmmod_set_as[i].keysync_u8)
      {
         DCO_pwmmod_rt_as[i].cnt_u32 = 0;
      }
   }

   if(DCO_vcf_lfo_s.resomod_dco_set_s.keysync_u8)
   {
      DCO_resomod_dco_rt_s.cnt_u32 = 0;
   }

   if(DCO_vcf_lfo_s.cutoffmod_dco_set_s.keysync_u8)
   {
      DCO_cutoffmod_dco_rt_s.cnt_u32 = 0;
   }
}


