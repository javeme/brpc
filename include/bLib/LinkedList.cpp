#ifndef LinkedList_CPP
#define LinkedList_CPP

#include "LinkedList.h"
#include "StringBuilder.h"

namespace bluemei{

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
	Node<EleType> *pNode=m_pHeader;//要删除节点的上一个节点
	Node<EleType> *pToDelete;
	if(index>0)
	{
		index-=1;
		for(int i=0;i<index;i++)
		{
			pNode=pNode->next;
		}
		if(index==0)
			m_pRear=m_pHeader;
	}
	else
	{
		if(m_pRear==m_pHeader)
		{
			m_pRear=m_pHeader=nullptr;
		}
		else
		{
			pNode=m_pRear;
			m_pHeader=(m_pRear->next)->next;
		}
	}
	pToDelete=pNode->next;//pNode为要删除节点的前趋
	value=move(pToDelete->data);
	pNode->next=pToDelete->next;
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
	Node<EleType> *pNode=m_pRear;
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
				pNode->next=nullptr;
			else
				pNode->next=pToDelete->next;
			if(pToDelete==m_pRear)
				m_pRear=pNode;
			if(pToDelete==m_pHeader)
				m_pHeader=pNode->next;
			delete pToDelete;
			m_nSize--;
			break;
		}
		pNode=pToDelete;
		pToDelete=pToDelete->next;
	}
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
Iterator<EleType>* LinkedList<EleType>::iterator(int index/*=0*/)
{
	m_lock.getLock();
	Node<EleType> *pNode=m_pRear;
	if(index<0)
		index=0;
	if(index>=m_nSize)
		return new LinkedListIterator<EleType>(nullptr);
	for(int i=0;i<index;i++)
	{
		pNode=pNode->next;
	}
	return new LinkedListIterator<EleType>(pNode);
}

template <class EleType>
void LinkedList<EleType>::releaseIterator(Iterator<EleType>* pIterator)
{
	delete pIterator;
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


}//end of namespace bluemei
#endif