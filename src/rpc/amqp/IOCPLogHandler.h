#pragma once
#include "IOCPHandler.h"
#include "IOCPChannelFactory.h"
#include "BRpcUtil.h"
#include "Log.h"


namespace AMQP{

class IOCPConnection;
using brpc::BRpcUtil;
using brpc::Log;


///////////////////////////////////////////////////////////////////////////
//IOCPLogHandler
class IOCPLogHandler : public AMQP::IOCPHandler
{
public:
    virtual uint16_t onNegotiate(IOCPConnection *connection, uint16_t interval)
    {
        BRpcUtil::debug("AMQP.onNegotiate()\n");
        Log::getLogger()->info("AMQP.onNegotiate()");
        return interval;
    }

    virtual void onConnected(IOCPConnection *connection)
    {
        BRpcUtil::debug("AMQP.onConnected()\n");
        Log::getLogger()->info("AMQP.onConnected()");
    }

    virtual void onHeartbeat(IOCPConnection *connection)
    {
        BRpcUtil::debug("AMQP.onHeartbeat()\n");
        Log::getLogger()->info("AMQP.onHeartbeat()");
    }

    virtual void onError(IOCPConnection *connection, const char *message)
    {
        BRpcUtil::debug("AMQP.onError(%s)\n", message);
        Log::getLogger()->info("AMQP.onError()");
    }

    virtual void onClosed(IOCPConnection *connection)
    {
        BRpcUtil::debug("AMQP.onClosed()\n");
        Log::getLogger()->info("AMQP.onClosed()");
    }
};

}//end of namespace brpc