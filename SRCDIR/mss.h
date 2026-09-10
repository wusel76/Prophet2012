//mss.h - header file for mss.c Midi-Control-Library
//V 1.02 (c) 11.1999 by Daniel Berger

class Logger;

void midi_setLogger(Logger *log);

int midi_init(unsigned int indevnr, unsigned int outdevnr);
int midi_exit(int noteoffs);

// hardware-dependant low-level

/*void midi_send1(unsigned char b);
void midi_read1(unsigned char *b);
void midi_send2(unsigned char b1, unsigned char b2);
void midi_send3(unsigned char b1, unsigned char b2, unsigned char b3);
void midi_read3(unsigned char *b);
void midi_sendn(unsigned char *buf, int nr);
void midi_readn(unsigned char *buf, int nr);*/


// hardware-independant high-level
int midi_check();
int midi_sysexerror();
void midi_write(unsigned char *buf, int nr);
void midi_read(unsigned char *buf);

void midi_noteon(unsigned char channel, unsigned char note, unsigned char velocity);
void midi_noteoff(unsigned char channel, unsigned char note, unsigned char velocity);
void midi_keypressure(unsigned char channel, unsigned char note, unsigned char velocity);
void midi_controlchange(unsigned char channel, unsigned char controller, unsigned char value);
void midi_programchange(unsigned char channel, unsigned char program);
void midi_channelpressure(unsigned char channel, unsigned char pressure);
void midi_pitchwheelchange(unsigned char channel, int value);

void midi_setsysexcode(unsigned char newsysexcode);
void midi_senddata(unsigned char *buf, int nr);

void midi_flush(void);

unsigned char midi_receivedata(void);
