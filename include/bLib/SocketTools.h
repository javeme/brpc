#ifndef _SocketTools_h_
#define _SocketTools_h_

#include "bluemeiLib.h"
#include "CodeUtil.h"

namespace blib{

#define CP_GBK 950

#ifdef WIN32
# define ioctl ioctlsocket
#pragma comment(lib,"ws2_32.lib")
#endif

class BLUEMEILIB_API SocketTools : public CodeUtil
{
public:
	static String getWinsockErrorMsg(int errCode);

	static int initSocketContext();
	static int cleanUpSocketContext();
};

}//end of namespace blib
#endif