#pragma once
#include "stdafx.h"
#include "Query.h"
#include "Session.h"

namespace brpc{


_QueryResult::~_QueryResult()
{
	for(unsigned int i = size()-1; i < size(); i--) {
		Model* toDelete = null;
		this->removeAt(i, toDelete);
		delete toDelete;
	}
}


Query::Query(Connection* conn,
	const Class* tableClass,
	const String& table,
	const ArrayList<String>& fileds,
	const ConditionWrapper& where/*=CW_NONE*/,	
	const ArrayList<String>& groupBy,
	const ConditionWrapper& having/*=CW_NONE*/,
	const OrderBy& orderBy,
	unsigned int limit,
	unsigned int offset,
	bool distinct)
	: CURD(table), m_connection(conn), m_tableClass(tableClass),
	m_expectedFileds(fileds), m_where(where),
	m_groupBy(groupBy), m_having(having), m_orderBy(orderBy),
	m_limit(limit), m_offset(offset), m_distinct(distinct)
{
	;
}

Query& Query::query(cstring field)
{
	if (m_expectedFileds.size() == 1 && m_expectedFileds[0].trim() == "*") {
		m_expectedFileds[0] = field;
	}
	else {
		m_expectedFileds.add(field);
	}
	return *this;
}

Query& Query::query(const Column& field)
{
	return this->query(field.query()->toSQL());
}

Query& Query::filter(const ConditionWrapper& condition)
{
	if (m_where.equals(CW_NONE)) {
		m_where = condition;
	}
	else {
		m_where = m_where && condition;
	}
	return *this;
}

Query& Query::filter(const Column& field)
{
	return this->filter(field.query() == field.columnValue());
}

Query& Query::filterById(Model& model)
{
	return this->filter(model.id().query() == model.id().columnValue());
}

Query& Query::groupBy(cstring field)
{
	this->m_groupBy.add(field);
	return *this;
}

Query& Query::groupBy(const Column& field)
{
	return this->groupBy(field.query()->toSQL());
}

Query& Query::having(const ConditionWrapper& condition)
{
	this->m_having = condition;
	return *this;
}

Query& Query::orderBy(cstring field, bool desc/*=false*/)
{
	this->m_orderBy.add(Pair<String, bool>(field, desc));
	return *this;
}

Query& Query::orderBy(const Column& field, bool desc/*=false*/)
{
	return this->orderBy(field.query()->toSQL(), desc);
}

Query& Query::limit(unsigned int num)
{
	this->m_limit = num;
	return *this;
}

Query& Query::offset(unsigned int index)
{
	this->m_offset = index;
	return *this;
}

Query& Query::distinct(bool yes/*=false*/)
{
	this->m_distinct = yes;
	return *this;
}

String Query::toSQL() const
{
	if (m_expectedFileds.size() == 0)
		throw SQLException("No fields to query");
	bool selectAll = (m_expectedFileds[0].trim() == "*");

	String where = m_where->toSQL();
	String having = m_having->toSQL();
	StringBuilder sql(128 + where.length() + having.length());

	//String::format("select %s from %s where %s", ...);
	// select *
	sql.append("select ");
	if (m_distinct)
		sql.append("distinct ");
	sql.append(String(",").join(m_expectedFileds));
	// from table
	sql.append(" from ");
	sql.append(table());
	// where
	if (!m_where.equals(CW_NONE)) {
		sql.append(" where ");
		sql.append(where);
	}
	// group-by
	if (m_groupBy.size() > 0) {
		sql.append(" group by ");
		sql.append(String(",").join(m_groupBy));
	}
	// having
	if (!m_having.equals(CW_NONE)) {
		sql.append(" having ");
		sql.append(having);
	}
	// order by
	for (size_t i=0; i<m_orderBy.size(); i++) {
		sql.append(" order by ");
		sql.append(m_orderBy[i].key);
		sql.append(m_orderBy[i].value ? " desc " : "");
	}

	// limit
	if (m_limit > 0) {
		// TODO: add more generic way('limit' is just in mysql/pg)
		sql.append(" limit ");
		sql.append((int)m_limit);

	}
	// offset
	if (m_offset > 0) {
		sql.append(" offset ");
		sql.append((int)m_offset);

	}
	return sql.toString();
}

QueryResult Query::all()
{
	checkNullPtr(m_connection);
	// TODO: should we use session->query()?
	ScopePointer<ResultSet> result = m_connection->query(this->toSQL());
	// parse result
	return Model::fromDatabase(result, m_tableClass);
}


}//end of namespace brpc