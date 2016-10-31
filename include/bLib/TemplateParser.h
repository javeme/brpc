#pragma once
#include "Object.h"
#include "TemplateExpr.h"
#include "CodeUtil.h"

namespace blib{

typedef char char_t;

class BLUEMEILIB_API ParseTemplateException : public Exception
{
public:
	ParseTemplateException(const String& type):Exception(type){}
	virtual String name() const {
		return "ParseTemplateException";
	}
};

class BLUEMEILIB_API Token : public Object
{
public:
	typedef enum Symbol{
		NONE = 0,
		END = 1,
		CONST_STR = 2,
		IDENT = 3,
		SYM_END
	}sym_t;

public:
	Token(unsigned int pos=0){
		this->token = NONE;
		this->pos = pos;
		this->lineNo = 0;
		this->linePos = 0;
		this->str = "";
		this->intVal = 0;
		this->fVal = 0;
	}

	void nextPos(unsigned int offset=1){
		this->pos += offset;
		this->linePos += offset;
	}

	void nextLine(){
		this->pos += 1;
		this->lineNo += 1;
		this->linePos = 0;
	}

	String toString() const{
		return String::format("Token:%d(%s)(line %u:%u)",
			this->token, this->str.c_str(), this->lineNo, this->linePos);
	}
private:
	Symbol token;
	unsigned int pos;
	unsigned int lineNo;
	unsigned int linePos;

	String str;
	union{ int intVal; double fVal; };

	friend class Tokenizer;
	friend class TemplateParser;
};

class BLUEMEILIB_API Tokenizer : public Object
{
public:
	Tokenizer(const String& s);
	virtual ~Tokenizer();

	void reset(const String& s);
	void destroy();

	char readChar(unsigned int pos)
	{
		if(pos >= content.length())
			throwpe(OutOfBoundException(pos, content.length()));
		return this->content[pos];
	}

	String readIdent(unsigned int pos);
	unsigned int readString(unsigned int pos, String& str);

	Token* readNext(Token* current);

	Token currentToken(){
		return *this->current;
	}

	Token nextToken();

	Token peekToken(){
		return *this->next;
	}
private:
	String content;
	Token* current;
	Token* next;
};

class BLUEMEILIB_API TemplateParser : public Object
{
public:
	TemplateParser() : tokenizer("") {}
	virtual ~TemplateParser(){}

	TemplateExpr* parse(const String& tmpl){
		this->tokenizer.reset(tmpl);
		return this->tmpl();
	}

	Token currentToken(){
		return this->tokenizer.currentToken();
	}

	Token peekToken(){
		return this->tokenizer.peekToken();
	}

	Token nextToken(){
		return this->tokenizer.nextToken();
	}

	TemplateExpr* joinConstString(){
		return new StringExpr(this->nextToken().str);
	}

	TemplateExpr* joinVar(){
		return new VarExpr(this->nextToken().str);
	}

	TemplateExpr* expr();

	TemplateExpr* joiner(TemplateExpr* left);

	TemplateExpr* stmt();

	TemplateExpr* tmpl();

private:
	Tokenizer tokenizer;
};

}