#pragma once
#include "stdafx.h"
#include "Query.h"
#include "Session.h"

namespace brpc{


Query Query::query(cstring field)
{
	if (m_expectedFileds.size() == 1 && m_expectedFileds[0].trim() == "*") {
		ArrayList<String> fields;
		fields.add(field);
		return Query(table(), fields, m_condition, m_connection);
	}
	else {
		ArrayList<String> fields = m_expectedFileds;
		fields.add(field);
		return Query(table(), fields, m_condition, m_connection);
	}
}

brpc::Query Query::query(Colume& field)
{
	return query(field.columeName());
}

Query Query::filter(const ConditionWrapper& condition)
{
	if (m_condition.equals(CW_NONE)) {
		return Query(table(), m_expectedFileds, condition, m_connection);
	}
	else {
		return Query(table(), m_expectedFileds,
			m_condition && condition, m_connection);
	}
}

Query Query::filter(Colume& field)
{
	return filter(field.query() == new ValueCondition(field.columeValue()));
}

Query Query::filterById(Model& model)
{
	return filter(model.id().query() == new ValueCondition(model.id().columeValue()));
}

String Query::toSQL() const
{
	if (m_expectedFileds.size() == 0)
		throw SqlException("No fields to query");
	bool selectAll = (m_expectedFileds[0].trim() == "*");

	String condition = m_condition->toSQL();
	StringBuilder sql(32 + condition.length());

	//String::format("select %s from %s where %s", ...);
	// select
	sql.append("select ");
	if (!selectAll)
		sql.append("`");
	sql.append(String("`,`").join(m_expectedFileds));
	if (!selectAll)
		sql.append("`");
	sql.append(" from ");
	sql.append(table());
	// where
	if (!m_condition.equals(CW_NONE)) {
		sql.append(" where ");
		sql.append(condition);
	}
	// TODO: add group-by...
	return sql.toString();
}

ArrayList<Model*> Query::all()
{
	checkNullPtr(m_connection);
	ArrayList<Model*> rs;
	String result = m_connection->query(this->toSQL());
	// TODO parse result
	return rs;
}

}//end of namespace brpc