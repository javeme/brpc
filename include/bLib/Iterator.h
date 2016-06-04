#pragma once
#include "bluemeiLib.h"

namespace bluemei{

template <class T>
class Iterator
{
public:
	virtual~Iterator(){}
	virtual bool hasNext()=0;
	virtual T next()=0;
	virtual bool remove()=0;
};

}//end of namespace bluemei