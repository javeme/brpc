#pragma once
#include "HashMap.h"
#include "LinkedList.h"

namespace blib{

//func map
template<class K,class V>
class BLUEMEILIB_TEMPLATE MultiValueHashMap : public Object
{
public:
	MultiValueHashMap(unsigned int capacity=16,float loadFactor=0.84f)
		: m_hashMap(capacity,loadFactor) {}
	virtual~MultiValueHashMap(){}

public:
	virtual bool put(const K& k,const V& v);
	virtual bool get(const K& key,LinkedList<V>& list) const;
	virtual bool remove(const K& key,V& v);
	virtual void clear();
	virtual bool contain(const K& key);
	virtual unsigned int size() const;
	virtual String toString() const;

public:
	typedef Entry<K,LinkedList<V>> MVEntry;
	virtual RefPointer<Iterator<MVEntry>> iterator(){
		return m_hashMap.iterator();
	}

protected:
	HashMap<K,LinkedList<V>> m_hashMap;
};

template<class K,class V>
bool MultiValueHashMap<K, V>::put( const K& k,const V& v )
{
	LinkedList<V>* list=m_hashMap.get(k);
	if(list==null)
	{
		LinkedList<V> l;
		l.addToLast(v);
		return m_hashMap.put(k,l);
	}
	else
	{
		list->addToLast(v);
		return true;
	}
}

template<class K,class V>
bool MultiValueHashMap<K, V>::get( const K& key,LinkedList<V>& list ) const
{
	return m_hashMap.get(key,list);
}

template<class K,class V>
bool MultiValueHashMap<K, V>::remove( const K& key,V& v )
{
	LinkedList<V>* list=m_hashMap.get(key);
	if(list==null)
		return false;
	return list->remove(v)>=0;
}

template<class K,class V>
void MultiValueHashMap<K, V>::clear()
{
	m_hashMap.clear();
}

template<class K,class V>
bool MultiValueHashMap<K, V>::contain( const K& key )
{
	return m_hashMap.contain(key);
}

template<class K,class V>
unsigned int MultiValueHashMap<K, V>::size() const
{
	return m_hashMap.size();
}

template<class K,class V>
String MultiValueHashMap<K, V>::toString() const
{
	StringBuilder sb(100*m_hashMap.size());
	sb.append("MultiValueHashMap size=").append((int)size());
	sb.append("{");
	auto i=const_cast<MultiValueHashMap<K, V>*>(this)->iterator();
	while(i->hasNext())
	{
		auto entry=i->next();
		String v=entry.value.toString();
		v=v.substring(v.find("{"),v.length());
		sb.append(Value2String<K>(entry.key)).append('=').append(v).append(',');
	}
	int last=sb.lastIndexOf(",");
	if(last>0)
		sb.deleteCharAt(last);
	sb.append('}');
	return sb.toString();
}

template class BLUEMEILIB_API MultiValueHashMap<String,int>;
template class BLUEMEILIB_API MultiValueHashMap<String,Object*>;

}//end of namespace blib