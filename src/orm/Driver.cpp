#pragma once
#include "orm/Driver.h"
#include "orm/SqlExpression.h"
#include "orm/drivers/mysql/MySQLConnection.h"


namespace brpc{

DriverConnection* DriverConnection::loadDriver(const String& backend)
{
	if (backend.startWith("mysql")) {
		return new MySQLConnection();
	}

	throwpe(SQLException("Can't load driver: " + backend));
}

}//end of namespace brpc