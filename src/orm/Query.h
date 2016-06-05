#pragma once
#include "blib.h"
#include "Condition.h"
#include "CURD.h"
#include "Connection.h"


namespace brpc{

class Model;
class Colume;


/////////////////////////////////////////////////////////////////
//Query
class Query : public CURD
{
public:
	Query(const String& table, const ArrayList<String>& fileds,
		const ConditionWrapper& condition=CW_NONE, Connection* conn=null)
		: CURD(table), m_connection(conn),
		m_expectedFileds(fileds), m_condition(condition){
	}

	virtual Query query(cstring field);
	virtual Query query(Colume& field);

	virtual Query filter(const ConditionWrapper& condition);
	virtual Query filter(Colume& field);
	virtual Query filterById(Model& model);

	virtual String toSQL() const;
public:
	Connection* getConnection() const { return m_connection; }
	void setConnection(Connection* val) { m_connection = val; }

	virtual ArrayList<Model*> all();
	//virtual Model* one();
	//virtual Model* first();
private:
	//String m_tableName;  // from table
	ArrayList<String> m_expectedFileds;  // select *
	ConditionWrapper m_condition;  // where

	Connection* m_connection;
};


}//end of namespace brpc