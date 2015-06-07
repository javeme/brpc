#pragma once
#include "stdafx.h"
#include "BRpcUtil.h"
#include "ObjectFactory.h"
#include "ObjectMap.h"

namespace bluemei{

bool BRpcUtil::s_isBrpcDebug = false;


Object* MapObjectConverter::object2map(Object* obj)
{
	checkNullPtr(obj);
	ObjectMap* map = new ObjectMap();
	auto flds = obj->getThisClass()->allFields();
	for(unsigned int i = 0; i < flds.size(); i++)
	{
		const FieldInfo* fldInfo = flds[i];
		cstring name = fldInfo->name();
		map->put(name, obj->getAttribute(name));
	}
	return map;
}

//Object* MapObjectConverter::map2object(cstring cls, Object* map)
Object* MapObjectConverter::map2object(Object* map, Object* obj)
{
	checkNullPtr(map);
	ObjectMap* objMap = dynamic_cast<ObjectMap*>(map);
	if(!objMap)
		throw NotMapException(map);
	//Object* obj = ObjectFactory::instance().createObject(cls);
	checkNullPtr(obj);
	for(auto itor = objMap->begin(); itor != objMap->end(); ++itor)
	{
		cstring fldName = itor->first.c_str();
		if(obj->getThisClass()->hasField(fldName))
		{
			obj->setAttribute(fldName, itor->second);
		}
	}
	return obj;
}


NotMapException::NotMapException(Object* obj)
{
	checkNullPtr(obj);
	this->setExceptionMsg(String::format("Object '%s' is not a map",
		obj->toString().c_str()));
}

String NotMapException::name() const
{
	return "NotMapException";
}

};
