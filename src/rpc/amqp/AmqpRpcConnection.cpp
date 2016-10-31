#pragma once
// NOTE: make sure include amqp header before others(due to macro max/min)
#include "bamqp/PublisherConsumer.h"
#include "src/rpc/amqp/IOCPLogHandler.h"
#include "src/rpc/amqp/AmqpRpcConnection.h"


namespace brpc{

#define KEY_AMQP_MSG_TYPE "x-amqp-message-type"


AmqpRpcConnection::AmqpRpcConnection(cstring url,
	RpcService* dispatcher, AuthChecker* authChecker, cstring serializerType,
	unsigned int timeout, AMQP::ChannelFactory* channelFactory) :
	RpcConnection(dispatcher, authChecker, serializerType), timeout(timeout),
	channelFactory(channelFactory), deleteChannelFactory(false)
{
	Url amqpUrl(url);
	this->id = amqpUrl.getParameter("self", "node-0");
	this->peerId = amqpUrl.getParameter("destination");
	this->topic = amqpUrl.getParameter("topic");

	this->updateAliveTime();

	//this is for client
	if(channelFactory == null) {
		//remove the parameters
		String addr = String(url).splitWith("?", 2)[0];
		//TODO: pass in the `channelFactory` by user?
		static AMQP::IOCPLogHandler handler;
		this->channelFactory = new AMQP::IOCPChannelFactory(addr, &handler);
		this->deleteChannelFactory = true;

		this->connect();
	}
}

AmqpRpcConnection::~AmqpRpcConnection()
{
	if(this->deleteChannelFactory) {
		this->disconnect();
		delete this->channelFactory;
	}
}

bool AmqpRpcConnection::checkConnected()
{
	return isAlive();
}

void AmqpRpcConnection::onSend(const DataPackage& output)
{
	this->publish(output);
}

brpc::DataPackage AmqpRpcConnection::onSendSynch(const DataPackage& output)
{
	String replyMe = "replyTo-" + Util::uuid4();
	// publish callback
	RequestCallback request = [&]() {
		this->publish(output, replyMe);
	};
	// do request and wait for reply
	// NOTE: because we should declare the queue(or exchange) and bind
	// the queue to the exchange, so call the consume() before the
	// publish(), in the consume() we will call the callback method,
	// that's publish(), and wait for a reply.
	DataPackage result;
	this->consume(result, replyMe, request);
	return result;
}

bool AmqpRpcConnection::onSerializeException(SerializeException& e)
{
	ErrorHandler::handle(e);
	return false;
}

bool AmqpRpcConnection::onReturnCallException(Exception& e)
{
	ErrorHandler::handle(e);
	return false;
}

bool AmqpRpcConnection::onReceive(DataPackage& input)
{
	this->updateAliveTime();
	return invoke(input.headers, input.body);
}

bool AmqpRpcConnection::onException(Exception& e)
{
	ErrorHandler::handle(e);
	return false;
}

void AmqpRpcConnection::updateAliveTime()
{
	this->aliveTime = Date::getCurrentTime();
	this->alive = true;
}

Date AmqpRpcConnection::getAliveTime() const
{
	return this->aliveTime;
}

void AmqpRpcConnection::setAlive(bool alive)
{
	this->alive = alive;
}

bool AmqpRpcConnection::isAlive() const
{
	return this->alive;
}

void AmqpRpcConnection::setReplyTo(const String& replyTo)
{
	this->replyTo = replyTo;
}

void AmqpRpcConnection::resetReplyTo()
{
	this->replyTo = "";
}

String AmqpRpcConnection::toString() const
{
	return String::format("AMQP[%s.%s]",
		this->topic.c_str(),
		this->peerId.c_str());
}

void AmqpRpcConnection::publish(const DataPackage& output, const String& replyMe)
{
	channelFactory = this->channelFactory;
	checkNullPtr(channelFactory);

	//TODO: remove this lock after we support multi thread
	ScopedLock sl(((AMQP::IOCPChannelFactory*)channelFactory)->getLock());

	String exchange = this->topic;
	String routingKey = this->topic + "." + peerId;
	String msgType = "@cast";
	String msgId = output.headers.getDefault(KEY_PACKAGE_ID, Util::uuid1());

	AMQP::Envelope envelope((const char*)output.body.array(),
		output.body.size());

	if(!replyMe.empty()) {
		msgType = "@call";
		envelope.setReplyTo(replyMe);
	}
	else if(!replyTo.empty())
	{
		exchange = replyTo;
		routingKey = replyTo;
		msgType = "@reply";
	}
	else if(output.headers.contain(KEY_AMQP_MSG_TYPE))
	{
		output.headers.get(KEY_AMQP_MSG_TYPE, msgType);
	}

	//TODO: set publisher as a member
	ScopePointer<AMQP::Publisher> publisher = new AMQP::Publisher(
		channelFactory->connection(),
		exchange, routingKey);

	//construct envelope
	AMQP::Table headers;
	auto itor = const_cast<DataPackage&>(output).headers.iterator();
	while(itor->hasNext()) {
		auto item = itor->next();
		headers.set(item.key, item.value.c_str());
	}

	envelope.setTypeName(msgType);
	envelope.setAppID(id);
	envelope.setMessageID(msgId);
	envelope.setContentType(output.headers.getDefault(KEY_CONTENT_TYPE,
		"text/json"));
	envelope.setContentEncoding(output.headers.getDefault(KEY_CHARSET,
		"UTF-8"));
	envelope.setHeaders(headers);

	bool sucess = publisher->publish(envelope);
	AMQP::Consumer::debug("published message " + msgType
		+ " to " + exchange + ":" + routingKey);
	assert(sucess);
}

void AmqpRpcConnection::consume(DataPackage& input, const String& queue,
	const RequestCallback& request)
{
	channelFactory = this->channelFactory;
	checkNullPtr(channelFactory);

	volatile int status = 0;
	String peerAppId = this->peerId;
	String error;
	String exchange = queue;

	//TODO: remove this lock after we support multi thread
	UniqueLock& lock(((AMQP::IOCPChannelFactory*)channelFactory)->getLock());
	lock.getLock();

	ScopePointer<AMQP::Consumer> consumer = new AMQP::Consumer(
		channelFactory->connection(),
		queue, queue + "-tag");
	consumer->setDeleteExchange(true);
	consumer->setDeleteQueue(true);

	AMQP::Monitor monitor(consumer);
	consumer->bindExchange(exchange, AMQP::direct).onSuccess([&, monitor](){
		//do request(publish) when consumer is ready!
		if(monitor.valid())
			request();
	});

	consumer->subscribe([&, monitor](const AMQP::Message &message,
		uint64_t deliveryTag, bool redelivered) {

		std::string messageType = message.typeName();
		AMQP::Consumer::debug("received message: " + messageType);

		if(!monitor.valid())
			return;

		try{
			if(!message.hasAppID())
			{
				// ignore
				error = "Received message without ID: " + messageType;
			}
			else if(messageType == "@reply")
			{
				if(peerAppId != message.appID()) {
					BRpcUtil::debug(
						"Warning: unexpected app-id '%s'(expected '%s')\n",
						message.appID().c_str(),
						peerAppId.c_str());
				}
				parseMessage(input, message);
				status = 1;
			}
			else
			{
				error = "Unknown message: " + messageType;;
			}
		}catch (Exception& e){
			error = "AMQP error: " + e.toString();
		}

		consumer->channel()->ack(deliveryTag);
		if(status == 0)
			status = -1;
	});

	lock.releaseLock();

	//wait for reply
	Waiter waiter([&](){ return status != 0; }, this->timeout);
	waiter.wait();
	if(status < 0)
		throw RpcException(error);
}

void AmqpRpcConnection::parseMessage(DataPackage& package,
	const AMQP::Message& message)
{
	//TODO: parse
	package.headers.put(KEY_CONTENT_TYPE, message.contentType());
	package.headers.put(KEY_CONTENT_LEN, std::to_string(message.bodySize()));
	package.headers.put(KEY_CHARSET, message.contentEncoding());
	package.headers.put(KEY_PACKAGE_ID, message.messageID());

	package.headers.put(KEY_STATUS, (cstring)message.headers()[KEY_STATUS]);
	package.headers.put(KEY_RESPONSE, (cstring)message.headers()[KEY_RESPONSE]);

	package.body.writeBytes((const byte*)message.body(),
		(uint32)message.bodySize());
}

void AmqpRpcConnection::connect()
{
	DataPackage package;
	package.headers.put(KEY_AMQP_MSG_TYPE, "@connect");
	this->publish(package);
}

void AmqpRpcConnection::disconnect()
{
	DataPackage package;
	package.headers.put(KEY_AMQP_MSG_TYPE, "@disconnect");
	this->publish(package);
}

}//end of namespace brpc
