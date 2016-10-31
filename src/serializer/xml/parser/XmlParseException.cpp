#pragma once
#include "XmlParseException.h"

namespace brpc{

XmlParseException::~XmlParseException(void)
{
}

String XmlParseException::toString()const{
	String str="XmlParseException : ";
	return str.append(this->m_strLastError);//this->what()
}

}