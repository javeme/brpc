#ifndef LinkedList_H
#define LinkedList_H

#include "bluemeiLib.h"
#include "Iterator.h"
#include "CriticalLock.h"
#include "RuntimeException.h"

namespace bluemei{

template <class EleType>
struct Node : public Object
{
	EleType data;
	Node *next;
};

//Ñ­»·Á´±í
template <class EleType>
class LinkedList : public Object//BLUEMEILIB_API
{	
public:
	LinkedList();
	LinkedList(EleType data[],int length);

	LinkedList(const LinkedList<EleType>& other);
	LinkedList<EleType>& operator=(const LinkedList<EleType>& other);

	LinkedList(LinkedList<EleType>&& other);
	LinkedList<EleType>& operator=(LinkedList<EleType>&& other);

	virtual~LinkedList();
public:
	virtual bool addToLast(const EleType& value);
	virtual bool addToLast(EleType data[],int length);
	virtual bool removeFirstElement(EleType& value);
	virtual bool removeLastElement(EleType& value);
	virtual bool insert(int index,const EleType& value);
	virtual bool remove(int index,EleType& value);
	virtual int remove(const EleType& value);
	virtual bool clear();
	virtual bool get(int index,EleType& value) const;
	virtual EleType get(int index) const;
	virtual EleType operator[](int index);
	virtual bool getLastElement(EleType& value) const;
	virtual EleType getLastElement() const;
	virtual int getLocate(const EleType& value) const;
	virtual int size() const;
	virtual Iterator<EleType>* iterator(int index=0);
	virtual void releaseIterator(Iterator<EleType>* pIterator);
	virtual String toString() const;
protected:
	Node<EleType>* m_pHeader,*m_pRear;
	int m_nSize;
	CriticalLock m_lock;
};


template <class EleType>
class LinkedListIterator : public Iterator<EleType>
{
public:
	LinkedListIterator(Node<EleType>* pFirst){
		m_pFirst=nullptr;
		m_pCurrent=pFirst;
	}
	virtual~LinkedListIterator(){}
	virtual bool hasNext(){
		bool hasMore=( m_pCurrent!=nullptr && m_pCurrent!=m_pFirst);
		if(m_pFirst==nullptr)
			m_pFirst=m_pCurrent;
		return hasMore;
	}
	virtual EleType next(){
		m_pCurrent=m_pCurrent->next;
		if(m_pCurrent==nullptr)
		{
			throwpe(OutOfBoundException("there is no more element in LinkedList"));
		}
		return m_pCurrent->data;
	}
	virtual bool remove(){
		return false;
	}
private:
	Node<EleType>* m_pCurrent,*m_pFirst;
};

}//end of namespace bluemei
#endif 