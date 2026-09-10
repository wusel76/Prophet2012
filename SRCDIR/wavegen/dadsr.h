#ifndef __DADSRH__
#define __DADSRH__


// max time: 255 (due to internal x^2 calcs)
#define DADSR_MAX_OUTPUT   32767
#define DADSR_MAX_OUTPUT_LEVEL   127

#define DADSR_DCO_PWM_MAXTIME 127
#define DADSR_DCO_FREQ_MAXTIME 127
#define DADSR_VCF_CUTOFF_MAXTIME 127
#define DADSR_VCF_RESONANCE_MAXTIME 127
#define DADSR_VCA_MAXTIME 127
#define DADSR_DCO_OUTPUT_MAXTIME DADSR_VCA_MAXTIME

typedef enum { DADSR_DELAY, DADSR_ATTACK, DADSR_DECAY, DADSR_SUSTAIN, DADSR_RELEASE, DADSR_STOP } DADSR_state_te;

typedef struct
{
   uint64_t delay_time_u16;
   uint64_t attack_time_u16;
   uint64_t decay_time_u16;
   uint16_t sustain_level_u16;
   uint64_t release_time_u16;
   uint8_t output_level_u8;
   uint32_t reserved1;
   uint32_t reserved2;
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
} DADSR_dadsr_data_ts;


typedef struct
{
   DADSR_state_te state_e;
   uint64_t DADSR_time_1ms_u16;
   float DADSR_internal_output_u16;
   float DADSR_output_u16;
   float DADSR_release_start_level_u16;
   uint64_t DADSR_dest_release_time_u16;
   uint64_t DADSR_dest_attack_time_u16;
   uint64_t DADSR_dest_decay_time_u16;
   uint64_t DADSR_dest_delay_time_u16;
} DADSR_runtime_data_ts;


extern DADSR_dadsr_data_ts DADSR_pwmmod_data_as[DCO_MAX_DCOS];
extern DADSR_runtime_data_ts DADSR_pwmmod_rt_as[DCO_MAX_DCOS];
extern DADSR_dadsr_data_ts DADSR_freqmod_data_as[DCO_MAX_DCOS];
extern DADSR_runtime_data_ts DADSR_freqmod_rt_as[DCO_MAX_DCOS];

extern DADSR_dadsr_data_ts DADSR_dcoout_data_as[DCO_MAX_DCOS];
extern DADSR_runtime_data_ts DADSR_dcoout_rt_as[DCO_MAX_DCOS];

extern DADSR_dadsr_data_ts DADSR_filtmod_data_s;
extern DADSR_runtime_data_ts DADSR_filtmod_rt_s;

extern DADSR_dadsr_data_ts DADSR_resmod_data_s;
extern DADSR_runtime_data_ts DADSR_resmod_rt_s;

extern DADSR_dadsr_data_ts DADSR_vcamod_data_s;
extern DADSR_runtime_data_ts DADSR_vcamod_rt_s;

void DADSRH_vInit(void);

void DADSRH_vHandler1ms(void);
void DADSRH_vHandleCtrlEnvs(void);


void DADSRH_vKeyOn(void);
void DADSRH_vKeyOff(void);

float DADSRH_u16GetOutput(DADSR_runtime_data_ts *dadsr_rt_ps);

void DADSRH_vProgramInit(void);


#endif

