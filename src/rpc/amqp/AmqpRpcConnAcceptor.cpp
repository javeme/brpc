#pragma once
// NOTE: make sure include amqp header before others(due to macro max/min)
#include "bamqp/PublisherConsumer.h"
#include "bamqp/IOCPChannelFactory.h"
#include "src/rpc/amqp/IOCPLogHandler.h"
#include "src/rpc/amqp/AmqpRpcConnAcceptor.h"
#include "src/rpc/amqp/AmqpRpcConnection.h"
#include "src/rpc/trans/RpcSocketFactory.h"


namespace brpc{

///////////////////////////////////////////////////////////////////////////
//AmqpRpcConnAcceptor
AmqpRpcConnAcceptor::AmqpRpcConnAcceptor(RpcServer* server)
	: RpcConnAcceptor(server), running(false)
{
	checkNullPtr(server);
	Url url(server->getUrl());
	this->nodeId = url.getParameter("self");
	this->topic = url.getParameter("topic", server->getName());
	this->url = url.toString().splitWith("?", 2)[0];
}

void AmqpRpcConnAcceptor::run()
{
	Log* logger = Log::getLogger();
	try{
		waitConnection();
	}catch (Exception& e) {
		BRpcUtil::debug("Failed to connect to MQ server.\n");
		logger->error("Failed to connect to MQ server: " + e.toString());
	}catch (std::exception& e){
		logger->error("Failed to connect to MQ server: " + String(e.what()));
	}catch (...){
		logger->error("Failed to connect to MQ server: unknown error");
	}
}

void AmqpRpcConnAcceptor::waitConnection()
{
	Log* logger = Log::getLogger();

	checkNullPtr(server);

	//"amqp://guest:guest@127.0.0.1:5672/"
	AMQP::IOCPLogHandler handler;
	AMQP::IOCPChannelFactory channelFactory(this->url, &handler);

	AMQP::Connection* connection = channelFactory.connection();
	String exchange = server->getName();
	String topic = this->topic;
	String node = this->nodeId;
	String key = topic + "." + node;
	String queue = topic + "." + node;
	String queueTag = queue + "-tag";

	//Consumer (throw Exception)
	ScopePointer<AMQP::Consumer> consumer = new AMQP::Consumer(
		connection,
		queue,
		queueTag);
	consumer->bindExchange(exchange, AMQP::topic, key).onSuccess([](){
		AMQP::Consumer::debug("bind consumer to exchange success!");
	});
	//consumer->setDeleteQueue(true);

	consumer->subscribe([&](const AMQP::Message &message, uint64_t deliveryTag,
		bool redelivered) {

		const std::string messageType = message.typeName();
		const std::string peerId = message.appID();

		AMQP::Consumer::debug("received message " + messageType
			+ " from " + peerId);

		try{
			if(!message.hasAppID())
			{
				// ignore message without ID
				logger->debug("Received message without ID: " + messageType);
			}
			else if(messageType == "@connect")
			{
				this->addConnection(peerId, &channelFactory);
			}
			else if(messageType == "@disconnect")
			{
				this->removeConnection(peerId);
			}
			else if(messageType == "@keepconnect")
			{
				this->getConnection(peerId)->updateAliveTime();
			}
			else if(messageType == "@cast")
			{
				AmqpRpcConnection* conn = this->getConnection(peerId);
				DataPackage package;
				AmqpRpcConnection::parseMessage(package, message);
				conn->onReceive(package);
			}
			else if(messageType == "@call")
			{
				AmqpRpcConnection* conn = this->getConnection(peerId);
				conn->setReplyTo(message.replyTo());
				DataPackage package;
				AmqpRpcConnection::parseMessage(package, message);
				conn->onReceive(package);
			}
			else
			{
				logger->warn("Unknown message: " + messageType);
			}
		}catch (Exception& e){
			logger->warn("AMQP error: " + e.toString());
			server->onError(peerId.c_str(), "handle-amqp-error", null);
		}

		consumer->channel()->ack(deliveryTag);
	});

	this->running = true;
	while(this->running)
	{
		//TODO: how long to sleep and timeout?
		Thread::sleep(1000);
		this->checkAlive(server->getTimeout());
	}
}

void AmqpRpcConnAcceptor::addConnection(String id,
	AMQP::ChannelFactory* channelFactory)
{
	checkNullPtr(server);
	AmqpRpcConnection* conn = new AmqpRpcConnection(
		server->getUrl(),
		server->getDispatcher(),
		server->getAuthChecker(),
		server->getSerializerType(),
		server->getTimeout(),
		channelFactory);
	conn->setPeerId(id);
	conn->setId(this->nodeId);
	conn->setTopic(this->topic);
	this->connections.put(id, conn);
	this->server->addConnection(conn);
}

void AmqpRpcConnAcceptor::removeConnection(String id)
{
	AmqpRpcConnection* conn = null;
	this->connections.remove(id, conn);
	this->server->removeConnection(conn); //conn will be delete
}

AmqpRpcConnection* AmqpRpcConnAcceptor::getConnection(String id)
{
	AmqpRpcConnection** conn = this->connections.get(id);
	if(conn == null) {
		throw NotFoundException("No AmqpRpcConnection named " + id);
	}
	return *conn;
}

void AmqpRpcConnAcceptor::checkAlive(unsigned int timeout)
{
	Date now = Date::getCurrentTime();

	auto itor = this->connections.iterator();
	while(itor->hasNext())
	{
		auto conn = itor->next().value;
		uint64 ms = now - conn->getAliveTime();
		if(ms > timeout)
			conn->setAlive(false);
	}
}

void AmqpRpcConnAcceptor::stop()
{
	this->running = false;
	this->wait();
}

void AmqpRpcConnAcceptor::wait()
{
	RpcConnAcceptor::wait();
}

}//end of namespace brpc
