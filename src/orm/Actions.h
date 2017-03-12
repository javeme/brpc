#pragma once
#include "blib/BString.h"
#include "orm/Condition.h"
#include "orm/Column.h"


namespace brpc{

class Action : public SQLExpression
{
public:
	Action(cstring action) : m_action(action) {}

	cstring action() const { return m_action; }

	virtual String toSQL() const {
		return m_action;
	}

public:
	Action(Action&& other) {
		*this = std::move(other);
	}

	Action& operator=(Action&& other) {
		SQLExpression::operator=(std::move(other));
		this->m_action = std::move(other.m_action);
		return *this;
	}

private:
	explicit Action(const Action&);
	Action&  operator=(const Action&);

private:
	String m_action;  // from table
};


//////////////////////////////////////////////////////////////
//Transaction
class TransactionBegin : public Action
{
public:
	TransactionBegin() : Action("begin;") {}
};

class TransactionCommit : public Action
{
public:
	TransactionCommit() : Action("commit;") {}
};

class TransactionRollback : public Action
{
public:
	TransactionRollback() : Action("rollback;") {}
};


//////////////////////////////////////////////////////////////
//Database/Table
class DatabaseCreater : public Action
{
public:
	DatabaseCreater(cstring db) : Action(String("create database ") + db) {}
};

class DatabaseDroper : public Action
{
public:
	DatabaseDroper(cstring db) : Action(String("drop database ") + db) {}
};

class TableCreater : public Action
{
public:
	TableCreater(cstring table)
		: Action(String("create table if not exists ") + table) {}
};

class TableDroper : public Action
{
public:
	TableDroper(cstring table)
		: Action(String("drop table if exists ") + table) {}
};


//////////////////////////////////////////////////////////////
//CURD
class CURD : public Action
{
public:
	CURD(cstring table) : Action("CURD"), m_tableName(table) {}

	CURD(CURD&& other) : Action("CURD") {
		*this = std::move(other);
	}

	CURD& operator=(CURD&& other) {
		Action::operator=(std::move(other));
		this->m_tableName = std::move(other.m_tableName);
		return *this;
	}

private:
	explicit CURD(const CURD&);
	CURD&  operator=(const CURD&);

public:
	cstring table() const { return m_tableName; }
protected:
	String m_tableName;  // from table
};


typedef HashMap<String, Column*> FiledValues;


class Inserter : public CURD
{
public:
	Inserter(cstring table, const FiledValues& pairs)
		: CURD(table), m_pairs(pairs) {
	}
public:
	virtual String toSQL() const {
		if(m_pairs.size() == 0)
			throw SQLException("No fields to insert");

		StringBuilder keys(16 * m_pairs.size());
		StringBuilder values(16 * m_pairs.size());
		auto iter = const_cast<FiledValues&>(m_pairs).iterator();
		while (iter->hasNext())
		{
			auto column = iter->next().value;

			keys.append(column->query()->toSQL());
			keys.append(",");

			values.append(column->columnValue()->toSQL());
			values.append(",");
		}

		if (keys.length() > 0)
			keys.pop();// remove ','
		if (values.length() > 0)
			values.pop();// remove ','

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
	FiledValues m_pairs;
};

class Updater : public CURD
{
public:
	Updater(cstring table, const FiledValues& pairs,
		const ConditionWrapper& condition=CW_NONE)
		: CURD(table), m_pairs(pairs), m_condition(condition) {
	}
public:
	virtual String toSQL() const {
		if(m_pairs.size() == 0)
			throw SQLException("No fields to update");
		StringBuilder setters(32 * m_pairs.size());
		auto iter = const_cast<FiledValues&>(m_pairs).iterator();
		while (iter->hasNext())
		{
			auto column = iter->next().value;

			setters.append(column->query()->toSQL());
			setters.append("=");
			setters.append(column->columnValue()->toSQL());
			setters.append(",");
		}

		if (setters.length() > 0)
			setters.pop();// remove ','

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
	FiledValues m_pairs;
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