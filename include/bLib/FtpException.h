#pragma once

#include "bluemeiLib.h"
#include "Exception.h"

namespace bluemei{


#define REFUSE -1
#define PASSWORD_EEROR -2
#define SET_BINARY_FAILED -3
#define CLOSE_CON_FAILED -4
#define SET_PASV_FAILED -5
#define SET_PORT_FAILED -6
#define SET_STOR_FAILED -7
#define SET_REST_FAILED -8
#define UPLOAD_FILE_FAILED	-9

class BLUEMEILIB_API FtpException :public Exception
{
public:
	FtpException(void){};
	FtpException(int type,cstring msg);
	FtpException(int type,const String& msg);
	virtual ~FtpException(void);
public:
	virtual String name() const;
private:
	int m_nErrorType;
};


}//end of namespace bluemei