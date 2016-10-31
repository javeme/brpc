#pragma once
#include "src/util/BRpcUtil.h"
#include "src/type/ObjectMap.h"
#include "src/util/RpcException.h"

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
		if(cls->hasField(fldName)) {
			obj->setAttribute(fldName, itor->second);
		}
		else {
			throwpe(BadCastException(String::format(
				"class '%s' without field '%s'",
				cls->getName(), fldName)));
		}
	}
	return obj;
}

Object* MapObjectConverter::object2map(Object* obj)
{
	checkNullPtr(obj);
	ScopePointer<ObjectMap> map = new ObjectMap();
	auto flds = obj->getThisClass()->allFields();
	for(unsigned int i = 0; i < flds.size(); i++)
	{
		const FieldInfo* fldInfo = flds[i];
		cstring name = fldInfo->name();
		map->put(name, obj->getAttribute(name));
	}
	return map.detach();
}

//Object* MapObjectConverter::map2object(cstring cls, Object* map)
Object* MapObjectConverter::map2object(Object* map, const Class* cls)
{
	checkNullPtr(map);
	checkNullPtr(cls);
	ObjectMap* objMap = dynamic_cast<ObjectMap*>(map);
	if(!objMap)
		throw NotMapException(map);

	//Object* obj = ObjectFactory::instance().createObject(clsName);
	ScopePointer<Object> obj = cls->createObject();
	brpc::map2object(objMap, obj, cls);
	return obj.detach();
}

Object* MapObjectConverter::map2object(Object* map, Object* obj)
{
	checkNullPtr(map);
	ObjectMap* objMap = dynamic_cast<ObjectMap*>(map);
	if(!objMap)
		throw NotMapException(map);

	return brpc::map2object(objMap, obj);
}

}//end of namespace brpc