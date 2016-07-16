#pragma once
#include "stdafx.h"
#include "Session.h"

namespace brpc{


/////////////////////////////////////////////////////////////////
//Model
FiledValues Model::updates(bool mustUpdated) const
{
	FiledValues updates;
	auto flds = this->getThisClass()->allFields();
	for (unsigned int i = 0; i < flds.size(); i++)
	{
		const FieldInfo* fldInfo = flds[i];
		cstring name = fldInfo->name();
		const void* attr = this->getAttributeAddr(name);
		// convert into any of TypeColume<?>* 
		// TODO: how to deal with that the attr is not a TypeColume?
		FieldBase* value = dynamic_cast<FieldBase*>((TypeColume<int>*)attr);
		bool updated = value->setted() && value->modified();
		if((mustUpdated && updated) || !mustUpdated) {
			Colume* col = dynamic_cast<Colume*>(value);
			checkNullPtr(col);
			updates.put(col->field(), col);
		}
	}
	return updates;
}

void Model::clean() const
{
	auto flds = this->getThisClass()->allFields();
	for (unsigned int i = 0; i < flds.size(); i++)
	{
		const FieldInfo* fldInfo = flds[i];
		cstring name = fldInfo->name();
		const void* attr = this->getAttributeAddr(name);
		// convert into any of TypeColume<?>* 
		// TODO: how to deal with that the attr is not a TypeColume?
		FieldBase* value = dynamic_cast<FieldBase*>((TypeColume<int>*)attr);
		bool updated = value->setted() && value->modified();
		if(updated) {
			value->setModified(false);
		}
	}
}

String Model::getTableName() const
{
	return String(thisClass()->getName()).toLower();
}

void Model::fromDatabase(ResultSet* result)
{
	auto flds = this->getThisClass()->allFields();
	for (unsigned int i = 0; i < flds.size(); i++)
	{
		const FieldInfo* fldInfo = flds[i];
		cstring name = fldInfo->name();

		const void* attr = this->getAttributeAddr(name);
		// convert into any of TypeColume<?>* 
		// TODO: how to deal with that the attr is not a TypeColume?
		FieldBase* value = dynamic_cast<FieldBase*>((TypeColume<int>*)attr);
		Colume* col = dynamic_cast<Colume*>(value);
		checkNullPtr(col);
		String colname = col->columeName();

		if (result->columnIndex(colname) != -1)
		{
			//BRpcUtil::debug(">>>> %s=%s\n", colname, result->getString(colname));
			ScopePointer<Object> obj = result->getObject(colname);
			this->setAttribute(name, obj);
		}
		else if (result->columnIndex(name) != -1)
		{
			//BRpcUtil::debug(">>>> %s=%s\n", name, result->getString(name));
			ScopePointer<Object> obj = result->getObject(name);
			this->setAttribute(name, obj);
		}
	}
	this->clean();
}

QueryResult Model::fromDatabase(ResultSet* result, const Class* cls)
{
	checkNullPtr(result);
	checkNullPtr(cls);

	QueryResult rs = QueryResult::create();
	while (result->next()) {
		Object* obj = cls->createObject();
		Model* model = dynamic_cast<Model*>(obj);
		if (obj != null && model == null) {
			delete obj;
			throwpe(SQLException(String::format(
				"The class %s is not a subclass of %s",
				cls->getName(), thisClass()->getName())));
		}
		checkNullPtr(model);
		rs->add(model);
		model->fromDatabase(result);
	}
	return rs;
}


/////////////////////////////////////////////////////////////////
//database Session
int Session::excute(cstring sql)
{
	int ret = m_connection->excute(sql);
	return ret;
}

Query Session::query(const Class* cls, cstring table,
	cstring selections[], size_t len)
{
	ArrayList<String> fields;
	for (size_t i=0; i<len; i++) {
		fields.add(selections[i]);
	}
	if (fields.size() == 0) {
		fields.add("*");
	}
	return Query(m_connection, cls, table, fields);
}

void Session::add(Model* model)
{
	checkNullPtr(model);
	m_connection->excute(Inserter(model->getTableName(), model->updates()));
	model->clean();
}

int Session::update(Model* model)
{
	checkNullPtr(model);
	FiledValues kvs = model->updates();
	Colume& id = model->id();
	Colume* col = null;
	kvs.remove(id.field(), col);
	ConditionWrapper cond = (id.query() == id.columeValue());
	int ret = m_connection->excute(Updater(model->getTableName(), kvs, cond));
	// TODO: refresh the model?
	model->clean();
	return ret;
}

int Session::remove(Model* model)
{
	checkNullPtr(model);
	Colume& id = model->id();
	ConditionWrapper cond = (id.query() == id.columeValue());
	int ret = m_connection->excute(Deleter(model->getTableName(), cond));
	model->clean();
	return ret;
}

bool Session::createDatabase(cstring db)
{
	int ret = m_connection->excute(DatabaseCreater(db));
	return ret == 1; // changed rows number is 1
}

bool Session::dropDatabase(cstring db)
{
	int ret = m_connection->excute(DatabaseDroper(db));
	return ret == 0; // why 0?
}

bool Session::createTable(cstring table)
{
	int ret = m_connection->excute(TableCreater(table));
	return ret == 0;
}

bool Session::createTable(Model* model)
{
	checkNullPtr(model);
	StringBuilder sql(256);

	// table name
	sql.append(model->getTableName());
	sql.append("(");

	// columes
	FiledValues cols = model->updates(false);
	auto iter = cols.iterator();
	while (iter->hasNext())
	{
		auto colume = iter->next().value;

		if (!colume->isNeedStore())
			continue;
		// field name
		sql.append(colume->columeName());
		// field type
		sql.append(" ");
		sql.append(colume->columeType());
		// primary key
		if (colume->isPrimary()) {
			sql.append(" primary key");
		}
		// not null
		if (colume->isNotNull()) {
			sql.append(" not null");
		}
		// default 
		if (!colume->defaultValue().empty()) {
			sql.append(" default ");
			sql.append(colume->defaultValue());
		}
		// TODO: unique key / forgin key ...
		sql.append(",");
	}
	if (cols.size() > 0)
		sql.pop();// remove ','

	cols.releaseIterator(iter);

	sql.append(")");
	return this->createTable(sql.toString());
}

bool Session::dropTable(cstring table)
{
	int ret = m_connection->excute(TableDroper(table));
	return ret == 0;
}

void Session::begin()
{
	m_connection->excute(TransactionBegin());
}

void Session::commit()
{
	m_connection->excute(TransactionCommit());
}

void Session::rollback()
{
	m_connection->excute(TransactionRollback());
}


}//end of namespace brpc