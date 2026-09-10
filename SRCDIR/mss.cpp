int sysexerr=0;

//-------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------
#ifdef WIN32
//hardwareabhaengiger Teil - Windows 95/98/NT

#include <windows.h>
#include <mmsystem.h>
#include "logger.h"

#ifndef _CONSOLE
   #include <wx/wx.h>
#endif

#include <list>
using namespace std;

#include "types.h"

#if 1
#if defined(DEBUG_VERSION)
   #define LOG_STREAM_TO_FILE
#endif
#endif

#ifdef LOG_STREAM_TO_FILE
   #include <stdio.h>
   #include "types.h"
   FILE *mlowf;
   uint16_t mtlowbytes = 0;
   uint16_t mtlowlfcnt = 0;
   bool mtlowtx = true;
#endif

#ifndef _CONSOLE
   #define USE_DIR_MUTEX
   #define USE_IN_CALLBACK_MUTEX
   #define USE_MSS_QUEUE_MUTEX
#endif

#define ADDBUFFER_IN_CALLBACK
#define UNPREPARE_IN_OUT_CALLBACK

HMIDIIN hMidiIn;
HMIDIOUT hMidiOut;


#ifdef USE_DIR_MUTEX
   wxMutex dir_mutex;
#endif

#ifdef USE_IN_CALLBACK_MUTEX
   wxMutex in_mutex;
#endif

#define MIDI_IN_BUFFERS 32 // 4
#define MIDI_OUT_BUFFERS 6 // 4

int err;

#define MIDI_IN_BUFSIZE 200 // 768
#define MIDI_OUT_BUFSIZE 200 // 256


#define RX_QUEUE_SIZE (128*1024)
uint8_t rx_queue_au8[RX_QUEUE_SIZE];
uint32_t rx_queue_in_idx_u16;
uint32_t rx_queue_out_idx_u16;
#ifdef USE_MSS_QUEUE_MUTEX
   wxMutex mss_queue_mutex;
#endif

unsigned char midiinbuf[MIDI_IN_BUFFERS][MIDI_IN_BUFSIZE];
unsigned char midioutbuf[MIDI_OUT_BUFFERS][MIDI_OUT_BUFSIZE];
MIDIHDR midiHdr[MIDI_IN_BUFFERS];
MIDIHDR midiouthdr[MIDI_OUT_BUFFERS];

volatile unsigned char inmsgcnt;
volatile unsigned char inmsgcnt_old;

unsigned int outbuf_idx = 0;

Logger *midi_log;


void midi_setLogger(Logger *log)
{
   midi_log = log;
}


void CALLBACK MidiInProc(HMIDIIN handle, UINT message, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2){
//Main callback routine for windows - stores sysex data in internal buffer cycbuf for later readout
//read out the data with routine midi_receivedata().
/*  union {              //use this with the (commented-out) rudimentary input-normal-midi-data-part below
    DWORD dwData; 
    BYTE bData[4]; 
  } u;
  int size,t;*/
   
#ifdef USE_DIR_MUTEX
  dir_mutex.Lock();
#endif

  LPMIDIHDR lpMIDIHeader;

  unsigned char *ptr;
  unsigned int length, i;
  bool found = false;
  unsigned int fidx = 0;

#ifdef LOG_STREAM_TO_FILE
   if(mtlowtx)
   {
      mtlowbytes = 0;
      mtlowlfcnt = 0;
      mtlowtx = false;
      fprintf(mlowf, "\nRX from Prophet:\n\t");
   }
#endif

  switch(message){
  case MIM_OPEN: //is called on midiInOpen()
    break;
  case MIM_CLOSE: //is called on midiInClose()
    break;
  case MIM_DATA: //is called when a MIDI message is received; it's packed in dwParam1. dwParam2 is Time Stamp
   {
#if 0
      unsigned char status = (unsigned char) (dwParam1 & 0x000000FF);
      midi_log->log("MidiInProc() : MIM_DATA 0x%.2x", status);
      unsigned char *ptr = (unsigned char *) &dwParam1;
         midi_log->log("  0x%.2x", *ptr++);
         midi_log->log("  0x%.2x", *ptr++);
         midi_log->log("  0x%.2x", *ptr++);

#ifdef LOG_STREAM_TO_FILE
     fprintf(mlowf, "!!! DATA\n");
#endif
#endif
   }
     break;
  case MIM_MOREDATA: //this should not occur (your computer is too slow for MIDI). Anyway, these messages are handles as MIM_DATA
     midi_log->log("MidiInProc() : MIM_MOREDATA");
     break;
  case MIM_ERROR: //"invalid" MIDI message. This will be handled as a normal one.
     midi_log->log("MidiInProc() : MIM_ERROR");
    break;
    
  case MIM_LONGDATA:  //System Exclusive Message received
  //case MIM_LONGERROR: //"invalid" System Exclusive Message received
#ifdef USE_IN_CALLBACK_MUTEX
    in_mutex.Lock();
#endif
    lpMIDIHeader = (LPMIDIHDR)dwParam1;   // Assign address of MIDIHDR to a LPMIDIHDR variable. Makes it easier to access the field that contains the pointer to our block of MIDI events
    ptr = (unsigned char *)(lpMIDIHeader->lpData);     // Get address of the MIDI event that caused this call
    length=lpMIDIHeader->dwBytesRecorded;
 
    if((lpMIDIHeader->dwFlags & MHDR_DONE) == MHDR_DONE)
    {

    }

#ifdef LOG_STREAM_TO_FILE
   for(i = 0; i < (lpMIDIHeader->dwBytesRecorded); i++) 
   {
      fprintf(mlowf, "%.2X ", *(ptr+i));
      mtlowbytes++;
      mtlowlfcnt++;
      if(mtlowlfcnt >= 18)
      {
         mtlowlfcnt = 0;
         fprintf(mlowf, "\n\t");
      }
      if((*(ptr+i)) == 0xf7)
      {
         fprintf(mlowf, "\npacket size %d\n\n", mtlowbytes);
         mtlowbytes = 0;
      }
   }
#endif

    //midi_log->log("rx len:%d!!!!!!!!!!!!", lpMIDIHeader->dwBytesRecorded);

      if(*ptr == 0xf0)
      {
         for(i = 0; i < (lpMIDIHeader->dwBytesRecorded); i++)  // len -1 ???
         {
#if 0
            midi_log->log(" lrx: 0x%.2x", *(ptr + i));
#endif
#ifdef USE_MSS_QUEUE_MUTEX
            mss_queue_mutex.Lock();
#endif
            rx_queue_au8[rx_queue_in_idx_u16] = (*(ptr + i));
            rx_queue_in_idx_u16++;
            if(rx_queue_in_idx_u16 >= RX_QUEUE_SIZE)
            {
               rx_queue_in_idx_u16 = 0;
            }
#ifdef USE_MSS_QUEUE_MUTEX
            mss_queue_mutex.Unlock();
#endif
         }
      }
      else
      {
         unsigned char i;
         if(lpMIDIHeader->dwBytesRecorded == 3) // workaround
         {
            if(*(ptr) == 0x7f)
            {
#ifdef LOG_STREAM_TO_FILE
               fprintf(mlowf, "\n!!! MidiInProc() : msg len2 workaround\n");
#endif
               midi_log->log("MidiInProc() : msg len2 workaround");

#ifdef USE_MSS_QUEUE_MUTEX
               mss_queue_mutex.Lock();
#endif

               rx_queue_au8[rx_queue_in_idx_u16] = 0xf0;
               rx_queue_in_idx_u16++;
               if(rx_queue_in_idx_u16 >= RX_QUEUE_SIZE)
               {
                  rx_queue_in_idx_u16 = 0;
               }

               rx_queue_au8[rx_queue_in_idx_u16] = 0x7e;
               rx_queue_in_idx_u16++;
               if(rx_queue_in_idx_u16 >= RX_QUEUE_SIZE)
               {
                  rx_queue_in_idx_u16 = 0;
               }

               rx_queue_au8[rx_queue_in_idx_u16] = 0;
               rx_queue_in_idx_u16++;
               if(rx_queue_in_idx_u16 >= RX_QUEUE_SIZE)
               {
                  rx_queue_in_idx_u16 = 0;
               }

               rx_queue_au8[rx_queue_in_idx_u16] = 0x7f;
               rx_queue_in_idx_u16++;
               if(rx_queue_in_idx_u16 >= RX_QUEUE_SIZE)
               {
                  rx_queue_in_idx_u16 = 0;
               }

               rx_queue_au8[rx_queue_in_idx_u16] = *(ptr + 1);
               rx_queue_in_idx_u16++;
               if(rx_queue_in_idx_u16 >= RX_QUEUE_SIZE)
               {
                  rx_queue_in_idx_u16 = 0;
               }

               rx_queue_au8[rx_queue_in_idx_u16] = 0x7f;
               rx_queue_in_idx_u16++;
               if(rx_queue_in_idx_u16 >= RX_QUEUE_SIZE)
               {
                  rx_queue_in_idx_u16 = 0;
               }
#ifdef USE_MSS_QUEUE_MUTEX
               mss_queue_mutex.Unlock();
#endif
            }
         }
         else
         {
            //midi_log->log("Multiframe sysex");
            // multiframe sysex data received
#ifdef USE_MSS_QUEUE_MUTEX
            mss_queue_mutex.Lock();
#endif
            for(i = 0; i < lpMIDIHeader->dwBytesRecorded; i++)
            {
               rx_queue_au8[rx_queue_in_idx_u16] = (*(ptr + i));
               rx_queue_in_idx_u16++;
               if(rx_queue_in_idx_u16 >= RX_QUEUE_SIZE)
               {
                  rx_queue_in_idx_u16 = 0;
               }
            }
#ifdef USE_MSS_QUEUE_MUTEX
            mss_queue_mutex.Unlock();
#endif
         }
      }

#ifdef ADDBUFFER_IN_CALLBACK
      if(lpMIDIHeader->dwBytesRecorded > 0)
      {
      #if 0
         lpMIDIHeader->dwBufferLength = MIDI_IN_BUFSIZE - 1;
         lpMIDIHeader->dwFlags = 0;
         lpMIDIHeader->dwUser = 0;
         midiInPrepareHeader(hMidiIn, lpMIDIHeader, sizeof(MIDIHDR));
      #endif
         midiInAddBuffer(hMidiIn, lpMIDIHeader, sizeof(MIDIHDR));
      }
#if 0
      else
      {
         midiInUnprepareHeader(hMidiIn, lpMIDIHeader, sizeof(MIDIHDR));
      }
#endif
#endif

      //midi_log->log("rx packet len:%d:", length);
    inmsgcnt++;
#ifdef USE_IN_CALLBACK_MUTEX
  in_mutex.Unlock();
#endif
    break;
  }
  

#ifdef USE_DIR_MUTEX
  dir_mutex.Unlock();
#endif
}


void CALLBACK MidiOutProc(
  HMIDIOUT hmo,      
  UINT wMsg,         
  DWORD_PTR dwInstance,  
  DWORD_PTR dwParam1,    
  DWORD_PTR dwParam2     
)
{
#ifdef UNPREPARE_IN_OUT_CALLBACK
if (MOM_DONE == wMsg)
   {
      LPMIDIHDR hdr = (LPMIDIHDR) dwParam1;
      MMRESULT res = ::midiOutUnprepareHeader(hMidiOut, hdr, sizeof(MIDIHDR));
      hdr->dwFlags = 0;
   }
#endif
}


int midi_init(unsigned int indevnr, unsigned int outdevnr){
//initializes Midi for input device innr and output device outnr

#ifdef LOG_STREAM_TO_FILE
   mlowf = fopen("MIDI_data_lowlevel.txt", "w");
#endif

   rx_queue_in_idx_u16 = 0;
   rx_queue_out_idx_u16 = 0;

   sysexerr=0;
   bool error = false;

   outbuf_idx = 0;

   inmsgcnt = 0;
   inmsgcnt_old = 0;

  //handle indevnr=-1, outdevnr=-1 are "use default"
  if (indevnr<0) indevnr=0;    //use first midi input device as default
  if (outdevnr<0) outdevnr=0;  //use first midi output device as default
  if(indevnr>=midiInGetNumDevs()) return(-2);    //requested input device not available
  if(outdevnr>=midiOutGetNumDevs()) return(-3);  //requested output device not available

  if(midiInOpen(&hMidiIn, indevnr, (DWORD)MidiInProc, 0, CALLBACK_FUNCTION /* | MIDI_IO_STATUS */)!=MMSYSERR_NOERROR) return(-4);
#ifdef UNPREPARE_IN_OUT_CALLBACK
  if(midiOutOpen(&hMidiOut, outdevnr, (DWORD)MidiOutProc, 0, CALLBACK_FUNCTION)!=MMSYSERR_NOERROR) return(-5); 
#else
  if(midiOutOpen(&hMidiOut, outdevnr, 0 /*(DWORD)MidiOutProc*/, 0, CALLBACK_NULL /*CALLBACK_FUNCTION*/)!=MMSYSERR_NOERROR) return(-5); 
#endif

  for(int i = 0; i < MIDI_IN_BUFFERS; i++)
  {
     midiHdr[i].lpData = (char *)&midiinbuf[i][0];
     midiHdr[i].dwBufferLength = MIDI_IN_BUFSIZE - 1;
     midiHdr[i].dwFlags = 0;
     midiHdr[i].dwUser = 0;
     err = midiInPrepareHeader(hMidiIn, &midiHdr[i], sizeof(MIDIHDR));
     if (err) return(-6);

     // Queue MIDI input buffer
     err = midiInAddBuffer(hMidiIn, &midiHdr[i], sizeof(MIDIHDR));
     if (err){
        error = true;
       
       return(-7);
     }
  }

  if(error)
  {
     for(int i = 0; i < MIDI_IN_BUFFERS; i++)
     {
         midiInUnprepareHeader(hMidiIn, &midiHdr[i], sizeof(MIDIHDR));
     }
     return(-7);
  }

  if(midiInStart(hMidiIn)!=MMSYSERR_NOERROR) return(-8);  //start midi input

  return(0);
}

int midi_exit(int noteoffs)
{
//closes Midi devices. Sends Note Offs to outputs if noteoffs
//  midiInReset(hMidiIn);
  MMRESULT ret; 
  int i;

  for(i = 0; i < MIDI_IN_BUFFERS; i++)
  {
      ret = midiInUnprepareHeader(hMidiIn, &midiHdr[i], sizeof(MIDIHDR));
  }

  //ret = midiInStop (hMidiIn);
  //ret = midiInReset(hMidiIn);
  //ret = midiInClose(hMidiIn);

  for(i = 0; i < MIDI_IN_BUFFERS; i++)
  {
      ret = midiInUnprepareHeader(hMidiIn, &midiHdr[i], sizeof(MIDIHDR));
  }
#if 0
  switch(ret)
  {
      case MIDIERR_STILLPLAYING:
         midi_log->log("midiInUnprepareHeader() returns MIDIERR_STILLPLAYING");
      break;
      case MMSYSERR_INVALPARAM:
         midi_log->log("midiInUnprepareHeader() returns MMSYSERR_INVALPARAM");
      break;
      case MMSYSERR_INVALHANDLE:
         midi_log->log("midiInUnprepareHeader() returns MMSYSERR_INVALHANDLE");
      break;
  }
#endif

  ret = midiInStop (hMidiIn);
  ret = midiInReset(hMidiIn);
  ret = midiInClose(hMidiIn);

#ifdef LOG_STREAM_TO_FILE
   fclose(mlowf);
#endif
  
  switch(ret)
  {
      case MIDIERR_STILLPLAYING:
         midi_log->log("midiInClose() returns MIDIERR_STILLPLAYING");
      break;
      case MMSYSERR_INVALHANDLE:
         midi_log->log("midiInClose() returns MMSYSERR_INVALHANDLE");
      break;
      case MMSYSERR_NOMEM:
         midi_log->log("midiInClose() returns MMSYSERR_NOMEM");
      break;
      case MMSYSERR_NOERROR:
         break;
  }
  if (ret!=MMSYSERR_NOERROR) return(-1);
  
  if (noteoffs) midiOutReset(hMidiOut);

  ret = midiOutReset(hMidiOut); 
  if (midiOutClose(hMidiOut)!=MMSYSERR_NOERROR)
  {
     midi_log->log("midiOutClose() failed");
     return(-2);
  }
  return(0);
}


void midi_sendn(unsigned char *b, int n){
  UINT err;
  uint16_t i;
#ifdef LOG_STREAM_TO_FILE
   mtlowtx = true;

   mtlowbytes = 0;
   mtlowlfcnt = 0;

   fprintf(mlowf, "\nTX to Prophet:\n\t");
   for(i = 0; i < n; i++)
   {
      fprintf(mlowf, "%.2X ", b[i]);
      mtlowbytes++;
      mtlowlfcnt++;
      if(mtlowlfcnt >= 18)
      {
         mtlowlfcnt = 0;
         fprintf(mlowf, "\n\t");
      }
      if(b[i] == 0xf7)
      {
         fprintf(mlowf, "\npacket size %d\n", mtlowbytes);
         mtlowbytes = 0;
      }
   }
#endif
  //midi_log->log("send %d!!!!!!!!!!!!!", outbuf_idx);
  for(i = 0; i < MIDI_OUT_BUFSIZE; i++)
  {
     midioutbuf[outbuf_idx][i] = 0;
  }
  memcpy(&midioutbuf[outbuf_idx][0], b, n);

  midiouthdr[outbuf_idx].lpData = (char *)&midioutbuf[outbuf_idx][0]; //(LPBYTE)&b[0];
  midiouthdr[outbuf_idx].dwBytesRecorded = n;
  midiouthdr[outbuf_idx].dwBufferLength = n;
  midiouthdr[outbuf_idx].dwFlags = 0;
  midiouthdr[outbuf_idx].dwUser = 0;

#if 0
  midi_log->log("midi_sendn(%d)", n);
  int i;
  for(i = 0; i < n; i++)
  {
     midi_log->log("        tx:%.2x", midioutbuf[outbuf_idx][i]);
  }
#endif

  err = midiOutPrepareHeader(hMidiOut,  &midiouthdr[outbuf_idx], sizeof(MIDIHDR));

  switch(err)
  {
     case MMSYSERR_INVALHANDLE:
        midi_log->log("midi_sendn() : midiOutPrepareHeader == MMSYSERR_INVALHANDLE");
        break;
     case MMSYSERR_INVALPARAM:
        midi_log->log("midi_sendn() : midiOutPrepareHeader == MMSYSERR_INVALPARAM");
        break;
     case MMSYSERR_NOMEM:
        midi_log->log("midi_sendn() : midiOutPrepareHeader == MMSYSERR_NOMEM");
        break;
  }

  if (err==MMSYSERR_NOERROR)
  {
//Output the SysEx message. Note that this could return immediately if the device driver can output the message
//on its own in the background. Otherwise, the driver may make us wait in this call until the entire data is output
   //memcpy(midioutbuf, b, n);
   err = midiOutLongMsg(hMidiOut, &midiouthdr[outbuf_idx], sizeof(MIDIHDR));

   switch(err)
   {
      case MIDIERR_NOTREADY:
         midi_log->log("midi_sendn(): midiOutLongMsg == MIDIERR_NOTREADY");
         break;
      case MIDIERR_UNPREPARED:
         midi_log->log("midi_sendn(): midiOutLongMsg == MIDIERR_UNPREPARED");
         break;
      case MMSYSERR_INVALHANDLE:
         midi_log->log("midi_sendn(): midiOutLongMsg == MMSYSERR_INVALHANDLE");
         break;
      case MMSYSERR_INVALPARAM:
         midi_log->log("midi_sendn(): midiOutLongMsg == MMSYSERR_INVALPARAM");
         break;
   }
//Sleep(1000);
//Sleep(n*2);
#ifndef UNPREPARE_IN_OUT_CALLBACK
    // Unprepare the buffer and MIDIHDR
    while (MIDIERR_STILLPLAYING == midiOutUnprepareHeader(hMidiOut, &midiouthdr[outbuf_idx], sizeof(MIDIHDR)))
    {
      // Delay to give it time to finish
      Sleep(1);
    }
#endif
  }
   outbuf_idx++;
   if(outbuf_idx >= MIDI_OUT_BUFFERS)
   {
      outbuf_idx = 0;
   }
}

UINT sendMIDIEvent(HMIDIOUT hMidiOut, BYTE bStatus, BYTE bData1, BYTE bData2) {
//used internally
  union { 
    DWORD dwData; 
    BYTE bData[4]; 
  } u; 
 
  // Construct the MIDI message
  u.bData[0] = bStatus; 
  u.bData[1] = bData1; 
  u.bData[2] = bData2; 
  u.bData[3] = 0; 
 
  // Send the message
  return midiOutShortMsg(hMidiOut, u.dwData); 
} 

void midi_send2(unsigned char b1, unsigned char b2){
  sendMIDIEvent(hMidiOut,b1,b2,0);
}

void midi_send3(unsigned char b1, unsigned char b2, unsigned char b3){
  sendMIDIEvent(hMidiOut,b1,b2,b3);
}

void lowlevel_midi_senddata(unsigned char *buf, int nr){
//precondition: 0<=sysexcode<128
//this is an easy version with 4-bit coding
   //  int i;

#ifdef USE_DIR_MUTEX
  dir_mutex.Lock();
#endif

  //send MIDI message
  midi_sendn(buf,nr);

#ifdef USE_DIR_MUTEX
  dir_mutex.Unlock();
#endif
}

void midi_readn(unsigned char *b, int n){} //Windows-Dummy for almost unimplementable function (at least it's not worth implementing)

#endif



////////////////////////////////////////////////////////////////////////////////////////////
//Hardwareunabhaengiger Teil
////////////////////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------------------
//Routinen zum Datenuebertragen
//(Nicht MIDI-konforme Rohdaten; deshalb nur zur Datenuebertragung von Rechner zu Rechner
//verwenden. Synthesizer in der MIDI-Chain wuerden diese Rohdaten missverstehen!)

int midi_check()
{
   int retval = 0;

   if(inmsgcnt != inmsgcnt_old)
   {
      inmsgcnt_old = inmsgcnt;
#ifndef ADDBUFFER_IN_CALLBACK
      for (int i = 0; i < MIDI_IN_BUFFERS; i++)
      {
	      if (midiHdr[i].dwFlags==(MHDR_DONE|MHDR_PREPARED)){
    //midi_log->log("add MIDI in buffer %d", i);
	        // add a buffer if one is free
           midiInUnprepareHeader( hMidiIn,&midiHdr[i], sizeof(MIDIHDR));
	        midiHdr[i].dwBufferLength = MIDI_IN_BUFSIZE-1;
           midiHdr[i].dwBytesRecorded = 0;
	        midiHdr[i].dwUser = 0;
	        midiHdr[i].dwFlags = 0;
	        midiInPrepareHeader(hMidiIn,&midiHdr[i], sizeof(MIDIHDR));
	        midiInAddBuffer(hMidiIn,&midiHdr[i],sizeof(MIDIHDR));
	      }	
      }
#endif
   }

#ifdef USE_DIR_MUTEX
   dir_mutex.Lock();
#endif

#ifdef USE_MSS_QUEUE_MUTEX
   mss_queue_mutex.Lock();
#endif
   if(rx_queue_in_idx_u16 != rx_queue_out_idx_u16)
   {
      retval = 1;
   }
#ifdef USE_MSS_QUEUE_MUTEX
   mss_queue_mutex.Unlock();
#endif

#ifdef USE_DIR_MUTEX
   dir_mutex.Unlock();
#endif
   return retval;
}

int midi_sysexerror(){
//Windows: returns 1 if sysex data has been lost due to buffer overflow since last call of sysexerror(), 0 else
//Linux: returns 0 always (it is assumed that data is never lost)
  int k=sysexerr;
  sysexerr=0;
  return(k);
}

void midi_write(unsigned char *buf, int nr){
//sends out nr bytes to MIDI device
  midi_sendn(buf,nr);
}

void midi_read(unsigned char *buf, int nr){
//reads in nr bytes from MIDI device. Waits if necessary
//CAUTION! This works when using Linux, but not under Windows (95/98/NT).
  midi_readn(buf,nr);
}

//----------------------------------------------------------------------------------------
//Routinen fuer Standard-Midi Output
//Steuerung von Synthesizern entsprechend dem Standard-Midi-Protokoll

void midi_noteon(unsigned char channel, unsigned char note, unsigned char velocity){
  midi_send3(0x90+(channel&15),note&127,velocity&127);
}

void midi_noteoff(unsigned char channel, unsigned char note, unsigned char velocity){
  midi_send3(0x80+(channel&15),note&127,velocity&127);
}

void midi_keypressure(unsigned char channel, unsigned char note, unsigned char velocity){
  midi_send3(0xa0+(channel&15),note&127,velocity&127);
}

void midi_controlchange(unsigned char channel, unsigned char controller, unsigned char value){
  midi_send3(0xb0+(channel&15),controller&127,value&127);
}

void midi_programchange(unsigned char channel, unsigned char program){
  midi_send2(0xc0+(channel&15),program&127);
}

void midi_channelpressure(unsigned char channel, unsigned char pressure){
  midi_send2(0xd0+(channel&15),pressure&127);
}

void midi_pitchwheelchange(unsigned char channel, int value){
  //value is 14-bit-value(0..16383); center (no change) is 8192.  
  midi_send3(0xe0+(channel&15),value&127,(value>>7)&127);
}

//----------------------------------------------------------------------------------------
// System Exclusive Data Transfer
// MIDI-konforme Datenuebertragung von Rechner zu Rechner.
// Diese Form der Datenuebertragung ist etwas langsamer, aber vertraegt sich mit
// Fremdsynthesizern in der MIDI-Chain (falls nicht gerade die Sysex-Codes der jeweiligen
// Synthesizer verwendet werden)


void midi_senddata(unsigned char *buf, int nr){
//precondition: 0<=sysexcode<128
  lowlevel_midi_senddata(buf, nr);
}

unsigned char midi_receivedata(void)
{
   unsigned char retval;
#ifdef USE_DIR_MUTEX
   dir_mutex.Lock();
#endif

#ifdef USE_MSS_QUEUE_MUTEX
   mss_queue_mutex.Lock();
#endif
   retval = rx_queue_au8[rx_queue_out_idx_u16];
   rx_queue_out_idx_u16++;
   if(rx_queue_out_idx_u16 >= RX_QUEUE_SIZE)
   {
      rx_queue_out_idx_u16 = 0;
   }
#ifdef USE_MSS_QUEUE_MUTEX
   mss_queue_mutex.Unlock();
#endif

#ifdef USE_DIR_MUTEX
   dir_mutex.Unlock();
#endif
   return retval;
}

void midi_flush(void)
{
//   int i;
#ifdef USE_MSS_QUEUE_MUTEX
   mss_queue_mutex.Lock();
#endif
   rx_queue_in_idx_u16 = 0;
   rx_queue_out_idx_u16 = 0;
#ifdef USE_MSS_QUEUE_MUTEX
   mss_queue_mutex.Unlock();
#endif

#if 0
  for(i = 0; i < MIDI_IN_BUFFERS; i++)
  {
      midiInUnprepareHeader(hMidiIn, &midiHdr[i], sizeof(MIDIHDR));
  }
  for(i = 0; i < MIDI_IN_BUFFERS; i++)
  {
     midiHdr[i].lpData = (char *)&midiinbuf[i][0];
     midiHdr[i].dwBufferLength = MIDI_IN_BUFSIZE;
     midiHdr[i].dwFlags = 0;
     err = midiInPrepareHeader(hMidiIn, &midiHdr[i], sizeof(MIDIHDR));

     // Queue MIDI input buffer
     err = midiInAddBuffer(hMidiIn, &midiHdr[i], sizeof(MIDIHDR));
  }
  midiInStart(hMidiIn);
#endif
}

