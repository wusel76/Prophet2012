#include "types.h"
#include "wavegenconfig.h"
#include "synthengine.h"
#include "dco.h"
#include "dadsr.h"


static void DADSRH_vHandlerDADSR(DADSR_dadsr_data_ts *dadsr_ps, DADSR_runtime_data_ts *dadsr_rt_ps);
static void DADSRH_vKeyOnEvent(DADSR_dadsr_data_ts *dadsr_ps, DADSR_runtime_data_ts *dadsr_rt_ps);
static void DADSRH_vKeyOffEvent(DADSR_runtime_data_ts *dadsr_rt_ps);


DADSR_dadsr_data_ts DADSR_pwmmod_data_as[DCO_MAX_DCOS];
DADSR_runtime_data_ts DADSR_pwmmod_rt_as[DCO_MAX_DCOS];

DADSR_dadsr_data_ts DADSR_freqmod_data_as[DCO_MAX_DCOS];
DADSR_runtime_data_ts DADSR_freqmod_rt_as[DCO_MAX_DCOS];

DADSR_dadsr_data_ts DADSR_dcoout_data_as[DCO_MAX_DCOS];
DADSR_runtime_data_ts DADSR_dcoout_rt_as[DCO_MAX_DCOS];

DADSR_dadsr_data_ts DADSR_filtmod_data_s;
DADSR_runtime_data_ts DADSR_filtmod_rt_s;

DADSR_dadsr_data_ts DADSR_resmod_data_s;
DADSR_runtime_data_ts DADSR_resmod_rt_s;

DADSR_dadsr_data_ts DADSR_vcamod_data_s;
DADSR_runtime_data_ts DADSR_vcamod_rt_s;


void DADSRH_vInit(void)
{
   uint8_t i;
   for(i = 0; i < DCO_MAX_DCOS; i++)
   {
      DADSR_dcoout_data_as[i].delay_time_u16 = 0;
      DADSR_dcoout_data_as[i].attack_time_u16 = 0;
      DADSR_dcoout_data_as[i].decay_time_u16 = 0;
      DADSR_dcoout_data_as[i].sustain_level_u16 = DADSR_MAX_OUTPUT;
      DADSR_dcoout_data_as[i].release_time_u16 = DADSR_DCO_OUTPUT_MAXTIME;
      DADSR_dcoout_data_as[i].output_level_u8 = DADSR_MAX_OUTPUT_LEVEL;
      DADSR_dcoout_data_as[i].reserved1 = 0;
      DADSR_dcoout_data_as[i].reserved2 = 0;
      DADSR_dcoout_data_as[i].reserved3 = 0;
      DADSR_dcoout_data_as[i].reserved4 = 0;
      DADSR_dcoout_data_as[i].reserved5 = 0;
      DADSR_dcoout_data_as[i].reserved6 = 0;
      DADSR_dcoout_data_as[i].reserved7 = 0;
      DADSR_dcoout_data_as[i].reserved8 = 0;
      DADSR_dcoout_data_as[i].reserved9 = 0;
      DADSR_dcoout_data_as[i].reserved10 = 0;
      DADSR_dcoout_data_as[i].reserved11 = 0;
      DADSR_dcoout_data_as[i].reserved12 = 0;

      DADSR_dcoout_rt_as[i].DADSR_internal_output_u16 = 0;
      DADSR_dcoout_rt_as[i].state_e = DADSR_DELAY;
      DADSR_dcoout_rt_as[i].DADSR_time_1ms_u16 = 0;
      DADSR_dcoout_rt_as[i].DADSR_output_u16 = 0;

      // PWM mod DADSRs
      DADSR_pwmmod_data_as[i].delay_time_u16 = 0;
      DADSR_pwmmod_data_as[i].attack_time_u16 = 0;
      DADSR_pwmmod_data_as[i].decay_time_u16 = 0;
      DADSR_pwmmod_data_as[i].sustain_level_u16 = DADSR_MAX_OUTPUT;
      DADSR_pwmmod_data_as[i].release_time_u16 = 0;
      DADSR_pwmmod_data_as[i].output_level_u8 = 0;
      DADSR_pwmmod_data_as[i].reserved1 = 0;
      DADSR_pwmmod_data_as[i].reserved2 = 0;
      DADSR_pwmmod_data_as[i].reserved3 = 0;
      DADSR_pwmmod_data_as[i].reserved4 = 0;
      DADSR_pwmmod_data_as[i].reserved5 = 0;
      DADSR_pwmmod_data_as[i].reserved6 = 0;
      DADSR_pwmmod_data_as[i].reserved7 = 0;
      DADSR_pwmmod_data_as[i].reserved8 = 0;
      DADSR_pwmmod_data_as[i].reserved9 = 0;
      DADSR_pwmmod_data_as[i].reserved10 = 0;
      DADSR_pwmmod_data_as[i].reserved11 = 0;
      DADSR_pwmmod_data_as[i].reserved12 = 0;

      DADSR_pwmmod_rt_as[i].DADSR_internal_output_u16 = 0;
      DADSR_pwmmod_rt_as[i].state_e = DADSR_DELAY;
      DADSR_pwmmod_rt_as[i].DADSR_time_1ms_u16 = 0;
      DADSR_pwmmod_rt_as[i].DADSR_output_u16 = 0;

      // Freq mod DADSRs
      DADSR_freqmod_data_as[i].delay_time_u16 = 0;
      DADSR_freqmod_data_as[i].attack_time_u16 = 0;
      DADSR_freqmod_data_as[i].decay_time_u16 = 0;
      DADSR_freqmod_data_as[i].sustain_level_u16 = DADSR_MAX_OUTPUT;
      DADSR_freqmod_data_as[i].release_time_u16 = 0;
      DADSR_freqmod_data_as[i].output_level_u8 = 0;
      DADSR_freqmod_data_as[i].reserved1 = 0;
      DADSR_freqmod_data_as[i].reserved2 = 0;
      DADSR_freqmod_data_as[i].reserved3 = 0;
      DADSR_freqmod_data_as[i].reserved4 = 0;
      DADSR_freqmod_data_as[i].reserved5 = 0;
      DADSR_freqmod_data_as[i].reserved6 = 0;
      DADSR_freqmod_data_as[i].reserved7 = 0;
      DADSR_freqmod_data_as[i].reserved8 = 0;
      DADSR_freqmod_data_as[i].reserved9 = 0;
      DADSR_freqmod_data_as[i].reserved10 = 0;
      DADSR_freqmod_data_as[i].reserved11 = 0;
      DADSR_freqmod_data_as[i].reserved12 = 0;

      DADSR_freqmod_rt_as[i].DADSR_internal_output_u16 = 0;
      DADSR_freqmod_rt_as[i].state_e = DADSR_DELAY;
      DADSR_freqmod_rt_as[i].DADSR_time_1ms_u16 = 0;
      DADSR_freqmod_rt_as[i].DADSR_output_u16 = 0;
   }
   
   DADSR_filtmod_data_s.delay_time_u16 = 0;
   DADSR_filtmod_data_s.attack_time_u16 = 0;
   DADSR_filtmod_data_s.decay_time_u16 = 0;
   DADSR_filtmod_data_s.sustain_level_u16 = DADSR_MAX_OUTPUT;
   DADSR_filtmod_data_s.release_time_u16 = 0;
   DADSR_filtmod_data_s.output_level_u8 = 0;
   DADSR_filtmod_data_s.reserved1 = 0;
   DADSR_filtmod_data_s.reserved2 = 0;
   DADSR_filtmod_data_s.reserved3 = 0;
   DADSR_filtmod_data_s.reserved4 = 0;
   DADSR_filtmod_data_s.reserved5 = 0;
   DADSR_filtmod_data_s.reserved6 = 0;
   DADSR_filtmod_data_s.reserved7 = 0;
   DADSR_filtmod_data_s.reserved8 = 0;
   DADSR_filtmod_data_s.reserved9 = 0;
   DADSR_filtmod_data_s.reserved10 = 0;
   DADSR_filtmod_data_s.reserved11 = 0;
   DADSR_filtmod_data_s.reserved12 = 0;

   DADSR_filtmod_rt_s.DADSR_internal_output_u16 = 0;
   DADSR_filtmod_rt_s.state_e = DADSR_DELAY;
   DADSR_filtmod_rt_s.DADSR_time_1ms_u16 = 0;
   DADSR_filtmod_rt_s.DADSR_output_u16 = 0;

   DADSR_resmod_data_s.delay_time_u16 = 0;
   DADSR_resmod_data_s.attack_time_u16 = 0;
   DADSR_resmod_data_s.decay_time_u16 = 0;
   DADSR_resmod_data_s.sustain_level_u16 = DADSR_MAX_OUTPUT;
   DADSR_resmod_data_s.release_time_u16 = 0;
   DADSR_resmod_data_s.output_level_u8 = 0;
   DADSR_resmod_data_s.reserved1 = 0;
   DADSR_resmod_data_s.reserved2 = 0;
   DADSR_resmod_data_s.reserved3 = 0;
   DADSR_resmod_data_s.reserved4 = 0;
   DADSR_resmod_data_s.reserved5 = 0;
   DADSR_resmod_data_s.reserved6 = 0;
   DADSR_resmod_data_s.reserved7 = 0;
   DADSR_resmod_data_s.reserved8 = 0;
   DADSR_resmod_data_s.reserved9 = 0;
   DADSR_resmod_data_s.reserved10 = 0;
   DADSR_resmod_data_s.reserved11 = 0;
   DADSR_resmod_data_s.reserved12 = 0;

   DADSR_resmod_rt_s.DADSR_internal_output_u16 = 0;
   DADSR_resmod_rt_s.state_e = DADSR_DELAY;
   DADSR_resmod_rt_s.DADSR_time_1ms_u16 = 0;
   DADSR_resmod_rt_s.DADSR_output_u16 = 0;

   DADSR_vcamod_data_s.delay_time_u16 = 0;
   DADSR_vcamod_data_s.attack_time_u16 = 0;
   DADSR_vcamod_data_s.decay_time_u16 = 0;
   DADSR_vcamod_data_s.sustain_level_u16 = DADSR_MAX_OUTPUT;
   DADSR_vcamod_data_s.release_time_u16 = 0;
   DADSR_vcamod_data_s.output_level_u8 = DADSR_MAX_OUTPUT_LEVEL;
   DADSR_vcamod_data_s.reserved1 = 0;
   DADSR_vcamod_data_s.reserved2 = 0;
   DADSR_vcamod_data_s.reserved3 = 0;
   DADSR_vcamod_data_s.reserved4 = 0;
   DADSR_vcamod_data_s.reserved5 = 0;
   DADSR_vcamod_data_s.reserved6 = 0;
   DADSR_vcamod_data_s.reserved7 = 0;
   DADSR_vcamod_data_s.reserved8 = 0;
   DADSR_vcamod_data_s.reserved9 = 0;
   DADSR_vcamod_data_s.reserved10 = 0;
   DADSR_vcamod_data_s.reserved11 = 0;
   DADSR_vcamod_data_s.reserved12 = 0;

   DADSR_vcamod_rt_s.DADSR_internal_output_u16 = 0;
   DADSR_vcamod_rt_s.state_e = DADSR_DELAY;
   DADSR_vcamod_rt_s.DADSR_time_1ms_u16 = 0;
   DADSR_vcamod_rt_s.DADSR_output_u16 = 0;

#if defined(_WIN32) || defined(__WXMAC__)
   DADSR_vcamod_data_s.attack_time_u16 = 30;
   DADSR_vcamod_data_s.release_time_u16 = 30; 
#endif
}


void DADSRH_vProgramInit(void)
{
}


static void DADSRH_vHandlerDADSR(DADSR_dadsr_data_ts *dadsr_ps, DADSR_runtime_data_ts *dadsr_rt_ps)
{  
   float t_u16;
   if(dadsr_ps->output_level_u8)
   {
      dadsr_rt_ps->DADSR_time_1ms_u16++;
   
      switch(dadsr_rt_ps->state_e)
      {
         case DADSR_DELAY:
            if(dadsr_rt_ps->DADSR_time_1ms_u16 >= dadsr_rt_ps->DADSR_dest_delay_time_u16)
            {
               dadsr_rt_ps->state_e = DADSR_ATTACK;
               dadsr_rt_ps->DADSR_time_1ms_u16 = 0;
            }
         break;
         case DADSR_ATTACK:
            t_u16 = dadsr_rt_ps->DADSR_dest_attack_time_u16;
            if(t_u16)
            {
#ifdef DADSR_LINEAR
               dadsr_rt_ps->DADSR_internal_output_u16 = ((float)(DADSR_MAX_OUTPUT * dadsr_rt_ps->DADSR_time_1ms_u16) / t_u16);
#else
               dadsr_rt_ps->DADSR_internal_output_u16 = (((float)(DADSR_MAX_OUTPUT * dadsr_rt_ps->DADSR_time_1ms_u16) * dadsr_rt_ps->DADSR_time_1ms_u16) / (float)(t_u16 * t_u16));
#endif
            }
            if(dadsr_rt_ps->DADSR_time_1ms_u16 >= dadsr_rt_ps->DADSR_dest_attack_time_u16)
            {
               dadsr_rt_ps->state_e = DADSR_DECAY;
               dadsr_rt_ps->DADSR_internal_output_u16 = DADSR_MAX_OUTPUT;
               dadsr_rt_ps->DADSR_time_1ms_u16 = 0;
            }
         break;
         case DADSR_DECAY:
            t_u16 = dadsr_rt_ps->DADSR_dest_decay_time_u16;
            if(t_u16)
            {
#ifdef DADSR_LINEAR
               //dadsr_rt_ps->DADSR_internal_output_u16 -= (DADSR_MAX_OUTPUT - dadsr_ps->sustain_level_u16) / t_u16;
               dadsr_rt_ps->DADSR_internal_output_u16 = (uint16_t)(((uint32_t)(DADSR_MAX_OUTPUT - dadsr_ps->sustain_level_u16) * (t_u16 - dadsr_rt_ps->DADSR_time_1ms_u16)) / t_u16) + dadsr_ps->sustain_level_u16;
#else
               float time_tmp_u16;
               time_tmp_u16 = t_u16 - dadsr_rt_ps->DADSR_time_1ms_u16;
               dadsr_rt_ps->DADSR_internal_output_u16 = (((((float)DADSR_MAX_OUTPUT - (float)dadsr_ps->sustain_level_u16) * time_tmp_u16) * time_tmp_u16) / (float)(t_u16 * t_u16)) + dadsr_ps->sustain_level_u16;
#endif
            }
            if(dadsr_rt_ps->DADSR_internal_output_u16 > DADSR_MAX_OUTPUT)  /* undeflowprotection */
            {
               dadsr_rt_ps->DADSR_internal_output_u16 = 0;
            }
            if(dadsr_rt_ps->DADSR_time_1ms_u16 >= dadsr_rt_ps->DADSR_dest_decay_time_u16)
            {
               dadsr_rt_ps->state_e = DADSR_SUSTAIN;
               dadsr_rt_ps->DADSR_time_1ms_u16 = 0;
            }
         break;
         case DADSR_SUSTAIN:
            dadsr_rt_ps->DADSR_internal_output_u16 = dadsr_ps->sustain_level_u16;
         break;
         case DADSR_RELEASE:
            if(dadsr_rt_ps->DADSR_dest_release_time_u16 != 0)
            {
               t_u16 = dadsr_rt_ps->DADSR_dest_release_time_u16;
               if(t_u16)
               {
#ifdef DADSR_LINEAR
			         dadsr_rt_ps->DADSR_internal_output_u16 = (((uint32_t)dadsr_rt_ps->DADSR_release_start_level_u16 * (dadsr_rt_ps->DADSR_dest_release_time_u16 - dadsr_rt_ps->DADSR_time_1ms_u16)) / t_u16);
#else
                  uint64_t time_tmp_u16;
                  time_tmp_u16 = dadsr_rt_ps->DADSR_dest_release_time_u16 - dadsr_rt_ps->DADSR_time_1ms_u16;
                  dadsr_rt_ps->DADSR_internal_output_u16 = ((((float)dadsr_rt_ps->DADSR_release_start_level_u16 * time_tmp_u16) * time_tmp_u16) / (float)(t_u16 * t_u16));
#endif
               }

               if(dadsr_rt_ps->DADSR_time_1ms_u16 > dadsr_rt_ps->DADSR_dest_release_time_u16)  /* undeflowprotection */
               {
                  dadsr_rt_ps->DADSR_internal_output_u16 = 0;
                  dadsr_rt_ps->state_e = DADSR_STOP;
               }
            }
            else
            {
			      dadsr_rt_ps->state_e = DADSR_STOP;
               dadsr_rt_ps->DADSR_internal_output_u16 = 0;
            }   
         break;
         case DADSR_STOP:
            dadsr_rt_ps->DADSR_internal_output_u16 = 0;
         break;
      }  
      
      dadsr_rt_ps->DADSR_output_u16 = (((float)dadsr_rt_ps->DADSR_internal_output_u16 * dadsr_ps->output_level_u8) / DADSR_MAX_OUTPUT_LEVEL);
   }
   else
   {
      dadsr_rt_ps->DADSR_output_u16 = 0;
   }   
}   

void DADSRH_vHandler1ms(void)
{
   uint8_t i;

   for(i = 0; i < DCO_MAX_DCOS; i++)
   {
      DADSRH_vHandlerDADSR(&DADSR_pwmmod_data_as[i], &DADSR_pwmmod_rt_as[i]);
      DADSRH_vHandlerDADSR(&DADSR_freqmod_data_as[i], &DADSR_freqmod_rt_as[i]);
   }
}

void DADSRH_vHandleCtrlEnvs(void)
{
   uint8_t i;

   DADSRH_vHandlerDADSR(&DADSR_filtmod_data_s, &DADSR_filtmod_rt_s);
   DADSRH_vHandlerDADSR(&DADSR_resmod_data_s, &DADSR_resmod_rt_s);
   DADSRH_vHandlerDADSR(&DADSR_vcamod_data_s, &DADSR_vcamod_rt_s);
   for(i = 0; i < DCO_MAX_DCOS; i++)
   {
      DADSRH_vHandlerDADSR(&DADSR_dcoout_data_as[i], &DADSR_dcoout_rt_as[i]);
   }
}   

static void DADSRH_vKeyOnEvent(DADSR_dadsr_data_ts *dadsr_ps, DADSR_runtime_data_ts *dadsr_rt_ps)
{
   dadsr_rt_ps->DADSR_time_1ms_u16 = 0;
   if(dadsr_ps->delay_time_u16)
   {
      dadsr_rt_ps->state_e = DADSR_DELAY;
      dadsr_rt_ps->DADSR_internal_output_u16 = 0;
   }
   else
   {
      if(dadsr_ps->attack_time_u16)
      {
         dadsr_rt_ps->state_e = DADSR_ATTACK;
         dadsr_rt_ps->DADSR_internal_output_u16 = 0;
      }
      else
      {
         dadsr_rt_ps->state_e = DADSR_DECAY;
         dadsr_rt_ps->DADSR_internal_output_u16 = DADSR_MAX_OUTPUT;
      }
   }
   dadsr_rt_ps->DADSR_dest_release_time_u16 = (dadsr_ps->release_time_u16 * dadsr_ps->release_time_u16) * 16;// >> 1;
   dadsr_rt_ps->DADSR_dest_attack_time_u16 = (dadsr_ps->attack_time_u16 * dadsr_ps->attack_time_u16) * 16; // >> 2;
   dadsr_rt_ps->DADSR_dest_decay_time_u16 = (dadsr_ps->decay_time_u16 * dadsr_ps->decay_time_u16) * 16; // >> 2;
   dadsr_rt_ps->DADSR_dest_delay_time_u16 = (dadsr_ps->delay_time_u16 * dadsr_ps->delay_time_u16) * 16; // >> 2;
}

void DADSRH_vKeyOn(void)
{
   uint8_t i;
   for(i = 0; i < DCO_MAX_DCOS; i++)
   {
      DADSRH_vKeyOnEvent(&DADSR_pwmmod_data_as[i], &DADSR_pwmmod_rt_as[i]);
      DADSRH_vKeyOnEvent(&DADSR_freqmod_data_as[i], &DADSR_freqmod_rt_as[i]);
      DADSRH_vKeyOnEvent(&DADSR_dcoout_data_as[i], &DADSR_dcoout_rt_as[i]);
   }
   DADSRH_vKeyOnEvent(&DADSR_filtmod_data_s, &DADSR_filtmod_rt_s);
   DADSRH_vKeyOnEvent(&DADSR_resmod_data_s, &DADSR_resmod_rt_s);
   DADSRH_vKeyOnEvent(&DADSR_vcamod_data_s, &DADSR_vcamod_rt_s);
}

#if defined(_WIN32) || defined(__WXMAC__)
static void DADSRH_vKeyOffEvent(DADSR_runtime_data_ts *dadsr_rt_ps)
#else
static inline void DADSRH_vKeyOffEvent(DADSR_runtime_data_ts *dadsr_rt_ps)
#endif
{
   dadsr_rt_ps->DADSR_time_1ms_u16 = 0;
   dadsr_rt_ps->state_e = DADSR_RELEASE;
   dadsr_rt_ps->DADSR_release_start_level_u16 = dadsr_rt_ps->DADSR_internal_output_u16;
}

void DADSRH_vKeyOff(void)
{
   uint8_t i;
   for(i = 0; i < DCO_MAX_DCOS; i++)
   {
      DADSRH_vKeyOffEvent(&DADSR_pwmmod_rt_as[i]);
      DADSRH_vKeyOffEvent(&DADSR_freqmod_rt_as[i]);
      DADSRH_vKeyOffEvent(&DADSR_dcoout_rt_as[i]);
   }
   DADSRH_vKeyOffEvent(&DADSR_filtmod_rt_s);
   DADSRH_vKeyOffEvent(&DADSR_resmod_rt_s);
   DADSRH_vKeyOffEvent(&DADSR_vcamod_rt_s);
}

#if defined(_WIN32) || defined(__WXMAC__)
float DADSRH_u16GetOutput(DADSR_runtime_data_ts *dadsr_rt_ps)
#else
uint16_t DADSRH_u16GetOutput(DADSR_runtime_data_ts *dadsr_rt_ps)
#endif
{
   return dadsr_rt_ps->DADSR_output_u16;
}
