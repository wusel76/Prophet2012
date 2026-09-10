#ifndef _CurlSerialH_
#define _CurlSerialH_

#include <windows.h>

class Logger;



class CurlSerial{
public:
	CurlSerial(Logger *log);
	~CurlSerial();

	bool initSerial(void);
	void closeSerial(void);

	inline bool isSerialOpen(void) { return serialopen; };

	void setComPort(const string &COMSTR) { com_port = COMSTR; };
	void setSerialSpeed(const string &SPEED);

   const wxString& getComPort(void) const { return com_port; };

	uint8_t readSerial(unsigned char *dataPtr, uint8_t length);
	bool writeSerial(unsigned char *data, unsigned int length);

   void flush(void);

private:
	bool serialopen;
	HANDLE hPort;
	wxString com_port;
	int	serial_speed;
   Logger *m_log;

	void sync(void);
};

#endif
