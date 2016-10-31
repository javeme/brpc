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
		Object* fldValue = itor->second;
		if(cls->hasField(fldName)) {
			// is the fldValue a ObjectMap?
			ObjectMap* subMap = dynamic_cast<ObjectMap*>(fldValue);
			if(subMap != null) {
				Object* attr = obj->getAttribute(fldName);
				if(attr != null) {
					fldValue = map2object(subMap, attr);
				}
			}
			// set value into `obj`
			obj->setAttribute(fldName, fldValue);
		}
		else {
			throwpe(BadCastException(String::format(
				"class '%s' without field '%s'",
				cls->getName(), fldName)));
		}
	}
	return obj;
}

static bool isTypedObject(Object* obj)
{
	// return true if obj is one of:
	//  [Number*, TString*, ObjectMap*, ObjectList*]
	return dynamic_cast<TypeVisitable*>(obj) != null;
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
		Object* val = obj->getAttribute(name);
		// cast obj to map if it's not a Typed Object
		if(!isTypedObject(val)) { // common object
			val = MapObjectConverter::object2map(val);
		}
		map->put(name, val);
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