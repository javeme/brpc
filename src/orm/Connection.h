#pragma once
#include "blib.h"
#include "Driver.h"


namespace brpc{


typedef class Connection : public Object
{
public:
	Connection(const String& connInfo)
	{
		m_driverConnection = DriverConnection::loadDriver(connInfo);
		checkNullPtr(m_driverConnection);

		try {
			m_driverConnection->open(connInfo);
		} catch (...) {
			delete m_driverConnection;
			m_driverConnection = null;
			throw;
		}
	}
	virtual ~Connection()
	{
		try {
			m_driverConnection->close();
		} catch (Exception& e) {
			Log::getLogger()->warn(e.toString());
		}
		delete m_driverConnection;
	}
public:
	virtual ResultSet* query(cstring sql)
	{
		BRpcUtil::debug(">>>> excute SQL(query): %s\n", sql);
		// call driver
		return m_driverConnection->executeQuery(sql);
	}

	// return changed rows number
	virtual int excute(cstring sql)
	{
		BRpcUtil::debug(">>>> excute SQL: %s\n", sql);
		// call driver
		return m_driverConnection->executeUpdate(sql);
	}

	virtual int excute(const Action& curd)
	{
		return excute(curd.toSQL());
	}

private:
	DriverConnection* m_driverConnection;

} DatabaseConnection;


}//end of namespace brpc