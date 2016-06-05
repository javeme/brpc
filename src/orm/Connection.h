#pragma once
#include "blib.h"


namespace brpc{

typedef class Connection : public Object
{
public:
	virtual String query(const String& sql)
	{
		printf(">>>> excute SQL(query): %s\n", sql.c_str());
		// TODO: call driver
		return "";
	}

	virtual int excute(const String& sql)
	{
		printf(">>>> excute SQL: %s\n", sql.c_str());
		// TODO: call driver
		return -1;
	}

	virtual int excute(const CURD& curd)
	{
		return excute(curd.toSQL());
	}
}DbConnection;


}//end of namespace brpc