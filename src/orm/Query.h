#pragma once
#include "blib.h"
#include "Condition.h"
#include "Actions.h"
#include "Connection.h"


namespace brpc{

class Model;
class Colume;
typedef  ArrayList<Pair<String, bool>> OrderBy;


/////////////////////////////////////////////////////////////////
//QueryResult
class _QueryResult : public ArrayList<Model*>, public PointerReference
{
	friend class QueryResult;
public:
	virtual ~_QueryResult();

	virtual String toString() const {
		return ArrayList<Model*>::toString();
	}
private:
	_QueryResult() {}
};

class QueryResult : public RefPointer<_QueryResult>
{
public:
	static QueryResult create() {
		return QueryResult(new _QueryResult());
	}
protected:
	QueryResult(_QueryResult* rs) : RefPointer(rs) {}
};


/////////////////////////////////////////////////////////////////
//Query
class Query : public CURD
{
public:
	Query(Connection* conn,
		const Class* tableClass,
		const String& table,
		const ArrayList<String>& fileds,
		const ConditionWrapper& where=CW_NONE,
		const ArrayList<String>& groupBy=ArrayList<String>(),
		const ConditionWrapper& having=CW_NONE,
		const OrderBy& orderBy=OrderBy(),
		unsigned int limit=0,
		unsigned int offset=0,
		bool distinct=false);

	virtual Query& query(cstring field);
	virtual Query& query(const Colume& field);

	virtual Query& filter(const ConditionWrapper& condition);
	virtual Query& filter(const Colume& field);
	virtual Query& filterById(Model& model);

	virtual Query& groupBy(cstring field);
	virtual Query& groupBy(const Colume& field);

	virtual Query& having(const ConditionWrapper& condition);

	virtual Query& orderBy(cstring field, bool desc=false);
	virtual Query& orderBy(const Colume& field, bool desc=false);

	virtual Query& limit(unsigned int num);
	virtual Query& offset(unsigned int index);

	virtual Query& distinct(bool yes=false);
	
	virtual String toSQL() const;
public:
	Connection* getConnection() const { return m_connection; }
	void setConnection(Connection* val) { m_connection = val; }

	virtual QueryResult all();
	//virtual Model* one();
	//virtual Model* first();
private:
	const Class* m_tableClass;  // the class mapping the table
	//String m_tableName;  // from table
	bool m_distinct; // distinct
	ArrayList<String> m_expectedFileds;  // select *
	ConditionWrapper m_where;  // where
	ArrayList<String> m_groupBy;  // group by
	ConditionWrapper m_having;  // having
	ArrayList<Pair<String, bool>> m_orderBy;  // order by
	unsigned int m_limit;  // limit
	unsigned int m_offset;  // offset

	Connection* m_connection;
};


}//end of namespace brpc