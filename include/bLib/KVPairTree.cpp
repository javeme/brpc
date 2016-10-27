#ifndef KVPairTree_CPP
#define KVPairTree_CPP

#include "KVPairTree.h"

namespace bluemei{

template<typename Key,typename Value>
KVPairTree<Key,Value>::KVPairTree()
{
	clear();
}

template<typename Key,typename Value>
KVPairTree<Key,Value>::~KVPairTree()
{

}

template<typename Key,typename Value>
void KVPairTree<Key,Value>::setSinglePair(const Key& k,const Value& v)
{
	m_bSingleValue=true;
	m_singleValue.key=k;
	m_singleValue.value=v;
}

template<typename Key,typename Value>
KVPairTree<Key,Value>& KVPairTree<Key,Value>::item(const Key& k)
{
	for(unsigned int i=0; i<m_valueList.size(); i++)
	{
		if(k==m_valueList[i].name())
			return m_valueList[i];
	}
	String key=Value2String<Key>(k);
	String s=String::format("KVPairTree item(): key '%s' not found",
		key.c_str());
	throwpe(Exception(s));
}

template<typename Key,typename Value>
KVPairTree<Key,Value> KVPairTree<Key, Value>::removeItem(unsigned int index)
{
	checkBound(index);
	KVPairTree<Key,Value> prop=m_valueList[index];
	m_valueList.erase(m_valueList.begin()+index);
	return prop;
}

template<typename Key,typename Value>
KVPairTree<Key,Value> KVPairTree<Key, Value>::removeItem(const Key& k)
{
	for(auto iter = m_valueList.begin(); iter != m_valueList.end();)
	{
		auto& item=*iter;
		if(k==item.name())
		{
			KVPairTree<Key,Value> prop=item;
			iter=m_valueList.erase(iter);
			return prop;
		}
		else
			++iter;
	}
	String key=Value2String<Key>(k);
	String s=String::format("KVPairTree removeItem(): key '%s' not found",
		key.c_str());
	throwpe(Exception(s));
}

template<typename Key,typename Value>
bool KVPairTree<Key, Value>::contain( const Key& k ) const
{
	for(unsigned int i=0; i<m_valueList.size(); i++)
	{
		if(k==m_valueList[i].name())
			return true;
	}
	return false;
}

}//end of namespace bluemei
#endif