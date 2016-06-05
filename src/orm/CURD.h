#pragma once
#include "blib.h"
#include "Condition.h"


namespace brpc{


class CURD : public SqlExpression
{
public:
	CURD(cstring table) : m_tableName(table) {}

	cstring table() const { return m_tableName; }
protected:
	String m_tableName;  // from table
};


class Inserter : public CURD
{
public:
	Inserter(cstring table, const HashMap<String, String>& pairs)
		: CURD(table), m_pairs(pairs) {
	}
public:
	virtual String toSQL() const {
		if(m_pairs.size() == 0)
			throw SqlException("No fields to insert");

		StringBuilder keys(16 * m_pairs.size());
		StringBuilder values(16 * m_pairs.size());
		auto iter = const_cast<HashMap<String, String>&>(m_pairs).iterator();
		while (iter->hasNext())
		{
			auto pair = iter->next();

			keys.append("`");
			keys.append(pair.key);
			keys.append("`,");

			values.append(pair.value);
			values.append(",");
		}
		const_cast<HashMap<String, String>&>(m_pairs).releaseIterator(iter);

		if (keys.length() > 0)
			keys.deleteCharAt(keys.length() - 1);// remove ','
		if (values.length() > 0)
			values.deleteCharAt(values.length() - 1);// remove ','

		/*return String::format("insert into %s(%s) (%s)",
			table(),
			keys.toString().c_str(),
			values.toString().c_str());
		*/

		StringBuilder sql(32 + keys.length() + values.length());
		// insert into
		sql.append("insert into ");
		sql.append(table());		
		// keys
		sql.append(" (");
		sql.append(keys.toString());
		sql.append(")");
		// values
		sql.append(" values(");
		sql.append(values.toString());
		sql.append(")");

		return sql.toString();
	}
protected:
	HashMap<String, String> m_pairs;
};

class Updater : public CURD
{
public:
	Updater(cstring table, const HashMap<String, String>& pairs,
		const ConditionWrapper& condition=CW_NONE)
		: CURD(table), m_pairs(pairs), m_condition(condition) {
	}
public:
	virtual String toSQL() const {
		if(m_pairs.size() == 0)
			throw SqlException("No fields to update");
		StringBuilder setters(32 * m_pairs.size());
		auto iter = const_cast<HashMap<String, String>&>(m_pairs).iterator();
		while (iter->hasNext())
		{
			auto pair = iter->next();

			setters.append("`");
			setters.append(pair.key);
			setters.append("`=");
			setters.append(pair.value);
			setters.append(",");
		}
		const_cast<HashMap<String, String>&>(m_pairs).releaseIterator(iter);

		if (setters.length() > 0)
			setters.deleteCharAt(setters.length() - 1);// remove ','

		String condition = m_condition->toSQL();

		StringBuilder sql(64 + condition.length() + setters.length());
		// update table
		sql.append("update ");
		sql.append(table());
		// set age=18
		sql.append(" set ");
		sql.append(setters.toString());
		// where
		sql.append(" where ");
		sql.append(condition);

		return sql.toString();
	}
protected:
	HashMap<String, String> m_pairs;
	ConditionWrapper m_condition; 
};

class Deleter : public CURD
{
public:
	Deleter(cstring table, const ConditionWrapper& condition=CW_NONE)
		: CURD(table), m_condition(condition) {}
public:
	virtual String toSQL() const {
		String condition = m_condition->toSQL();

		StringBuilder sql(64 + condition.length());
		// delete from table
		sql.append("delete from ");
		sql.append(table());
		// where
		if (!m_condition.equals(CW_NONE)) {
			sql.append(" where ");
			sql.append(condition);
		}

		return sql.toString();
	}
protected:
	ConditionWrapper m_condition;
};

}//end of namespace brpc