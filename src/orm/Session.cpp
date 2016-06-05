#pragma once
#include "stdafx.h"
#include "Session.h"

namespace brpc{


/////////////////////////////////////////////////////////////////
//Model
HashMap<String, String> Model::updates() const
{
	HashMap<String, String> updates;
	auto flds = this->getThisClass()->allFields();
	for (unsigned int i = 0; i < flds.size(); i++)
	{
		const FieldInfo* fldInfo = flds[i];
		cstring name = fldInfo->name();
		const void* attr = this->getAttributeAddr(name);
		// convert into any of TypeColume<?>*
		FieldBase* value = dynamic_cast<FieldBase*>((TypeColume<int>*)attr);
		bool updated = value->setted() && value->modified();
		if(updated) {
			// TODO: how to convert it into a string
			Colume* col = dynamic_cast<Colume*>(value);
			updates.put(col->columeName(), col->columeValue());
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
		FieldBase* value = (FieldBase*)(TypeColume<int>*)attr;
		bool updated = value->setted() && value->modified();
		if(updated) {
			value->setModified(false);
		}
	}
}


/////////////////////////////////////////////////////////////////
//database Session
void Session::add(Model* model)
{
	checkNullPtr(model);
	m_connection->excute(Inserter(model->getTableName(), model->updates()));
	model->clean();
}

int Session::update(Model* model)
{
	checkNullPtr(model);
	HashMap<String, String> kvs = model->updates();
	Colume& id = model->id();
	kvs.remove(id.columeName(), String());
	ConditionWrapper cond = (id.query() == new ValueCondition(id.columeValue()));
	int ret = m_connection->excute(Updater(model->getTableName(), kvs, cond));
	// TODO: refresh the model?
	model->clean();
	return ret;
}

int Session::remove(Model* model)
{
	checkNullPtr(model);
	Colume& id = model->id();
	ConditionWrapper cond = (id.query() == new ValueCondition(id.columeValue()));
	int ret = m_connection->excute(Deleter(model->getTableName(), cond));
	model->clean();
	return ret;
}


}//end of namespace brpc