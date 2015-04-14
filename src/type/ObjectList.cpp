#pragma once
#include "stdafx.h"
#include "ObjectList.h"

namespace bluemei{

bool ObjectList::remove(unsigned int index, bool del)
{
	Object* obj = null;
	bool success = Vector::remove(index, obj);
	
	if(success && del)
		delete obj;

	return success;
}

void ObjectList::destroy()
{
	for (unsigned int i=0; i<size(); i++)
	{
		Object* obj = (*this)[i];
		delete obj;
		//itor = this->erase(itor);
	}
	this->clear();
}


Object* ObjectList::operator[](unsigned int index) const
{
	return (*const_cast<ObjectList*>(this)).Vector::operator[](index);
}

String ObjectList::toString() const
{
	StringBuilder sb(size() * 10);
	sb.append('[');
	for (unsigned int i=0; i<size(); i++)
	{
		Object* obj = (*this)[i];
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
	sb.append(']');

	return sb.toString();
}

void ObjectList::accept(TypeVisiter* visitor)
{
	checkNullPtr(visitor);
	visitor->visit(this);
}

void ObjectList::visitElements(TypeVisiter* visitor)
{
	checkNullPtr(visitor);
	for (unsigned int i=0; i<size(); i++)
	{
		Object* obj = (*this)[i];
		ObjectList::Iterator itor(obj);
		itor.accept(visitor);
	}
}

}//end of namespace bluemei