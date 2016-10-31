#pragma once
#include "blib/BString.h"
#include "blib/Log.h"
#include "orm/Driver.h"


namespace brpc{

typedef class Connection : public Object
{
public:
	Connection(const String& connInfo)
	{
		SQLExpression::debug("Open a database connection...\n");
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
		SQLExpression::debug("Close a database connection...\n");
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
		SQLExpression::debug(">>>> query SQL: %s", sql);
		// call driver
		return m_driverConnection->executeQuery(sql);
	}

	// return changed rows number
	virtual int excute(cstring sql)
	{
		SQLExpression::debug(">>>> excute SQL: %s", sql);
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