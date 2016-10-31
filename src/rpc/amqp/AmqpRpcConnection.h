#pragma once
#include "blib/Date.h"
#include "bamqp/ChannelFactory.h"
#include "src/rpc/RpcConnection.h"
#include "src/rpc/trans/RpcSocket.h"

namespace AMQP{ class Message; }


namespace brpc{

/*
* 基于AMQP协议RPC远程调用环境
* @author Javeme
* @create 2014/7/13
*/
class AmqpRpcConnection : public RpcConnection,
	public RpcReceiveListener
{
public:
	AmqpRpcConnection(cstring url,
		RpcService* dispatcher, AuthChecker* authChecker,
		cstring serializerType, unsigned int timeout=0,
		AMQP::ChannelFactory* channelFactory=nullptr);
	virtual ~AmqpRpcConnection();

public:
	String getTopic() const { return topic; }
	void setTopic(const String& val) { topic = val; }

	String getId() const { return id; }
	void setId(const String& val) { id = val; }

	String getPeerId() const { return peerId; }
	void setPeerId(const String& val) { peerId = val; }

	virtual bool checkConnected();

	virtual void updateAliveTime();
	virtual Date getAliveTime() const;

	virtual void setAlive(bool alive);
	virtual bool isAlive() const;

	virtual void setReplyTo(const String& replyTo);
	virtual void resetReplyTo();

public:
	virtual void onSend(const DataPackage& output);
	virtual DataPackage onSendSynch(const DataPackage& output);

	virtual bool onSerializeException(SerializeException& e);
	virtual bool onReturnCallException(Exception& e);

	virtual bool onReceive(DataPackage& input);
	virtual bool onException(Exception& e);

	virtual String toString() const;

public:
	static void parseMessage(DataPackage& package, const AMQP::Message& message);

protected:
	virtual void publish(const DataPackage& output, const String& replyMe="");

	typedef std::function<void()> RequestCallback;
	virtual void consume(DataPackage& input, const String& queue,
		const RequestCallback& request);

	virtual void connect();
	virtual void disconnect();
private:
	String topic;
	String id, peerId;
	String replyTo;

	bool alive;
	Date aliveTime;

	unsigned int timeout;

	AMQP::ChannelFactory* channelFactory;
	bool deleteChannelFactory;
};

}//end of namespace brpc