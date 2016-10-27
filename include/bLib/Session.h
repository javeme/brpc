#pragma once
#include "BeanContainer.h"

namespace bluemei{

class BLUEMEILIB_API Session : public BeanContainer
{
public:
	Session(void);
	/*Session(Session&& other){
		__super::operator=(other);
	};*/
	virtual ~Session(void);
protected:
	Session(const Session& other){*this=other;};
	Session& operator=(const Session& other){
		__super::operator=(other);
		return *this;
	};
};

}//end of namespace bluemei