#pragma once
#include "HashMap.h"
#include "LinkedList.h"

namespace bluemei{

//func map
template<class K,class V>
class BLUEMEILIB_TEMPLATE MultiValueHashMap : public Object
{
public:
	MultiValueHashMap(unsigned int capacity=16,float loadFactor=0.84f):m_hashMap(capacity,loadFactor){}
	virtual~MultiValueHashMap(){}

public:
	virtual bool put(const K& k,const V& v);
	virtual bool get(const K& key,LinkedList<V>& list)const;
	virtual bool remove(const K& key,V& v);
	virtual void clear();
	virtual bool contain(const K& key);
	virtual unsigned int size() const;
	virtual String toString()const;

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

/*
#include "MultiValueHashMap.h"
template<> Value2String<Entry<String,LinkedList<String>>*>::operator String()const{
	if(value==nullptr)
		return "<null>";
	return value->toString();
}template class Value2String<Entry<String,LinkedList<String>>*>;

void testMVMap()
{
	MultiValueHashMap<String,String> mvMap;
	mvMap.put("lizhmei","18931861012");
	mvMap.put("lizhmei","15810301283");
	mvMap.put("yikong","010-12345678");
	mvMap.put("yikong","010-22345678");
	mvMap.put("yikong","010-22345678");
	mvMap.put("yikong","010-32345678");
	mvMap.put("yikong","010-42345678");
	mvMap.put("yikong","010-52345678");
	mvMap.put("yikong","010-62345678");
	mvMap.put("yikong","010-72345678");
	mvMap.put("yikong","010-82345678");
	mvMap.put("yikong","010-92345678");
	mvMap.put("yikong","010-02345678");
	mvMap.put("hj","13870551634");
	mvMap.put("hj","8420340");
	for(int i=0;i<100;i++)
	{
		String str=String::format("%c%c%c%c",rand()%128,rand()%128,rand()%128,rand()%128);
		mvMap.put(str,String()+i+i+i+i);
	}
	String s=mvMap.toString();
	int pos=sizeof(mvMap)-sizeof(ArrayList<LinkedList<String>>);
	s=((ArrayList<Entry<String,LinkedList<String>>*>*)(((char*)&mvMap)+pos))->toString();
	auto nulllist=s.splitWith("<null>");

	ArrayList<int> l1;
	l1.add(1);
	ArrayList<int> l2=move(l1);
	for(int i=0;i<100;i++)
		l1.add(i);
	s=l1.toString();
}*/

}//end of namespace bluemei