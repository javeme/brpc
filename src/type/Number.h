#pragma once
#include "blib.h"
#include "TypeVisitor.h"


namespace brpc{
	
typedef __int64 lint;

enum NumberPrecedence{
	PRED_LOWEST,
	PRED_BOOL,
	PRED_BYTE,
	PRED_SHORT,
	PRED_INT,
	PRED_INT64,
	PRED_FLOAT,
	PRED_DOUBLE,
	PRED_HIGHEST
};

class Number;
class Integer;
class Long;
class Float;
class Double;

class NumberVisiter : public bluemei::Object
{
public:
	NumberVisiter(){
		result=nullptr;
	}
	virtual ~NumberVisiter(){
		destroyResult();
	}
public:											
	virtual Number* getResult() const { return result; }
	virtual Number* detachResult();
	virtual void destroyResult();
protected:										
	Number* result;	

public:
	virtual bool visitInteger(Integer* v,Number* other)=0;
	virtual bool visitLong(Long* v, Number* other)=0;
	virtual bool visitFloat(Float* v,Number* other)=0;	
	virtual bool visitDouble(Double* v, Number* other)=0;

};


class Number : public bluemei::Object, public TypeVisitable
{
public:
	Number(){}
	virtual ~Number(){}
	virtual Number& operator=(const Number& other){ return *this; };
public:
	virtual int precedence()const=0;

	virtual int toInt()const=0;
	virtual lint toLong()const=0;
	virtual float toFloat()const=0;
	virtual double toDouble()const=0;

	virtual Number* toThisType(const Number* v)const=0;
	virtual bool isSameType(const Number& other) const{
		return precedence() == other.precedence();
	}
	
	virtual bool accept(NumberVisiter* visitor,Number* other)=0;

public:
	virtual void accept(TypeVisiter* visitor){ return visitor->visit(this); }
};

template<class T>
class TNumber : public Number
{
public:
	TNumber(T v=0):number(v){}
	virtual ~TNumber(){}
public:
	TNumber(const Number& other){ *this=other; }
	TNumber& setValue(const T& v){ number=v; return *this; }
	operator T()const{ return number; }
public:
	virtual int toInt() const
	{
		return (int)number;
	}

	virtual lint toLong() const
	{
		return (lint)number;
	}

	virtual double toDouble() const
	{
		return (double)number;
	}

	virtual float toFloat() const
	{
		return (float)number;
	}
	
	virtual String toString()const
	{
		return Value2String<T>(number);
	}
protected:
	T number;
};

/*template<> 
class TNumber<int>
{
public:
	TNumber<int>& operator=(const Number& other){
		number=other.toInt();
		return *this;
	}
	virtual int precedence() const
	{
		return PRED_INT;
	}
};
typedef TNumber<int> Integer;*/
class Integer : public TNumber<int>
{
public:
	DECLARE_DCLASS(Integer);
public:
	explicit Integer(int v=0):TNumber(v){}
	Integer& operator=(const Number& other){
		number=other.toInt();
		return *this;
	}

	virtual int precedence() const
	{
		return PRED_INT;
	}

	virtual Number* toThisType(const Number* v) const
	{
		return new Integer(v->toInt());
	}

	virtual bool accept(NumberVisiter* visitor,Number* other)
	{
		return visitor->visitInteger(this,other);
	}
};

class Char : public Integer
{
public:
	DECLARE_DCLASS(Char);
public:
	Char(char v=0):Integer(v){}
	Char& operator=(const Number& other){
		number=other.toInt();
		return *this;
	}

	virtual int precedence() const
	{
		return PRED_BYTE;
	}

	virtual Number* toThisType(const Number* v) const
	{
		return new Char((char)v->toInt());
	}

	virtual String toString() const
	{
		return String::format("%c", (char)*this);
	}

	operator char()const{ return (char)number; }
};

class Boolean : public Integer
{
public:
	DECLARE_DCLASS(Boolean);
public:
	Boolean(bool v=false):Integer(v){}
	Boolean& operator=(const Number& other){
		number=other.toInt();
		return *this;
	}

	virtual int precedence() const
	{
		return PRED_BOOL;
	}

	virtual Number* toThisType(const Number* v) const
	{
		//return new Boolean((bool)v->toInt());
		return new Boolean(v->toInt() != 0);
	}

	operator bool()const{ return number != 0; }

	virtual String toString()const
	{
		return Value2String<bool>(number != 0);
	}
};

class Short : public Integer
{
public:
	DECLARE_DCLASS(Short);
public:
	Short(short v=0):Integer(v){}
	Short& operator=(const Number& other){
		number=other.toInt();
		return *this;
	}

	virtual int precedence() const
	{
		return PRED_SHORT;
	}

	virtual Number* toThisType(const Number* v) const
	{
		return new Short((short)v->toInt());
	}

	operator short()const{ return (short)number; }
};

class Long : public TNumber<lint>
{
public:
	DECLARE_DCLASS(Long);
public:
	Long(lint v=0):TNumber(v){}
	Long& operator=(const Number& other){
		number=other.toInt();
		return *this;
	}

	virtual int precedence() const
	{
		return PRED_INT64;
	}

	virtual Number* toThisType(const Number* v) const
	{
		return new Long(v->toInt());
	}

	virtual bool accept(NumberVisiter* visitor,Number* other)
	{
		return visitor->visitLong(this,other);
	}
};

class Float : public TNumber<float>
{
public:
	DECLARE_DCLASS(Float);
public:
	Float(float v=0):TNumber(v){}
	Float& operator=(const Number& other){
		number=other.toFloat();
		return *this;
	}

	virtual int precedence() const
	{
		return PRED_FLOAT;
	}

	virtual Number* toThisType(const Number* v) const
	{
		return new Float(v->toFloat());
	}
	
	virtual bool accept(NumberVisiter* visitor,Number* other)
	{
		return visitor->visitFloat(this,other);
	}
};

class Double : public TNumber<double>
{
public:
	DECLARE_DCLASS(Double);
public:
	Double(double v=0):TNumber(v){}
	Double& operator=(const Number& other){
		number=other.toInt();
		return *this;
	}

	virtual int precedence() const
	{
		return PRED_DOUBLE;
	}

	virtual Number* toThisType(const Number* v) const
	{
		return new Double(v->toInt());
	}

	virtual bool accept(NumberVisiter* visitor,Number* other)
	{
		return visitor->visitDouble(this,other);
	}
};

inline Number* NumberVisiter::detachResult() 
{ 					
	Number* tmp=result; 					
	result=nullptr; 						
	return tmp; 							
}

inline void NumberVisiter::destroyResult()
{
	delete detachResult();
}

//保留浮点结果的除法
class NumberDivForFloatVisiter : public NumberVisiter
{
public:
	NumberDivForFloatVisiter(){}
	virtual ~NumberDivForFloatVisiter(){}
public:
	virtual bool visitInteger(Integer* v,Number* other) 
	{
		delete result;
		result=new Double((int)*v / other->toDouble());
		return true;
	}

	virtual bool visitLong(Long* v, Number* other) 
	{
		delete result;
		result=new Double((lint)*v / other->toDouble());
		return true;
	}

	virtual bool visitFloat(Float* v,Number* other) 
	{
		delete result;
		result=new Float((float)*v / other->toFloat());
		return true;
	}

	virtual bool visitDouble(Double* v, Number* other) 
	{
		delete result;
		result=new Double((double)*v / other->toDouble());
		return true;
	}
};


//添加一个所有数据类型的访问者(+,-,*,/,>,<...)
#define ADD_ALL_NumberVisiter(className,op)\
	class className : public NumberVisiter								\
	{																	\
	public:																\
		className(){}													\
		virtual ~className(){}											\
	public:																\
		virtual bool visitInteger(Integer* v,Number* other) 			\
		{																\
			delete result;												\
			result=new Integer((int)*v op other->toInt());				\
			return true;												\
		}																\
																		\
		virtual bool visitLong(Long* v, Number* other) 					\
		{																\
			delete result;												\
			result=new Long((lint)*v op other->toLong());				\
			return true;												\
		}																\
																		\
		virtual bool visitFloat(Float* v,Number* other) 				\
		{																\
			delete result;												\
			result=new Float((float)*v op other->toFloat());			\
			return true;												\
		}																\
																		\
		virtual bool visitDouble(Double* v, Number* other) 				\
		{																\
			delete result;												\
			result=new Double((double)*v op other->toDouble());			\
			return true;												\
		}																\
	};

ADD_ALL_NumberVisiter(NumberAddVisiter			, + );
ADD_ALL_NumberVisiter(NumberSubVisiter			, - );
ADD_ALL_NumberVisiter(NumberMulVisiter			, * );
ADD_ALL_NumberVisiter(NumberDivVisiter			, / );

ADD_ALL_NumberVisiter(NumberLessThanVisiter		, < );
ADD_ALL_NumberVisiter(NumberGreaterThanVisiter	, > );
ADD_ALL_NumberVisiter(NumberLessThanEqVisiter	, <= );
ADD_ALL_NumberVisiter(NumberGreaterThanEqVisiter, >= );
ADD_ALL_NumberVisiter(NumberEqualVisiter		, == );
ADD_ALL_NumberVisiter(NumberNotEqualVisiter		, != );

//添加一个整型数据类型的访问者(<<,>>,&&,||,%,&,|,^)
#define ADD_INT_NumberVisiter(className,op)\
	class className : public NumberVisiter								\
	{																	\
	public:																\
		className(){}													\
		virtual ~className(){}											\
	public:																\
		virtual bool visitInteger(Integer* v,Number* other) 			\
		{																\
			delete result;												\
			result=new Integer((int)*v op other->toInt());				\
			return true;												\
		}																\
																		\
		virtual bool visitLong(Long* v, Number* other) 					\
		{																\
			delete result;												\
			result=new Long((lint)*v op other->toLong());				\
			return true;												\
		}																\
																		\
		virtual bool visitFloat(Float* v,Number* other) 				\
		{																\
			return false;												\
		}																\
																		\
		virtual bool visitDouble(Double* v, Number* other) 				\
		{																\
			return false;												\
		}																\
	};

ADD_INT_NumberVisiter(NumberLeftShiftVisiter	, <<);
ADD_INT_NumberVisiter(NumberRightShiftVisiter	, >>);
ADD_INT_NumberVisiter(NumberAndVisiter			, &&);
ADD_INT_NumberVisiter(NumberOrVisiter			, ||);
ADD_INT_NumberVisiter(NumberModVisiter			, % );
ADD_INT_NumberVisiter(NumberBitAndVisiter		, & );
ADD_INT_NumberVisiter(NumberBitOrVisiter		, | );
ADD_INT_NumberVisiter(NumberBitXorVisiter		, ^ );

//添加一个所有数据类型的单目运算的访问者(-,&,*)
#define ADD_ONE_ALL_NumberVisiter(className,op)\
	class className : public NumberVisiter								\
	{																	\
	public:																\
		className(){}													\
		virtual ~className(){}											\
	public:																\
		virtual bool visitInteger(Integer* v,Number* other) 			\
		{																\
			delete result;												\
			result=new Integer(op(int)*v);								\
			return true;												\
		}																\
																		\
		virtual bool visitLong(Long* v, Number* other) 					\
		{																\
			delete result;												\
			result=new Long(op(lint)*v);								\
			return true;												\
		}																\
																		\
		virtual bool visitFloat(Float* v,Number* other) 				\
		{																\
			delete result;												\
			result=new Float(op(float)*v);								\
			return true;												\
		}																\
																		\
		virtual bool visitDouble(Double* v, Number* other) 				\
		{																\
			delete result;												\
			result=new Double(op(double)*v);							\
			return true;												\
		}																\
	};

ADD_ONE_ALL_NumberVisiter(NumberNegativeVisiter	, - );

//添加一个整型数据类型的单目运算的访问者
#define ADD_ONE_INT_NumberVisiter(className,op)\
	class className : public NumberVisiter								\
	{																	\
	public:																\
		className(){}													\
		virtual ~className(){}											\
	public:																\
		virtual bool visitInteger(Integer* v,Number* other) 			\
		{																\
			delete result;												\
			result=new Integer(op(int)*v);								\
			return true;												\
		}																\
																		\
		virtual bool visitLong(Long* v, Number* other) 					\
		{																\
			delete result;												\
			result=new Long(op(lint)*v);								\
			return true;												\
		}																\
																		\
		virtual bool visitFloat(Float* v,Number* other) 				\
		{																\
			return false;												\
		}																\
																		\
		virtual bool visitDouble(Double* v, Number* other) 				\
		{																\
			return false;												\
		}																\
	};

ADD_ONE_INT_NumberVisiter(NumberNotVisiter		, ! );
ADD_ONE_INT_NumberVisiter(NumberBitNotVisiter	, ~ );

//添加所有数据类型的赋值运算的访问者(=,+=,*=...)
#define ADD_ASSIGN_NumberVisiter(className,op)							 \
	class className : public NumberVisiter								 \
	{																	 \
	public:																 \
		className(){}													 \
		virtual ~className(){}											 \
	public:																 \
		virtual bool visitInteger(Integer* v,Number* other) 			 \
		{																 \
			delete result;												 \
			int i=v->toInt();											 \
			i op other->toInt();										 \
			v->setValue(i);												 \
			result=new Integer(i);										 \
			return true;												 \
		}																 \
																		 \
		virtual bool visitLong(Long* v, Number* other) 					 \
		{																 \
			delete result;												 \
			lint i=v->toLong();											 \
			i op other->toLong();										 \
			v->setValue(i);												 \
			result=new Long(i);											 \
			return true;												 \
		}																 \
																		 \
		virtual bool visitFloat(Float* v,Number* other) 				 \
		{																 \
			delete result;												 \
			float f=v->toFloat();										 \
			f op other->toFloat();										 \
			v->setValue(f);												 \
			result=new Float(f);										 \
			return true;												 \
		}																 \
																		 \
		virtual bool visitDouble(Double* v, Number* other) 				 \
		{																 \
			delete result;												 \
			double f=v->toDouble();										 \
			f op other->toDouble();										 \
			v->setValue(f);												 \
			result=new Double(f);										 \
			return true;												 \
		}																 \
	};

ADD_ASSIGN_NumberVisiter(NumberAssignVisiter		, =		);
ADD_ASSIGN_NumberVisiter(NumberAssignAddVisiter		, +=	);
ADD_ASSIGN_NumberVisiter(NumberAssignSubVisiter		, -=	);
ADD_ASSIGN_NumberVisiter(NumberAssignMulVisiter		, *=	);
ADD_ASSIGN_NumberVisiter(NumberAssignDivVisiter		, /=	);

//添加一个整型数据类型的赋值运算的访问者(%=,&=,>>=...)
#define ADD_ASSIGN_INT_NumberVisiter(className,op)						 \
	class className : public NumberVisiter								 \
	{																	 \
	public:																 \
		className(){}													 \
		virtual ~className(){}											 \
	public:																 \
		virtual bool visitInteger(Integer* v,Number* other) 			 \
		{																 \
			delete result;												 \
			int i=v->toInt();											 \
			i op other->toInt();										 \
			v->setValue(i);												 \
			result=new Integer(i);										 \
			return true;												 \
		}																 \
																		 \
		virtual bool visitLong(Long* v, Number* other) 					 \
		{																 \
			delete result;												 \
			lint i=v->toLong();											 \
			i op other->toLong();										 \
			v->setValue(i);												 \
			result=new Long(i);											 \
			return true;												 \
		}																 \
																		 \
		virtual bool visitFloat(Float* v,Number* other) 				 \
		{																 \
			return false;												 \
		}																 \
																		 \
		virtual bool visitDouble(Double* v, Number* other) 				 \
		{																 \
			return false;												 \
		}																 \
	};

ADD_ASSIGN_INT_NumberVisiter(NumberAssignModVisiter		, %=	);
ADD_ASSIGN_INT_NumberVisiter(NumberAssignRShiftVisiter	, >>=	);
ADD_ASSIGN_INT_NumberVisiter(NumberAssignLShiftVisiter	, <<=	);
ADD_ASSIGN_INT_NumberVisiter(NumberAssignBAndVisiter	, &=	);
ADD_ASSIGN_INT_NumberVisiter(NumberAssignBXorVisiter	, ^=	);
ADD_ASSIGN_INT_NumberVisiter(NumberAssignBOrVisiter		, |=	);


}//end of namespace bluemei

