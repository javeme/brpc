#ifndef Observer_H_H
#define Observer_H_H
#include "Object.h"
#include "Exception.h"
#include "LinkedList.cpp"

namespace bluemei{

//观察者（Observer）角色
template <class MessageType>
class Observer : public Object
{
public:
	virtual void onNotifyException(Exception& e, Object* pFrom)=0;
	virtual void onNotify(MessageType* pMessage, Object* pFrom)=0;
};

//主题（Subject）角色
template <class MessageType>
class Observable : public Object
{
public:
	Observable(){
		this->changed=false;
	}
	virtual ~Observable(){
		unregisterAllObserver();
	}
public:
	virtual void registerObserver(Observer<MessageType>* pObserver){
		if(obs.getLocate(pObserver)<0)
			obs.addToLast(pObserver);
	}
	virtual void unregisterObserver(Observer<MessageType>* pObserver){
		obs.remove(pObserver);
	}
	virtual void unregisterAllObserver()
	{
		obs.clear();
	}

	virtual void notifyObservers(Exception& e, Object* pFrom)
	{
		if(!changed) 
			return;
		clearChanged();

		for(int i=0; i<obs.size(); i++)
			obs.get(i)->onNotifyException(e,pFrom);
	}
	virtual void notifyObservers(MessageType* pMessage, Object* pFrom)
	{
		if(!changed) 
			return;
		clearChanged();

		for(int i=0; i<obs.size(); i++)
			obs.get(i)->onNotify(pMessage,pFrom);
	}

	virtual void setChanged()
	{
		changed = true;
	}
	virtual void clearChanged()
	{
		changed = false;
	}
	virtual bool hasChanged()
	{
		return changed;
	}

	virtual int countObservers()
	{
		return obs.size();
	}
protected:
	LinkedList<Observer<MessageType>*> obs;
	bool changed;
};

}//end of namespace bluemei
#endif