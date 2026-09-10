#ifndef __SYNTHENGH__
#define __SYNTHENGH__

#define SYNENG_MAX_TRANSPOSE 48
#define SYNENG_MAX_TUNE 50
#define SYNENG_CTRL_INTERVAL_DIV 32  // f = SAMPLEFREQ / SYNENG_CTRL_INTERVAL_DIV
#define SYNENG_CTRL_FREQ (SAMPLEFREQ / SYNENG_CTRL_INTERVAL_DIV)
#define SYNENG_CTRL_ELEMENTS (OUTBUF_SIZE / SYNENG_CTRL_INTERVAL_DIV)
#define SYNENG_MAX_VELO_FILT 127
#define SYNENG_MAX_MOD_FILT 127
#define SYNENG_MAX_VELO_VCA 127
#define SYNENG_MAX_FILT_LFO 127
#define SYNENG_MAX_KT_MOD  127
#define SYNENG_MAX_PORTAMENTO_TIME 5000


#define SYNENG_MAX_NAME_LENGTH   15

typedef enum { SYNENG_NOMOD, SYNENG_NMOD, SYNENG_PMOD } SYNENG_mod_type_te;

typedef struct
{
   uint32_t cutoff_u32;
   uint32_t resonance_u32;
   uint32_t vca_u32;
   uint16_t out_idx_u16;
} SYNENG_ctrldata_ts;

typedef struct
{
   int8_t tune_i8;
   int8_t transpose_i8;
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
} SYNENG_dco_set_ts;   


typedef struct
{
   uint8_t level_u8;
   int8_t velocity_mod_i8;
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
} SYNENG_vca_set_ts;

typedef struct
{
   uint8_t cutoff_u8;
   SYNENG_mod_type_te dadsr_cutoff_mod_type_e;
   int8_t velocity_cutoff_mod_i8;
   int8_t modwheel_cutoff_mod_i8;
   int8_t kt_cutoff_mod_i8;
   uint16_t resonance_u8; 
   SYNENG_mod_type_te dadsr_res_mod_type_e;
   int8_t velocity_res_mod_i8;
   int8_t modwheel_res_mod_i8;
   int8_t kt_res_mod_i8;
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
} SYNENG_vcf_set_ts;

extern uint16_t SYNENG_calc_ctrl_cnt_u16;

extern uint8_t SYNENG_name_au8[SYNENG_MAX_NAME_LENGTH + 1];
extern SYNENG_dco_set_ts SYNENG_dco_set_as[DCO_MAX_DCOS];

extern volatile uint16_t SYNENG_sample_out_idx_u16;
extern volatile uint16_t SYNENG_sample_in_idx_u16;

extern volatile uint16_t SYNENG_ctrl_in_idx_u16;
extern volatile uint16_t SYNENG_ctrl_out_idx_u16;

extern SYNENG_ctrldata_ts SYNENG_ctrldata_as[(OUTBUF_SIZE / SYNENG_CTRL_INTERVAL_DIV) + 1];

extern uint32_t SYNENG_buf_au32[OUTBUF_SIZE];

extern uint16_t SYNENG_portamento_time_u16;

extern SYNENG_vca_set_ts SYNENG_vca_set_s;
extern SYNENG_vcf_set_ts SYNENG_vcf_set_s;
extern uint8_t SYNENG_calc_audio_u8;

void SYNENG_vInit(void);
void SYNENG_vHandler(void);

void SYNENG_vKeyOn(uint8_t key_u8, uint8_t velocity_u8);
void SYNENG_vKeyOff(uint8_t key_u8);


#endif
