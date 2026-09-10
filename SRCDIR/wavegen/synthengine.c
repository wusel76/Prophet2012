#include "types.h"
#include "wavegenconfig.h"
//#include "seq.h"
#include "synthengine.h"
#include "dco.h"
#include "dadsr.h"
#include "freqlookup.h"

#define RENDER_SAMPLES  (OUTBUF_SIZE / 2) // 500



typedef struct
{
   uint8_t key_u8;
   uint8_t velocity_u8;   
} SYNENG_voice_note_ts;   


SYNENG_voice_note_ts SYNENG_voice_note_s;
SYNENG_dco_set_ts SYNENG_dco_set_as[DCO_MAX_DCOS];

uint32_t SYNENG_buf_au32[OUTBUF_SIZE];
uint8_t SYNENG_name_au8[SYNENG_MAX_NAME_LENGTH + 1];


volatile uint16_t SYNENG_sample_in_idx_u16;
volatile uint16_t SYNENG_sample_out_idx_u16;

volatile uint16_t SYNENG_ctrl_in_idx_u16;
volatile uint16_t SYNENG_ctrl_out_idx_u16;
uint16_t SYNENG_calc_ctrl_cnt_u16;

static uint8_t SYNENG_velocity_u8;
static uint8_t SYNENG_key_u8;

static uint8_t SYNENG_last_key_u8;
static uint8_t SYNENG_portamento_u8;
static uint32_t SYNENG_portamento_start_mHz_u32;
static uint16_t SYNENG_portamento_time_cnt_u16;
static uint32_t SYNENG_portamento_src_mHz_au32[DCO_MAX_DCOS];
static uint32_t SYNENG_portamento_dest_mHz_au32[DCO_MAX_DCOS];
uint16_t SYNENG_portamento_time_u16;

   
SYNENG_vca_set_ts SYNENG_vca_set_s;
SYNENG_vcf_set_ts SYNENG_vcf_set_s;

uint8_t SYNENG_calc_audio_u8;

#ifdef MICROE_MMB32 
static uint16_t SYNENG_vca_out_u16;
#endif

SYNENG_ctrldata_ts SYNENG_ctrldata_as[(OUTBUF_SIZE / SYNENG_CTRL_INTERVAL_DIV) + 1];

#if defined(_WIN32) || defined(__WXMAC__)
   static uint16_t SYNENG_u16GetSamplesInFIFO(void);
#else
   static inline uint16_t SYNENG_u16GetSamplesInFIFO(void);
#endif

void SYNENG_vInit(void)
{
   uint16_t i;
   SYNENG_last_key_u8 = 0;
   SYNENG_name_au8[0] = 0;
   SYNENG_sample_in_idx_u16 = 0;
   SYNENG_sample_out_idx_u16 = 0;
   SYNENG_ctrl_in_idx_u16 = 0;
   SYNENG_ctrl_out_idx_u16 = 0;
   SYNENG_calc_ctrl_cnt_u16 = 0;
   SYNENG_velocity_u8 = 0;
   SYNENG_key_u8 = 0;
   SYNENG_calc_audio_u8 = 1;
   SYNENG_portamento_u8 = 0;
   SYNENG_portamento_start_mHz_u32 = 0;
   SYNENG_portamento_time_cnt_u16 = 0;
   SYNENG_portamento_time_u16 = 0;

#ifdef MICROE_MMB32 
   SYNENG_vca_out_u16 = 0;
#endif

   for(i = 0; i < RENDER_SAMPLES; i++)
   {
      SYNENG_buf_au32[i] = 0;
   }

   for(i = 0; i < SYNENG_CTRL_ELEMENTS; i++)
   {
      SYNENG_ctrldata_as[i].cutoff_u32 = 0;
      SYNENG_ctrldata_as[i].resonance_u32 = 0;
      SYNENG_ctrldata_as[i].vca_u32 = 0;
      SYNENG_ctrldata_as[i].out_idx_u16 = 0xffff;
   }
   
   for(i = 0; i < DCO_MAX_DCOS; i++)
   {
      SYNENG_portamento_dest_mHz_au32[i] = 0;
      SYNENG_portamento_src_mHz_au32[i] = 0;
      SYNENG_dco_set_as[i].tune_i8 = 0;
      SYNENG_dco_set_as[i].transpose_i8 = 0;
      SYNENG_dco_set_as[i].reserved1 = 0;
      SYNENG_dco_set_as[i].reserved2 = 0;
      SYNENG_dco_set_as[i].reserved3 = 0;
      SYNENG_dco_set_as[i].reserved4 = 0;
      SYNENG_dco_set_as[i].reserved5 = 0;
      SYNENG_dco_set_as[i].reserved6 = 0;
      SYNENG_dco_set_as[i].reserved7 = 0;
      SYNENG_dco_set_as[i].reserved8 = 0;
      SYNENG_dco_set_as[i].reserved9 = 0;
      SYNENG_dco_set_as[i].reserved10 = 0;
      SYNENG_dco_set_as[i].reserved11 = 0;
      SYNENG_dco_set_as[i].reserved12 = 0;
   }   
   SYNENG_voice_note_s.key_u8 = 0;
   SYNENG_voice_note_s.velocity_u8 = 0;
   DADSRH_vInit();
   DCO_vInit();

   SYNENG_vca_set_s.level_u8 = 127;
   SYNENG_vca_set_s.velocity_mod_i8 = 0;
   SYNENG_vca_set_s.reserved1 = 0;
   SYNENG_vca_set_s.reserved2 = 0;
   SYNENG_vca_set_s.reserved3 = 0;
   SYNENG_vca_set_s.reserved4 = 0;
   SYNENG_vca_set_s.reserved5 = 0;
   SYNENG_vca_set_s.reserved6 = 0;
   SYNENG_vca_set_s.reserved7 = 0;
   SYNENG_vca_set_s.reserved8 = 0;
   SYNENG_vca_set_s.reserved9 = 0;
   SYNENG_vca_set_s.reserved10 = 0;
   SYNENG_vca_set_s.reserved11 = 0;
   SYNENG_vca_set_s.reserved12 = 0;
   
   SYNENG_vcf_set_s.cutoff_u8 = 127;
   SYNENG_vcf_set_s.dadsr_cutoff_mod_type_e = SYNENG_PMOD;
   SYNENG_vcf_set_s.velocity_cutoff_mod_i8 = 0;
   SYNENG_vcf_set_s.modwheel_cutoff_mod_i8 = 0;
   SYNENG_vcf_set_s.kt_cutoff_mod_i8 = 0;
   SYNENG_vcf_set_s.reserved1 = 0;
   SYNENG_vcf_set_s.reserved2 = 0;
   SYNENG_vcf_set_s.reserved3 = 0;
   SYNENG_vcf_set_s.reserved4 = 0;
   SYNENG_vcf_set_s.reserved5 = 0;
   SYNENG_vcf_set_s.reserved6 = 0;
   SYNENG_vcf_set_s.reserved7 = 0;
   SYNENG_vcf_set_s.reserved8 = 0;
   SYNENG_vcf_set_s.reserved9 = 0;
   SYNENG_vcf_set_s.reserved10 = 0;
   SYNENG_vcf_set_s.reserved11 = 0;
   SYNENG_vcf_set_s.reserved12 = 0;

   SYNENG_vcf_set_s.resonance_u8 = 0;
   SYNENG_vcf_set_s.dadsr_res_mod_type_e = SYNENG_PMOD;
   SYNENG_vcf_set_s.velocity_res_mod_i8 = 0;
   SYNENG_vcf_set_s.modwheel_res_mod_i8 = 0;
   SYNENG_vcf_set_s.kt_res_mod_i8 = 0;
}


#if defined(_WIN32) || defined(__WXMAC__)
static uint16_t SYNENG_u16GetSamplesInFIFO(void)
#else
static inline uint16_t SYNENG_u16GetSamplesInFIFO(void)
#endif
{
   uint16_t retval = 0;
   
   retval = ((SYNENG_sample_in_idx_u16 - SYNENG_sample_out_idx_u16) + OUTBUF_SIZE) % OUTBUF_SIZE;
   
   return retval;   
}


void SYNENG_vHandler(void)    // will be called by timer2 ISR every 1ms
{
   uint16_t i;
   
   SYNENG_portamento_time_cnt_u16++;
   
   //if(SYNENG_u16GetSamplesInFIFO() <= (RENDER_SAMPLES - 10))
   {      
      //TRISBbits.TRISB0 = 0;
      //LATBbits.LATB0 = 1;
      
      if(SYNENG_portamento_u8 && SYNENG_portamento_time_u16)
      {
         if(SYNENG_portamento_time_cnt_u16 <= SYNENG_portamento_time_u16)
         {
            for(i = 0; i < DCO_MAX_DCOS; i++)
            {
               float f;
               float delta;
               delta = (float)SYNENG_portamento_dest_mHz_au32[i] - (float)SYNENG_portamento_src_mHz_au32[i];
               f = (float)SYNENG_portamento_dest_mHz_au32[i] -  ((delta * (400 - SYNENG_portamento_time_cnt_u16)) / SYNENG_portamento_time_u16);
               DCO_vSetFreq(&DCO_dco_rt_as[i], &DCO_dco_set_as[i], (uint32_t)f, DCO_SAMPLE_FREQ);
            }
         }
         else
         {
            SYNENG_portamento_u8 = 0;
         }               
      }   

      i = 0;
      if(SYNENG_calc_audio_u8)
      {
         //while((i < RENDER_SAMPLES))
         {
            if(SYNENG_ctrldata_as[SYNENG_ctrl_in_idx_u16].vca_u32)
            {
#ifdef MICROE_MMB32   
#if 1 // 1 == static  0==use vca
               SYNENG_buf_au32[SYNENG_sample_in_idx_u16] = (DCO_i32GetDCOsOutput() / DCO_MAX_DCOS) ;
#else    
               SYNENG_buf_au32[SYNENG_sample_in_idx_u16] = (((DCO_i32GetDCOsOutput() / DCO_MAX_DCOS) * SYNENG_vca_out_u16) / 65535);
#endif
#else
               
#ifdef _WIN321 // !!!! TODO !!!! DADSR problem
               SYNENG_buf_au32[SYNENG_sample_in_idx_u16] = ((int64_t)DCO_i32GetDCOsOutput() * SYNENG_ctrldata_as[SYNENG_ctrl_in_idx_u16].vca_u32) / (65535 << 8);
               SYNENG_buf_au32[SYNENG_sample_in_idx_u16] = SYNENG_buf_au32[SYNENG_sample_in_idx_u16] + (32768 * DCO_MAX_DCOS);
#else
               SYNENG_buf_au32[SYNENG_sample_in_idx_u16] = (uint32_t)(DCO_i32GetDCOsOutput() + (32768 * DCO_MAX_DCOS));
#endif

#endif
            }
            else
            {
               SYNENG_buf_au32[SYNENG_sample_in_idx_u16] = (32768 * DCO_MAX_DCOS);
            }
             
   
            //SYNENG_calc_ctrl_cnt_u16++;
            //if(SYNENG_calc_ctrl_cnt_u16 >= SYNENG_CTRL_INTERVAL_DIV)
            {
               float t;
//               uint8_t vi;
               SYNENG_calc_ctrl_cnt_u16 = 0;
            
               DADSRH_vHandleCtrlEnvs();
               
               SYNENG_ctrldata_as[SYNENG_ctrl_in_idx_u16].out_idx_u16 = SYNENG_sample_in_idx_u16;
              
               // ---------------------------------------- VCA ----------------------------------------
#if 1 // 0=apply offset
               t = (float)(DADSRH_u16GetOutput(&DADSR_vcamod_rt_s) * 2);
               t = (( (float)(((float)t *  SYNENG_velocity_u8) / 128) * (SYNENG_vca_set_s.velocity_mod_i8 + SYNENG_MAX_VELO_VCA)) / 128);
#else
               t = (( (int32_t)(((uint32_t)65535 *  SYNENG_velocity_u8) / 127) * SYNENG_vca_set_s.velocity_mod_i8) / SYNENG_MAX_VELO_VCA);
               t += (int32_t)(DADSRH_u16GetOutput(&DADSR_vcamod_rt_s) << 1);
#endif
               
               if(t < 0)
               {
                  t = 0;
               }
               else
               if(t > 65535)
               {
                  t = 65535;
               }
   

#ifdef MICROE_MMB32x // !!!!!!!!!!!!!!!!!!!!!
               SYNENG_vca_out_u16 = (uint16_t)(((uint32_t)t * SYNENG_vca_set_s.level_u8) / 127); 
#else
               SYNENG_ctrldata_as[SYNENG_ctrl_in_idx_u16].vca_u32 = ((uint32_t)t * 256);            
               
               //SYNENG_ctrldata_as[SYNENG_ctrl_in_idx_u16].vca_u32 = (SYNENG_ctrldata_as[SYNENG_ctrl_in_idx_u16].vca_u32 * SYNENG_vca_set_s.level_u8) / 127;
               SYNENG_ctrldata_as[SYNENG_ctrl_in_idx_u16].vca_u32 = (SYNENG_ctrldata_as[SYNENG_ctrl_in_idx_u16].vca_u32 * SYNENG_vca_set_s.level_u8) / 128;

#if defined(_WIN32) || defined(__WXMAC__) // !!! TO DO !!! DADSR problem
               SYNENG_ctrldata_as[SYNENG_ctrl_in_idx_u16].vca_u32 = 1;
#endif

#endif
               
               SYNENG_ctrl_in_idx_u16++;
               if(SYNENG_ctrl_in_idx_u16 >= SYNENG_CTRL_ELEMENTS)
               {
                  SYNENG_ctrl_in_idx_u16 = 0;
               }                     
            }   
   
            SYNENG_sample_in_idx_u16++;
            if(SYNENG_sample_in_idx_u16 >= OUTBUF_SIZE)
            {
               SYNENG_sample_in_idx_u16 = 0;  
            }   
            i++;
         }     
      }
      //LATBbits.LATB0 = 0;
   }     
         
   DADSRH_vHandler1ms();
}


void SYNENG_vKeyOn(uint8_t key_u8, uint8_t velocity_u8)
{
   uint8_t i;
   
   SYNENG_key_u8 = key_u8;
   
   if(SYNENG_last_key_u8)
   {
      // portamento
      SYNENG_portamento_u8 = 1;
      SYNENG_portamento_time_cnt_u16 = 0;

      for(i = 0; i < DCO_MAX_DCOS; i++)
      {
         uint32_t f;
         f = FREQ_u32GetFreq(SYNENG_last_key_u8);

         if(SYNENG_dco_set_as[i].tune_i8 != 0)
         {
            if(SYNENG_dco_set_as[i].tune_i8 > 0)
            {
               int32_t freq_dif = FREQ_u32GetFreq(SYNENG_last_key_u8 + 1) - f;
               freq_dif = (freq_dif * SYNENG_dco_set_as[i].tune_i8) / 100;
               f = (uint32_t)(f + freq_dif); 
            }
            else
            {
               int32_t freq_dif = FREQ_u32GetFreq(SYNENG_last_key_u8 - 1) - f;
               freq_dif = (freq_dif * SYNENG_dco_set_as[i].tune_i8) / 100;
               f = (uint32_t)(f - freq_dif); 
            }
         }  

         SYNENG_portamento_src_mHz_au32[i] = f;
      }   
   }
   else
   {
      //SYNENG_portamento_u8 = 0;
   }   
   SYNENG_last_key_u8 = key_u8;
   
   for(i = 0; i < DCO_MAX_DCOS; i++)
   {
      uint32_t f;
      int8_t key;
      key = key_u8;
      key += SYNENG_dco_set_as[i].transpose_i8;
      if(key < 0)
      {
         key = 0;
      }   
      f = FREQ_u32GetFreq(key);

      if(SYNENG_dco_set_as[i].tune_i8 != 0)
      {
         if(SYNENG_dco_set_as[i].tune_i8 > 0)
         {
            int32_t freq_dif = FREQ_u32GetFreq(key + 1) - f;
            freq_dif = (freq_dif * SYNENG_dco_set_as[i].tune_i8) / 100;
            f = (uint32_t)(f + freq_dif); 
         }
         else
         {
            int32_t freq_dif = FREQ_u32GetFreq(key - 1) - f;
            freq_dif = (freq_dif * SYNENG_dco_set_as[i].tune_i8) / 100;
            f = (uint32_t)(f - freq_dif); 
         }
      }   
      SYNENG_portamento_dest_mHz_au32[i] = f;
      DCO_vSetFreq(&DCO_dco_rt_as[i], &DCO_dco_set_as[i], f, DCO_SAMPLE_FREQ);
   }   
   
   DADSRH_vKeyOn();
   DCO_vKeyOn();
   
   SYNENG_velocity_u8 = velocity_u8;
#if 0
   SYNENG_voice_note_as[voice_u8].key_u8 = key_u8;
   SYNENG_voice_note_as[voice_u8].velocity_u8 = velocity_u8;
#endif
}

   
void SYNENG_vKeyOff(uint8_t key_u8)
{
   SYNENG_last_key_u8 = 0;
   DADSRH_vKeyOff();
}

