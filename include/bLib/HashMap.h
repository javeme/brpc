#pragma once
#include "HashCoder.h"
#include "StringBuilder.h"
#include "Iterator.h"
#include "RuntimeException.h"

namespace bluemei{

template<class K,class V>
class BLUEMEILIB_TEMPLATE Entry : public Object
{
	template<class K,class V> friend class HashMap;
public:
	Entry(){ hash=0; next=nullptr; }
	Entry(const K& k,const V& v,unsigned int h=0,Entry<K,V>* next=nullptr){
		key=k;
		value=v;
		this->hashcode=h;
		this->next=next;
	}
	virtual ~Entry(){ delete next; }
public:
	virtual String toString()const{
		return Value2String<K>(key)+"="+Value2String<V>(value);
	}
	virtual Entry<K,V>* dettachNext(){
		Entry<K,V>* tmp=next;
		next=nullptr;
		return tmp;
	}
public:
	K key;
	V value;
	unsigned int hashcode;
protected:
	Entry<K,V>* next;
};

template<class K,class V>
class BLUEMEILIB_TEMPLATE IMap : public Object
{
public:
	virtual bool put(const K& k,const V& v)=0;
	virtual V* get(const K& key)const=0;
	virtual bool get(const K& key,V& v)const=0;
	virtual bool remove(const K& key,V& v)=0;
	virtual bool contain(const K& key)const=0;
	virtual unsigned int size() const=0;

	virtual RefPointer<Iterator<Entry<K,V>>> iterator()=0;
	virtual void releaseIterator(Iterator<Entry<K,V>>* itor)=0;

	virtual IMap& merge(const IMap& other, bool overwrite=true)
	{
		auto i=const_cast<IMap&>(other).iterator();
		while(i->hasNext()) {
			Entry<K,V> entry=i->next();
			if(!(this->contain(entry.key) && !overwrite))
				(void)put(entry.key, entry.value);
		}
		const_cast<IMap&>(other).releaseIterator(i);
		return *this;
	}
public:
	static unsigned int indexFor(unsigned int hash, unsigned length){
		return hash%length;
		//return hash & (length-1);
	}
};

template<class K,class V>
class BLUEMEILIB_TEMPLATE HashMap : public IMap<K,V>
{
	typedef Entry<K,V> HashEntry;
	typedef HashMap<K,V> MyType;
public:
	HashMap(unsigned int capacity=16,float loadFactor=0.84f):m_nMaxSize(capacity),m_entryTable(m_nMaxSize,1)
	{
		m_nSize=0;
		if(loadFactor<0.4)
			loadFactor=0.4f;
		else if(loadFactor>1)
			loadFactor=1;
		m_fLoadFactor=loadFactor;
		m_nThreshold=(unsigned int)(m_nMaxSize*m_fLoadFactor);
		initListNull(m_entryTable);
	}

	virtual ~HashMap()
	{
		clear();
	}
public:
	HashMap(const HashMap& other)
	{
		this->operator=(other);
	}
	HashMap& operator=(const HashMap& other)
	{
		if(this!=&other)
		{
			this->clear();
			/*this->m_nSize=other.m_nSize;
			this->m_fLoadFactor=other.m_fLoadFactor;
			this->m_nThreshold=other.m_nThreshold;
			this->m_nMaxSize=other.m_nMaxSize;

			this->m_entryTable=ArrayList<HashEntry*>(this->m_nMaxSize,1.0);
			initListNull(this->m_entryTable);
			other.transfer(this->m_entryTable);*/
			this->m_nSize=0;
			this->m_fLoadFactor=other.m_fLoadFactor;
			this->m_nThreshold=other.m_nThreshold;
			this->m_nMaxSize=other.m_nMaxSize;

			this->m_entryTable=ArrayList<HashEntry*>(this->m_nMaxSize,1.0);
			initListNull(this->m_entryTable);
			this->merge(other);
		}
		return *this;
	}
	HashMap(HashMap&& other)
	{
		*this=std::move(other);
	}
	HashMap& operator=(HashMap&& other)
	{
		if(this!=&other)
		{
			this->m_nSize=other.m_nSize;
			this->m_fLoadFactor=other.m_fLoadFactor;
			this->m_nThreshold=other.m_nThreshold;
			this->m_nMaxSize=other.m_nMaxSize;
			this->m_entryTable=std::move(other.m_entryTable);

			other.m_nMaxSize=16;
			other.m_fLoadFactor=0.84f;
			other.m_nSize=0;
			other.m_nThreshold=(unsigned int)(other.m_nMaxSize*other.m_fLoadFactor);
			other.m_entryTable=ArrayList<HashEntry*>(other.m_nMaxSize,1);
			initListNull(other.m_entryTable);
		}
		return *this;
	}

	const V& operator[](const K& key)const
	{
		V* v=get(key);
		if (v == null)
			throwpe(NotFoundException("key " + (String)Value2String<K>(key)));
		return *v;
	}
	V& operator[](const K& key)
	{
		V* v=get(key);
		if (v == null) {
			if(put(key, V()) && (v=get(key)))
				return *v;
		}
		else
			return *v;
		throwpe(NotFoundException("key " + (String)Value2String<K>(key)));
	}
public:
	virtual bool put(const K& k,const V& v);
	virtual V* get(const K& key)const;
	virtual bool get(const K& k,V& v)const;
	virtual V getDefault(const K& key,const V& defaultVal)const;
	virtual bool remove(const K& key,V& v);
	virtual void clear();
	virtual bool contain(const K& key)const;
	virtual unsigned int size() const;
	virtual String toString()const;

	virtual RefPointer<Iterator<Entry<K,V>>> iterator();
protected:
	virtual void releaseIterator(Iterator<HashEntry>* itor);

	void addEntry(const K& k,const V& v,unsigned int h, unsigned int index);
	void resize(unsigned int newCapacity);
	void transfer(ArrayList<HashEntry*>& newTable)const;
protected:
	void initListNull(ArrayList<HashEntry*>& list);
protected:
	unsigned int m_nSize;
	float m_fLoadFactor;
	unsigned int m_nThreshold;
	unsigned int m_nMaxSize;
	ArrayList<HashEntry*> m_entryTable;

public:
	//迭代器
	class BLUEMEILIB_TEMPLATE HashMapIterator : public Iterator<HashEntry>
	{
	protected:
		MyType& m_hashMapRef;
	public:
		HashMapIterator(MyType& hashMap):m_hashMapRef(hashMap){
			currentIndex=-1;
			currentEntry=nullptr;
		}
		virtual ~HashMapIterator(){
			m_hashMapRef.releaseIterator(this);
		}
		virtual bool hasNext(){
			unsigned int index=currentIndex;
			HashEntry* entry=currentEntry;
			return findNext(index, entry);
		}
		virtual HashEntry next(){
			if(!findNext(currentIndex, currentEntry))
				throwpe(OutOfBoundException("there is no more element in HashMap"));

			return HashEntry(currentEntry->key,currentEntry->value);
		}
		virtual bool remove();
	protected:
		bool findNext(unsigned int& index, HashEntry*& entry);
	private:
		unsigned int currentIndex;
		HashEntry* currentEntry;
	};
public:
	friend HashMapIterator;
};


template<class K,class V>
void HashMap<K, V>::addEntry(const K& k,const V& v,unsigned int h,
	unsigned int index)
{
	HashEntry* next=m_entryTable[index];
	m_entryTable[index]=new HashEntry(k,v,h,next);
	m_nSize++;
	if (m_nSize >= m_nThreshold)
		resize(2*m_nMaxSize);//未考虑溢出
}

template<class K,class V>
void HashMap<K, V>::resize(unsigned int newCapacity)
{
	ArrayList<HashEntry*> newTable(newCapacity,1.0);
	initListNull(newTable);
	transfer(newTable);
	m_entryTable=move(newTable);
	m_nMaxSize=newCapacity;
	m_nThreshold=(unsigned int)(m_nMaxSize * m_fLoadFactor);
}

template<class K,class V>
void HashMap<K, V>::transfer(ArrayList<HashEntry*>& newTable)const
{
	const ArrayList<HashEntry*>& src=m_entryTable;
	unsigned int newCapacity=newTable.size();
	unsigned int oldSize=src.size();
	for (unsigned int j=0; j < oldSize; j++) {
		HashEntry* entry=src[j];
		if (entry != null) {
			//src[j]=null;
			do {
				//保存src节点链的下一个节点
				HashEntry* next=entry->next;
				//将entry搭链到新表中去
				unsigned int i=indexFor(entry->hashcode, newCapacity);
				entry->next=newTable[i];
				newTable[i]=entry;
				//下一个src节点链的节点
				entry=next;
			} while (entry != null);
		}
	}
}

template<class K,class V>
void HashMap<K, V>::initListNull(ArrayList<HashEntry*>& list)
{
	for (unsigned int i=0; i < list.size(); i++)
	{
		list[i]=null;
	}
}

template<class K,class V>
bool HashMap<K, V>::put(const K& k,const V& v)
{
	if(m_nSize>=m_nMaxSize)
		return false;
	unsigned int h=hashCode<K>(k);
	unsigned int index=indexFor(h,m_nMaxSize);

	for(HashEntry* entry=m_entryTable[index]; entry!=nullptr; entry=entry->next)
	{
		if(k==entry->key){
			entry->value=v;
			return true;
		}
	}
	addEntry(k,v,h,index);
	return true;
}

template<class K,class V>
V* HashMap<K, V>::get(const K& key) const
{
	unsigned int index=indexFor(hashCode<K>(key),m_nMaxSize);
	for(HashEntry* entry=m_entryTable[index]; entry!=nullptr; entry=entry->next)
	{
		if(key==entry->key){
			return &entry->value;
		}
	}
	return nullptr;
}

template<class K,class V>
bool HashMap<K, V>::get(const K& k,V& v) const
{
	const V* pValue=get(k);
	if(pValue==nullptr){
		//throwpe(Exception("not found"));
		return false;
	}
	v=*pValue;
	return true;
}

template<class K,class V>
V HashMap<K, V>::getDefault(const K& key,const V& defaultVal) const
{
	const V* pValue=get(key);
	if(pValue==nullptr)
		return defaultVal;
	else
		return *pValue;
}

template<class K,class V>
bool HashMap<K, V>::remove(const K& key,V& v)
{
	unsigned int index=indexFor(hashCode<K>(key),m_nMaxSize);
	HashEntry* previous=nullptr;
	for(HashEntry* entry=m_entryTable[index]; entry!=nullptr; previous=entry,entry=entry->next)
	{
		if(key==entry->key){
			v=entry->value;
			if (previous==nullptr)
				m_entryTable[index]=entry->dettachNext();
			else
				previous->next=entry->dettachNext();
			delete entry;
			m_nSize--;

			return true;
		}
	}
	return false;
}

template<class K,class V>
void HashMap<K, V>::clear()
{
	//释放table
	for(unsigned int i=0;i<m_entryTable.size();i++)
	{
		if(m_entryTable[i])
		{
			delete m_entryTable[i];
			m_entryTable[i]=nullptr;
		}
	}
	//m_entryTable.clear();
	m_nSize=0;
}

template<class K,class V>
String HashMap<K, V>::toString() const
{
	StringBuilder sb(32*m_entryTable.size());
	sb.append("HashMap size=").append((int)size());
	sb.append("{");
	for(unsigned int i=0;i<m_entryTable.size();i++)
	{
		for(HashEntry* entry=m_entryTable[i]; entry!=nullptr; entry=entry->next)
		{
			sb.append(entry->toString()).append(",");
		}
	}
	int last=sb.lastIndexOf(",");
	if(last>0)
		sb.deleteCharAt(last);
	sb.append('}');
	return sb.toString();
}

template<class K,class V>
RefPointer<Iterator<Entry<K,V>>> HashMap<K, V>::iterator()
{
	return new HashMap::HashMapIterator(*this);
}

template<class K,class V>
void HashMap<K, V>::releaseIterator(Iterator<Entry<K,V>>* itor)
{
	//delete itor;
}

template<class K,class V>
bool HashMap<K, V>::contain(const K& key)const
{
	return get(key)!=nullptr;
}

template<class K,class V>
unsigned int HashMap<K, V>::size() const
{
	return m_nSize;
}

///////////////////////////////////////////////////////////////////
//class HashMap::HashMapIterator
template<class K, class V>
bool HashMap<K, V>::HashMapIterator::remove()
{
	if(currentEntry==nullptr || currentIndex<0)
	{
		//throwpe(NullPointerException("current entry is null"));
		return false;
	}
	else
	{
		HashEntry* toDelete=currentEntry;
		HashEntry*& first=m_hashMapRef.m_entryTable[currentIndex];
		if(first==nullptr){
			throwpe(NullPointerException("first entry is null"));
		}
		else if(first==currentEntry){//当前元素为首个元素
			first=currentEntry->dettachNext();
			currentIndex--;
			currentEntry=nullptr;
		}
		else{
			HashEntry* prev=first;
			while(prev->next!=currentEntry)
			{
				prev=prev->next;
			}
			if(prev==nullptr)
				throwpe(NullPointerException("previous entry is null"));
			prev->next=currentEntry->dettachNext();
			currentEntry=prev;
		}

		delete toDelete;
		m_hashMapRef.m_nSize--;
		return true;
	}
}

template<class K, class V>
bool HashMap<K, V>::HashMapIterator::findNext(unsigned int& index,
	Entry<K, V>*& entry)
{
	unsigned int size=m_hashMapRef.m_entryTable.size();
	while(index+1<=size)
	{
		//next ele in table
		if(entry==nullptr)
		{
			index++;
			if(index<size){
				entry=m_hashMapRef.m_entryTable[index];
				if(entry!=nullptr){
					return true;
				}
			}
		}
		//next ele in entry
		if(entry!=nullptr)
		{
			if(entry->next!=nullptr){
				entry=entry->next;
				return true;
			}
			else{
				entry=nullptr;
			}
		}
	}

	return false;
}


template class BLUEMEILIB_API HashMap<String,int>;
template class BLUEMEILIB_API HashMap<String,Object*>;

}//end of namespace bluemei