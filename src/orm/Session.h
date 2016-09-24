#pragma once
#include "blib.h"
#include "Query.h"
#include "Column.h"


namespace brpc{

class QueryResult;
class ResultSet;


/////////////////////////////////////////////////////////////////
//Model
class Model : public Object
{
public:
	virtual FiledValues updates(bool mustUpdated=true) const;
	virtual void clean() const;
	virtual void fromDatabase(ResultSet* result);

	virtual Column& id() = 0;
public:
	virtual String getTableName() const;

public:
	static QueryResult fromDatabase(ResultSet* result, const Class* cls);
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
	Query query() { return this->query<Cls>(null, 0); }

	template <typename Cls>
	Query query(cstring selections[], size_t len) {
		return this->query(Cls::thisClass(), Cls::tableName(),
			selections, len);
	}

	template <typename Cls>
	bool createTable() {
		static Cls model;
		return this->createTable(&model);
	}

	template <typename Cls>
	bool dropTable() {
		return this->dropTable(Cls::tableName());
	}

	virtual int excute(cstring sql);

	virtual Query query(const Class* cls, cstring table,
		cstring selections[], size_t len);
	virtual void add(Model* model);
	virtual int update(Model* model);
	virtual int remove(Model* model);

	virtual bool createDatabase(cstring db);
	virtual bool dropDatabase(cstring db);

	virtual bool createTable(cstring table);
	virtual bool createTable(Model* model);
	virtual bool dropTable(cstring table);

	virtual void begin();
	virtual void commit();
	virtual void rollback();
protected:
	Connection* m_connection;
};

}//end of namespace brpc