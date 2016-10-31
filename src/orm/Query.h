#pragma once
#include "blib/ArrayList.h"
#include "blib/Pair.h"
#include "blib/Ptr.h"
#include "orm/Condition.h"
#include "orm/Actions.h"
#include "orm/Connection.h"


namespace brpc{

class Model;
class Column;
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
	virtual Query& query(const Column& field);

	virtual Query& filter(const ConditionWrapper& condition);
	virtual Query& filter(const Column& field);
	virtual Query& filterById(Model& model);

	virtual Query& groupBy(cstring field);
	virtual Query& groupBy(const Column& field);

	virtual Query& having(const ConditionWrapper& condition);

	virtual Query& orderBy(cstring field, bool desc=false);
	virtual Query& orderBy(const Column& field, bool desc=false);

	virtual Query& limit(unsigned int num);
	virtual Query& offset(unsigned int index);

	virtual Query& distinct(bool yes=false);

	//virtual Query& join(cstring table);

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
	OrderBy m_orderBy;  // order by
	unsigned int m_limit;  // limit
	unsigned int m_offset;  // offset

	Connection* m_connection;
};

}//end of namespace brpc