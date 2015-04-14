#include "stdafx.h"
#include "XmlParseException.h"

namespace bluemei{

XmlParseException::~XmlParseException(void)
{
}

String XmlParseException::toString()const{
	String str="XmlParseException : ";
	return str.append(this->m_strLastError);//this->what()
}

}