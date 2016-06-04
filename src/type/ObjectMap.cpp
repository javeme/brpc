#pragma once
#include "stdafx.h"
#include "ObjectMap.h"

namespace brpc{


Object* ObjectMap::remove(cstring name, bool del)
{
	Object* obj = BeanContainer::remove(name);

	if(del && obj)
	{
		delete obj;
		return null;
	}

	return obj;
}

String ObjectMap::toString() const
{
	StringBuilder sb(size() * 20);
	sb.append('{');
	for (auto itor = begin(); itor != end(); ++itor)
	{
		const std::string& name = itor->first;
		Object* obj = itor->second;

		sb.append(name.c_str()).append(':');

		if (obj == null)
		{
			sb.append("null").append(',');
		}
		else
		{
			sb.append(obj->toString()).append(',');
		}
	}
	if (sb.length() > 0 && sb.charAt(sb.length()-1) == ',')
	{
		sb.deleteCharAt(sb.length()-1);
	}
	sb.append('}');

	return sb.toString();
}

}//end of namespace brpc