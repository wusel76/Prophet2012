#ifndef __DCOH__
#define __DCOH__

#define DCO_LFO_CALC_INTERVAL_DIV 10 // !!!!1000

#define DCO_SAMPLE_FREQ SAMPLEFREQ
#define DCO_LFO_SAMPLE_FREQ (DCO_SAMPLE_FREQ / DCO_LFO_CALC_INTERVAL_DIV)
#define DCO_PWM_RANGE   32767
#define DCO_MAX_LEVEL   127
#define DCO_OUTPUT_RANGE   65534
#define DCO_FM_MAX_ALGORITHM 13
#define DCO_FM_MAX_FEEDBACK   127
#define DCO_MAX_MAIN_TUNE 127
#define DCO_MAX_FREQ_HZ 20000
#define DCO_MAX_SUB_LEVEL 127
#define DCO_SCALE_MAX 10


typedef enum { DCO_SAW, DCO_TRI, DCO_PWM, DCO_SIN, DCO_NOISE } DCO_wave_te;
typedef enum { DCO_NOSYNC, DCO_SYNC1, DCO_SYNC2, DCO_SYNC3, DCO_SYNC4 } DCO_sync_te;
typedef enum { DCO_12RINGMOD = 0, DCO_1234RINGMOD, DCO_12XOR, DCO_1234XOR, DCO_ADD, DCO_FM } DCO_synmode_te;

typedef struct
{
   DCO_wave_te wave_e;
   uint32_t freq_mHz_u32;
   uint16_t pwm_dc_u16;
   uint8_t level_u8;
   DCO_sync_te sync_e;
   SYNENG_mod_type_te pwm_dadsr_mod_type_e;
   SYNENG_mod_type_te freq_dadsr_mod_type_e;
   uint8_t keysync_u8;
   uint8_t sub_osc_level_u8;
   int8_t scale_i8;
   uint8_t reserved_2_u8;
   uint8_t reserved_3_u8;
   
   uint32_t fix_freq_hz_u32;
   uint32_t reserved3;
   uint32_t reserved4;
   uint32_t reserved5;
   uint32_t reserved6;
   uint32_t reserved7;
   uint32_t reserved8;
   uint32_t reserved9;
   uint32_t reserved10;
   uint32_t reserved11;
   uint32_t reserved12;
} DCO_set_ts;


typedef struct
{
   float PWM_dc_cnt_u32;   
   float cnt_u32;
   float period_cnt_u32;
   float new_period_cnt_u32;
   float new_PWM_dc_cnt_u32;
   float lastoutput_i16;
   uint8_t zero_crossed_u8;
   uint8_t new_period_u8;
   uint16_t noise_cnt_u16;
   float volume_mod_lfo_i16;
   float lastnoise_i16;
   float sub_osc_cnt_u32;   
   float sub_osc_period_cnt_u32;
   float new_sub_osc_period_cnt_u32;
} DCO_rt_ts;   


typedef struct
{
   DCO_set_ts resomod_dco_set_s;
   DCO_set_ts cutoffmod_dco_set_s;   
} DCO_vcf_lfo_ts;   


extern DCO_set_ts DCO_dco_set_as[DCO_MAX_DCOS];
extern DCO_rt_ts DCO_dco_rt_as[DCO_MAX_DCOS];

extern DCO_set_ts DCO_pwmmod_set_as[DCO_MAX_DCOS];
extern DCO_rt_ts DCO_pwmmod_rt_as[DCO_MAX_DCOS];

extern DCO_set_ts DCO_freqmod_set_as[DCO_MAX_DCOS];
extern DCO_rt_ts DCO_freqmod_rt_as[DCO_MAX_DCOS];

extern DCO_set_ts DCO_volmod_set_as[DCO_MAX_DCOS];
extern DCO_rt_ts DCO_volmod_rt_as[DCO_MAX_DCOS];

extern DCO_vcf_lfo_ts DCO_vcf_lfo_s;
extern DCO_rt_ts DCO_cutoffmod_dco_rt_s;
extern DCO_rt_ts DCO_resomod_dco_rt_s;

extern DCO_synmode_te DCO_synmode_e;
extern uint8_t DCO_fm_algorithm_u8;

extern uint8_t DCO_fm_feedback_u8;
extern int8_t DCO_master_tune_i8;

void DCO_vInit(void);
void DCO_vSetFreq(DCO_rt_ts *rt_ps, DCO_set_ts *set_ps, uint32_t mHz_u32, uint32_t sample_freq_hz_u32);
void DCO_vSetPWM(DCO_rt_ts *rt_ps, DCO_set_ts *set_ps, uint16_t dc_u16);
float DCO_i16GetNextDCOValue(DCO_rt_ts *rt_ps, DCO_set_ts *set_ps);
float DCO_i16GetNextLFOValue(DCO_rt_ts *rt_ps, DCO_set_ts *set_ps);


float DCO_i32GetDCOsOutput(void);

void DCO_vKeyOn(void); // lfo sync handling

void DCO_vUpdateRuntime(void); // must be called after loading settings



#endif
