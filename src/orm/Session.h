#pragma once
#include "blib.h"
#include "Query.h"
#include "Colume.h"


namespace brpc{

/////////////////////////////////////////////////////////////////
//Model
class Model : public Object
{
public:
	virtual HashMap<String, String> updates() const;
	virtual void clean() const;

	virtual Colume& id() = 0;

public:
	virtual String getTableName() const {
		return String(thisClass()->getName()).toLower();
	}	
};


/////////////////////////////////////////////////////////////////
//database Session
class Session : public Object
{
public:
	Session(Connection* connection) : m_connection(connection) {
		checkNullPtr(m_connection);
	}
public:
	template <typename Cls>
	Query query()
	{
		ArrayList<String> fields;
		fields.add("*");
		return Query(Cls::tableName(), fields, CW_NONE, m_connection);
	}

	template <typename Cls>
	Query query(cstring selections[], size_t len)
	{
		ArrayList<String> fields;
		for (size_t i=0; i<len; i++) {
			fields.add(selections[i]);
		}
		if (fields.size() == 0)
			return this->query<Cls>();
		else
			return Query(Cls::tableName(), fields, CW_NONE, m_connection);
	}

	virtual void add(Model* model);

	virtual int update(Model* model);

	virtual int remove(Model* model);
protected:
	Connection* m_connection;
};

}//end of namespace brpc