#include "types.h"

#include <windows.h>
#include <stdio.h>

#include <wx/string.h>

#include <string>
#include <iostream>
using namespace std;

#include "CurlSerial.h"
#include "logger.h"

CurlSerial::CurlSerial(Logger *log) : com_port("COM4"), serialopen(false), serial_speed(CBR_38400), m_log(log)
{
	hPort = 0;
}


CurlSerial::~CurlSerial()
{
	if(hPort != 0){
		CloseHandle(hPort);  
	}
}


bool CurlSerial::initSerial(void)
{
  bool retVal = true;

	// variables used with the com port
	BOOL     m_bPortReady;
	DCB      m_dcb;

   m_log->log("Serial::initSerial()");

   // open serial port
	hPort = CreateFile(com_port.wc_str(), GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_NO_BUFFERING, NULL);
	if (hPort != INVALID_HANDLE_VALUE) {
		PurgeComm(hPort, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);

      m_bPortReady = SetupComm(hPort, 512, 512); // set buffer sizes
		if(!m_bPortReady){
			m_log->log("SetupComm failed");
		}

		m_bPortReady = GetCommState(hPort, &m_dcb);
		if(!m_bPortReady){
			m_log->log("GetCommState failed");
		}

      m_dcb.fBinary = TRUE;               // Binary mode; no EOF check 


		m_dcb.BaudRate = serial_speed; 

		m_dcb.ByteSize = 8;
		m_dcb.Parity = NOPARITY;
		m_dcb.StopBits = ONESTOPBIT;
		m_dcb.fAbortOnError = FALSE;

      m_dcb.fTXContinueOnXoff = TRUE;     // XOFF continues Tx
      
		m_dcb.fDsrSensitivity = FALSE;
		m_dcb.fOutxCtsFlow = FALSE;
		m_dcb.fOutxDsrFlow = FALSE;
		m_dcb.fOutX = FALSE;
		m_dcb.fInX = FALSE;
      m_dcb.fErrorChar = FALSE;           // Disable error replacement 
 
      m_dcb.fDtrControl = DTR_CONTROL_DISABLE;
      m_dcb.fRtsControl = RTS_CONTROL_DISABLE;

		// setting timeouts
		COMMTIMEOUTS Timeouts;
		ZeroMemory(&Timeouts, sizeof(COMMTIMEOUTS));
		Timeouts.ReadIntervalTimeout = MAXDWORD;
		Timeouts.ReadTotalTimeoutMultiplier = 0;  // 1     uart:1
		Timeouts.ReadTotalTimeoutConstant = 0;    // 0    uart:10
		Timeouts.WriteTotalTimeoutMultiplier = 1; // 1     uart:1
		Timeouts.WriteTotalTimeoutConstant = 1;   // 0     uart:1

      if(!SetCommTimeouts(hPort, &Timeouts)){
			m_log->log("Could not set timeouts");
		}


		m_bPortReady = SetCommState(hPort, &m_dcb);
		

		if (m_bPortReady)
      {
         m_log->log("   serial open");
			serialopen = true;
			sync();
         Sleep(500);
		}
      else
      {
			retVal = false;
		}


	//	FlushFileBuffers(hPort);
	//	PurgeComm(hPort, PURGE_RXCLEAR | PURGE_TXCLEAR);
      flush();
		// closing port
	}else{
		return false;
	}
	return retVal;
}



void CurlSerial::closeSerial(void)
{
   m_log->log("Serial::closeSerial()");
	if(hPort != 0){
		CloseHandle(hPort);  
		hPort = 0;
	}
	serialopen = false;
}


uint8_t CurlSerial::readSerial(unsigned char *dataPtr, uint8_t length)
{
	uint8_t retval = 0;

	//Sleep(10);

	//FlushFileBuffers(hPort);

	DWORD	nRead;

	nRead = 0;

	if (!ReadFile(hPort, dataPtr, length, &nRead, NULL))
   {
		//GetCommState(hPort, &dcb); 
	}

   retval = nRead;

	return retval;
}


void CurlSerial::flush(void)
{
	FlushFileBuffers(hPort);	

   PurgeComm(hPort, PURGE_TXABORT | PURGE_RXABORT); // PURGE_RXCLEAR | PURGE_TXCLEAR);
	PurgeComm(hPort, PURGE_RXCLEAR | PURGE_TXCLEAR);
}


// true = ok  false = error
bool CurlSerial::writeSerial(unsigned char *data, unsigned int length)
{
	bool retVal = true;
	unsigned long int n_out = 0;
	
	retVal = (bool) WriteFile(hPort, data, length, &n_out, NULL);
	if(retVal){
		if(n_out != 1){
			retVal = false;
		}
	}

	//FlushFileBuffers(hPort);
	
//	SetCommMask(hPort, EV_TXEMPTY);
//	PurgeComm(hPort, PURGE_RXCLEAR | PURGE_TXCLEAR);

	return retVal;
}


void CurlSerial::sync(void)
{
	DWORD	nRead;
	FlushFileBuffers(hPort);	
	SetCommMask(hPort, EV_TXEMPTY);
	PurgeComm(hPort, PURGE_TXABORT); // PURGE_RXCLEAR | PURGE_TXCLEAR);
	PurgeComm(hPort, PURGE_RXCLEAR | PURGE_TXCLEAR);

	//writeSerial(UNKNOWN_CMD);
	//writeSerial(UNKNOWN_CMD);

	PurgeComm(hPort, PURGE_TXABORT); // PURGE_RXCLEAR | PURGE_TXCLEAR);
	PurgeComm(hPort, PURGE_RXCLEAR | PURGE_TXCLEAR);

	nRead = 0;
#if 0
	if (!ReadFile(hPort, ctmp, 1, &nRead, NULL)){
			GetCommState(hPort, &dcb); 
	}
#endif
}

void CurlSerial::setSerialSpeed(const string &SPEED)
{
	if(SPEED == "4800")
	{
		serial_speed = CBR_4800;
	}else
	if(SPEED == "9600")
	{
		serial_speed = CBR_9600;
	}else
	if(SPEED == "19200")
	{
		serial_speed = CBR_19200;
	}else
	if(SPEED == "38400")
	{
		serial_speed = CBR_38400;
	}else
	if(SPEED == "57600")
	{
		serial_speed = CBR_57600;
	}else
	if(SPEED == "115200")
	{
		serial_speed = CBR_115200;
	}else
	if(SPEED == "256000")
	{
		serial_speed = CBR_256000;
	}
}



