#pragma once
#include "stdafx.h"
#include "BRpcUtil.h"
#include "ObjectMap.h"
#include "RpcException.h"


namespace brpc{

bool BRpcUtil::s_isBrpcDebug = false;

static Object* map2object(ObjectMap* objMap, Object* obj, const Class* cls=null)
{
	checkNullPtr(objMap);
	checkNullPtr(obj);

	if(cls == null)
		cls = obj->getThisClass();

	for(auto itor = objMap->begin(); itor != objMap->end(); ++itor)
	{
		cstring fldName = itor->first.c_str();
		if(cls->hasField(fldName))
		{
			try {
				obj->setAttribute(fldName, itor->second);
			} catch (...) {
				delete obj;
				throw;
			}
		}
	}
	return obj;
}

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
Object* MapObjectConverter::map2object(Object* map, const Class* cls)
{
	checkNullPtr(map);
	checkNullPtr(cls);
	ObjectMap* objMap = dynamic_cast<ObjectMap*>(map);
	if(!objMap)
		throw NotMapException(map);

	//Object* obj = ObjectFactory::instance().createObject(cls);
	Object* obj = cls->createObject();
	return brpc::map2object(objMap, obj, cls);
}

Object* MapObjectConverter::map2object(Object* map, Object* obj)
{
	checkNullPtr(map);
	ObjectMap* objMap = dynamic_cast<ObjectMap*>(map);
	if(!objMap)
		throw NotMapException(map);

	return brpc::map2object(objMap, obj);
}

}
