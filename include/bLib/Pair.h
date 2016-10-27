#pragma once
#include "bluemeiLib.h"
#include "Object.h"

namespace bluemei{

template<typename K,typename V>
class Pair : public Object//BLUEMEILIB_API
{
public:
	K key;
	V value;
public:
	Pair(const K& key=NULL,const V& value=NULL){
		this->key=key;
		this->value=value;
	};
	bool operator==(const Pair<K,V>& pair) const{
		if(this->key==pair.key && this->value==pair.value)
			return true;
		else
			return false;
	}
};

}//end of namespace bluemei