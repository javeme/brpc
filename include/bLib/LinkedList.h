#ifndef LinkedList_H
#define LinkedList_H

#include "bluemeiLib.h"
#include "Iterator.h"
#include "CriticalLock.h"
#include "RuntimeException.h"
#include "StringBuilder.h"

namespace blib{

template <class EleType>
class LinkedListIterator;

template <class EleType>
struct Node : public Object
{
	EleType data;
	Node *next;
};

//单向循环链表
template <class EleType>
class LinkedList : public Object//BLUEMEILIB_API
{
public:
	friend LinkedListIterator<EleType>;
public:
	LinkedList();
	LinkedList(EleType data[],int length);

	LinkedList(const LinkedList<EleType>& other);
	LinkedList<EleType>& operator=(const LinkedList<EleType>& other);

	LinkedList(LinkedList<EleType>&& other);
	LinkedList<EleType>& operator=(LinkedList<EleType>&& other);

	virtual ~LinkedList();
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
	virtual RefPointer<Iterator<EleType>> iterator(int index=0);
	virtual String toString() const;
protected:
	virtual void releaseIterator(Iterator<EleType>* pIterator);
protected:
	Node<EleType> *m_pHeader, *m_pRear;
	int m_nSize;
	CriticalLock m_lock;
};


template <class EleType>
class LinkedListIterator : public Iterator<EleType>
{
public:
	LinkedListIterator(LinkedList<EleType>* pRef,
		Node<EleType>* pFirst=nullptr){
		checkNullPtr(pRef);
		if(pFirst==nullptr)
			pFirst=pRef->m_pHeader;
		m_pLinkedListRef=pRef;
		m_pHead=pFirst;

		m_pPrepare=new Node<EleType>();
		m_pPrepare->next=m_pHead;

		m_pCurrent=m_pPrepare;
	}
	virtual ~LinkedListIterator(){
		m_pLinkedListRef->releaseIterator(this);
		delete m_pPrepare;
	}
	virtual bool hasNext(){
		bool hasMore=(m_pCurrent!=nullptr && m_pCurrent->next!=m_pHead);
		if(m_pCurrent==m_pPrepare)
			hasMore=(m_pHead!=nullptr);
		return hasMore;
	}
	virtual EleType next(){
		checkNullPtr(m_pCurrent);
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
	LinkedList<EleType>* m_pLinkedListRef;
	Node<EleType> *m_pCurrent, *m_pHead, *m_pPrepare;
};


template <class EleType>
LinkedList<EleType>::LinkedList()
{
	m_nSize=0;
	m_pHeader=m_pRear=NULL;
}

template <class EleType>
LinkedList<EleType>::LinkedList(EleType data[],int length)
{
	m_nSize=0;
	m_pHeader=m_pRear=NULL;
	addToLast(data,length);
}

template <class EleType>
LinkedList<EleType>::LinkedList(const LinkedList<EleType>& other)
{
	this->operator=(other);
}
template <class EleType>
LinkedList<EleType>& LinkedList<EleType>::operator=(const LinkedList<EleType>& other)
{
	if(this==&other)
		return *this;

	m_lock.getLock();
	((LinkedList<EleType>*)&other)->m_lock.getLock();

	clear();
	//复制数据,包括指针内容
	m_nSize=other.m_nSize;
	if(m_nSize>0){
		Node<EleType> *pNode=nullptr,*pLastNode=nullptr;
		Node<EleType> *pOtherNode=other.m_pHeader;
		for(int i=0;i<m_nSize;i++)
		{
			pNode=new Node<EleType>(*pOtherNode);
			pNode->next=nullptr;
			if(pLastNode!=nullptr)
				pLastNode->next=pNode;
			else
				m_pHeader=pNode;
			pLastNode=pNode;
			pOtherNode=pOtherNode->next;
		}
		m_pRear=pNode;
		m_pRear->next=m_pHeader;
	}

	((LinkedList<EleType>*)&other)->m_lock.releaseLock();
	m_lock.releaseLock();

	return *this;
}

template <class EleType>
LinkedList<EleType>::LinkedList(LinkedList<EleType>&& other)
{
	this->operator=(move(other));
}

template <class EleType>
LinkedList<EleType>& LinkedList<EleType>::operator=(LinkedList<EleType>&& other)
{
	//窃取资源, 千万不要窃取m_lock, 其属于每个对象本身的资源
	m_lock.getLock();
	other.m_lock.getLock();

	if (this!=&other)
	{
		this->clear();
		m_pHeader=other.m_pHeader;
		m_pRear=other.m_pRear;
		m_nSize=other.m_nSize;

		other.m_pHeader=nullptr;
		other.m_pRear=nullptr;
		other.m_nSize=0;
	}

	other.m_lock.releaseLock();
	m_lock.releaseLock();
	return *this;
}

template <class EleType>
LinkedList<EleType>::~LinkedList()
{
	clear();
}

template <class EleType>
bool LinkedList<EleType>::addToLast(const EleType& value)
{
	m_lock.getLock();
	bool isSuccess=false;
	if(m_nSize==0)
	{
		isSuccess=insert(0,value);
	}
	else{
		Node<EleType> *pNode=new Node<EleType>;
		if(pNode!=NULL)
		{
			pNode->data=value;
			pNode->next=m_pHeader;
			//TRACE("add to last:%p\n",pNode);

			m_pRear->next=pNode;
			m_pRear=pNode;
			m_nSize++;
			isSuccess=true;
		}
	}
	m_lock.releaseLock();
	return isSuccess;
}

template <class EleType>
bool LinkedList<EleType>::addToLast(EleType data[],int length)
{
	bool isSuccess=false;
	m_lock.getLock();
	Node<EleType> *pNode=NULL;
	if(data!=NULL && length>0)
	{
		int start=0;
		if(m_nSize==0)
		{
			isSuccess=insert(0,data[0]);
			start=1;
		}
		if(isSuccess){
			for(int i=start;i<length;i++)
			{
				pNode=new Node<EleType>;
				pNode->data=data[i];
				pNode->next=m_pHeader;

				m_pRear->next=pNode;
				m_pRear=pNode;

				m_nSize++;
			}
		}
	}
	m_lock.releaseLock();
	return isSuccess;
}

template <class EleType>
bool LinkedList<EleType>::removeLastElement(EleType& value)
{
	return remove(m_nSize-1,value);
}

template <class EleType>
bool LinkedList<EleType>::removeFirstElement(EleType& value)
{
	return remove(0,value);
}

template <class EleType>
bool LinkedList<EleType>::insert(int index,const EleType& value)
{
	if(index<0 || index>m_nSize)
		return false;
	m_lock.getLock();
	Node<EleType> *pNode=m_pRear;
	Node<EleType> *pNewNode=new Node<EleType>;
	pNewNode->data=value;
	if(m_nSize>0)
	{
		for(int i=0;i<index;i++)
		{
			pNode=pNode->next;
		}
		pNewNode->next=pNode->next;//pNode为要插入节点位置节点的前趋
		pNode->next=pNewNode;

		if(index==0)//头部
		{
			//此时pNode=m_pRear=m_pHeader
			m_pHeader=pNewNode;
		}
		else if(index==m_nSize)//尾部
		{
			m_pRear=pNewNode;
		}
	}
	else//m_nSize=0
	{
		if(m_nSize==0)
		{
			pNewNode->next=pNewNode;
			m_pRear=m_pHeader=pNewNode;
		}
	}
	m_nSize++;

	m_lock.releaseLock();
	return true;
}

template <class EleType>
bool LinkedList<EleType>::remove(int index,EleType& value)
{
	if(index<0 || index>=m_nSize)
		return false;
	m_lock.getLock();
	if(m_lock.getMyThreadEnteredCount()>1){//如果迭代器还没有释放,则不能移除
		m_lock.releaseLock();
		throwpe(Exception("don't move the list when other place using it"))
	}
	Node<EleType> *pPrev=m_pHeader;//要删除节点的上一个节点
	Node<EleType> *pToDelete;
	if(index>0)
	{
		index-=1;
		for(int i=0;i<index;i++)
		{
			pPrev=pPrev->next;
		}
		if(index==0)
			m_pRear=m_pHeader;
	}
	else // index 0
	{
		if(m_pRear==m_pHeader) // size is 0
		{
			m_pRear=m_pHeader=nullptr;
		}
		else 
		{
			pPrev=m_pRear;
			m_pHeader=(m_pRear->next)->next;
		}
	}
	pToDelete=pPrev->next;//pNode为要删除节点的前趋
	value=move(pToDelete->data);
	pPrev->next=pToDelete->next;
	delete pToDelete;
	pToDelete=NULL;
	m_nSize--;
	m_lock.releaseLock();
	return true;
}

template <class EleType>
int LinkedList<EleType>::remove(const EleType& value)
{
	m_lock.getLock();
	if(m_lock.getMyThreadEnteredCount()>1){//如果迭代器还没有释放,则不能移除
		m_lock.releaseLock();
		throwpe(Exception("don't move the list when other place using it"))
	}
	int index=-1;
	Node<EleType> *pPrev=m_pRear;
	Node<EleType> *pToDelete=m_pHeader;
	/*
	Node<EleType> *pTmp=m_pHeader;
	for(int i=0;i<m_nSize;i++)
	{
		TRACE("%p->",pTmp);
		pTmp=pTmp->next;
	}
	TRACE("size=%d\n",m_nSize);*/

	for(int i=0;i<m_nSize;i++)
	{
		if(pToDelete->data==value)
		{
			index=i;
			if(m_nSize==1)
			{
				// if it's removing the last one, update the head and rear only
				m_pRear=m_pHeader=nullptr;
			}
			else
			{
				// update the link
				pPrev->next=pToDelete->next;
				// update m_pRear or m_pHeader
				if(pToDelete==m_pRear)
					m_pRear=pPrev;
				if(pToDelete==m_pHeader)
					m_pHeader=m_pHeader->next;
			}
			// delete the node
			delete pToDelete;
			m_nSize--;
			break;
		}
		pPrev=pToDelete;
		pToDelete=pToDelete->next;
	} // end of for
	/*
	pTmp=m_pHeader;
	for(int i=0;i<m_nSize;i++)
	{
		TRACE("%p->",pTmp);
		pTmp=pTmp->next;
	}
	TRACE("size=%d\n\n",m_nSize);*/

	m_lock.releaseLock();
	return index;
}

template <class EleType>
bool LinkedList<EleType>::clear()
{
	m_lock.getLock();
	Node<EleType> *pNode=NULL;
	for(int i=0;i<m_nSize;i++)
	{
		pNode=m_pHeader;
		m_pHeader=m_pHeader->next;
		delete pNode;
	}
	m_nSize=0;
	m_pHeader=m_pRear=NULL;
	m_lock.releaseLock();
	return true;
}

template <class EleType>
bool LinkedList<EleType>::get(int index,EleType& value) const
{
	if(index>=0 && index<m_nSize)
	{
		Node<EleType> *pNode=m_pHeader;
		for(int i=0;i<index;i++)
		{
			pNode=pNode->next;
		}
		value=pNode->data;
		return true;
	}
	else
	{
		return false;
	}
}

template <class EleType>
EleType LinkedList<EleType>::get(int index) const
{
	EleType v;
	get(index,v);
	return v;
}

template <class EleType>
EleType LinkedList<EleType>::operator[](int index)
{
	return get(index);
}

template <class EleType>
bool LinkedList<EleType>::getLastElement(EleType& value) const
{
	return get(m_nSize-1,value);
}

template <class EleType>
EleType LinkedList<EleType>::getLastElement() const
{
	EleType v;
	getLastElement(v);
	return v;
}

template <class EleType>
int LinkedList<EleType>::getLocate(const EleType& value) const
{
	Node<EleType> *pNode=m_pHeader;
	for(int i=0;i<m_nSize;i++)
	{
		if(pNode->data==value)
			return i;
		pNode=pNode->next;
	}
	return -1;
}

template <class EleType>
int LinkedList<EleType>::size() const
{
	return m_nSize;
}


template <class EleType>
RefPointer<Iterator<EleType>> LinkedList<EleType>::iterator(int index/*=0*/)
{
	m_lock.getLock();
	Node<EleType> *pNode=m_pRear;
	if(index<=0)
		index=0;
	else if(index>=m_nSize)
		throwpe(OutOfBoundException(index,m_nSize));
	for(int i=0;i<index;i++)
	{
		pNode=pNode->next;
	}
	
	return new LinkedListIterator<EleType>(this, pNode);
}

template <class EleType>
void LinkedList<EleType>::releaseIterator(Iterator<EleType>* pIterator)
{
	//delete pIterator;
	m_lock.releaseLock();
}

template <class EleType>
String LinkedList<EleType>::toString() const
{
	StringBuilder sb(32*size());
	sb.append("LinkedList size=").append((int)size());
	sb.append("{");
	auto i=((LinkedList<EleType>*)this)->iterator();
	while (i->hasNext())
	{
		sb.append(Value2String<EleType>(i->next())).append(',');
	}
	((LinkedList<EleType>*)this)->releaseIterator(i);
	int last=sb.lastIndexOf(",");
	if(last>0)
		sb.deleteCharAt(last);
	sb.append('}');
	return sb.toString();
}

}//end of namespace blib
#endif