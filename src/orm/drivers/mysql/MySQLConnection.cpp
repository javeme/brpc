#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "orm/drivers/mysql/MySQLConnection.h"
#include "src/type/Number.h"
#include "src/util/Url.h"


namespace brpc{

MySQLResultSet::MySQLResultSet()
{
	m_result=nullptr;
	m_previousIndex=-1;
}

MySQLResultSet::MySQLResultSet(MYSQL_RES *r)
{
	m_result=r;
	m_previousIndex=-1;
}

MySQLResultSet::~MySQLResultSet()
{
	close();
}

bool MySQLResultSet::next()
{
	m_previousIndex++;
	if(m_result==nullptr)
	{
		throw SQLException("there is no result.");
	}
	MYSQL_ROW currentRow=mysql_fetch_row(m_result);
	return (currentRow!=nullptr);
}

bool MySQLResultSet::previous()
{
	if(m_previousIndex<0)
	{
		mysql_data_seek(m_result,0);
		m_previousIndex=-1-1;
		return false;
	}
	else
	{
		mysql_data_seek(m_result,m_previousIndex);
		m_previousIndex-=2;
		return next();
	}
	//MYSQL_ROW_OFFSET offset=mysql_row_tell(m_result);
	//MYSQL_ROW_OFFSET seekOffset=mysql_row_seek(m_result,offset);
}

//定位到某一行,row:[0,rowNum-1]
bool MySQLResultSet::absolute(uint64 row)
{
	if(row<0 || row>=rowsCount())
		return false;
	mysql_data_seek(m_result,row);
	m_previousIndex=row-1-1;//置previous-cursor为上一个位置(第二次-1是因为需要抵消接下来的next()调用)
	return next();//移到row位置,置next-cursor为下一个位置
}

void MySQLResultSet::afterLast()
{
	absolute(rowsCount()-1);//定位到最后一行
	next();
}

void MySQLResultSet::beforeFirst()
{
	absolute(0);
	previous();
}


uint32 MySQLResultSet::fieldsCount() const
{
	if(m_result==nullptr)
	{
		throw SQLException("there is no result.");
	}
	//mysql_field_count()表示应该返回的列数(result为null时照样可以得到)
	return mysql_num_fields(m_result);
}

uint64 MySQLResultSet::rowsCount() const
{
	if(m_result==nullptr)
	{
		throw SQLException("there is no result.");
	}
	return mysql_num_rows(m_result);
}

uint32 MySQLResultSet::columnIndex(cstring columnName) const
{
	//MYSQL_FIELD_OFFSET offset=mysql_field_tell(result);
	//MYSQL_FIELD_OFFSET offset=mysql_field_seek(result,0);
	//
	uint32 count=fieldsCount();
	if(count<=0)
		throw SQLException("there is no any column.");

	uint32 pos=-1;
	MYSQL_FIELD *fields = mysql_fetch_fields(m_result);
	for(uint32 i = 0; i < count; i++){
		if(strcmp(fields[i].name,columnName)==0)
		{
			pos=i;
			break;
		}
	}
	return pos;
}

void MySQLResultSet::close()
{
	if(m_result!=nullptr)
	{
		mysql_free_result(m_result);//释放结果资源
		m_result=nullptr;
	}
}

bool MySQLResultSet::isClosed() const
{
	return m_result==nullptr;
}

void MySQLResultSet::checkIndex(uint32 columnIndex)
{
	if(m_result==nullptr){
		throw SQLException("there is no result.");
	}
	else if(m_result->current_row==nullptr){
		throw SQLException("there is no record any more.");
	}
	else if(columnIndex<0 || columnIndex>=m_result->field_count){
		throw SQLException("column index out of bound.");
	}
}

cstring MySQLResultSet::getString(uint32 columnIndex)
{
	/*size_t *fieldsDataLenArray=mysql_fetch_lengths(m_result);
	for (i=0; i<m_result->field_count; i++) {
		MYSQL_ROW row=m_result->current_row;
		return string(row[i],fieldsDataLenArray[i]);
	}*/
	checkIndex(columnIndex);
	return m_result->current_row[columnIndex];
}

cstring MySQLResultSet::getString(cstring columnName)
{
	//MYSQL_FIELD* field=mysql_fetch_field(m_result);
	return getString(columnIndex(columnName));
}

int MySQLResultSet::getInt(uint32 columnIndex)
{
	return CodeUtil::str2Int(getString(columnIndex));
}

int MySQLResultSet::getInt(cstring columnName)
{
	return getInt(columnIndex(columnName));
}

double MySQLResultSet::getDouble(uint32 columnIndex)
{
	return CodeUtil::str2Float(getString(columnIndex));
}

double MySQLResultSet::getDouble(cstring columnName)
{
	return getDouble(columnIndex(columnName));
}

ByteArray MySQLResultSet::getBytes(uint32 columnIndex)
{
	checkIndex(columnIndex);
	return ByteArray((byte*)m_result->current_row[columnIndex],
		getDataLength(columnIndex));
}

ByteArray MySQLResultSet::getBytes(cstring columnName)
{
	return getBytes(columnIndex(columnName));
}

Object* MySQLResultSet::getObject(uint32 columnIndex)
{
	checkIndex(columnIndex);
	cstring val = m_result->current_row[columnIndex];
	MYSQL_FIELD *fields = mysql_fetch_fields(m_result);
	auto fieldType = fields[columnIndex].type;
	switch(fieldType)
	{
	case MYSQL_TYPE_BIT:
		return new Boolean(CodeUtil::str2Int(val) != 0);
	case MYSQL_TYPE_TINY:
		return new Char(CodeUtil::str2Int(val));
	case MYSQL_TYPE_SHORT:
		return new Short(CodeUtil::str2Int(val));
	case MYSQL_TYPE_INT24:
	case MYSQL_TYPE_LONG:
		return new Integer(CodeUtil::str2Int(val));
	case MYSQL_TYPE_LONGLONG:
		return new Long(::_atoi64(val));
	case MYSQL_TYPE_FLOAT:
		return new Float((float)CodeUtil::str2Float(val));
	case MYSQL_TYPE_DOUBLE:
		return new Double(CodeUtil::str2Float(val));
	case MYSQL_TYPE_VAR_STRING:
	case MYSQL_TYPE_STRING:
	case MYSQL_TYPE_VARCHAR:
		return new String(val);
	/* TODO: parse more type
		MYSQL_TYPE_DECIMAL
		MYSQL_TYPE_NULL,
		MYSQL_TYPE_TIMESTAMP,
		MYSQL_TYPE_DATE,
		MYSQL_TYPE_TIME,
		MYSQL_TYPE_DATETIME,
		MYSQL_TYPE_YEAR,
		MYSQL_TYPE_NEWDATE,
		MYSQL_TYPE_NEWDECIMAL,
		MYSQL_TYPE_ENUM,
		MYSQL_TYPE_SET,
		MYSQL_TYPE_TINY_BLOB,
		MYSQL_TYPE_MEDIUM_BLOB,
		MYSQL_TYPE_LONG_BLOB,
		MYSQL_TYPE_BLOB,
		MYSQL_TYPE_GEOMETRY,
	*/
	default:
		return null;
	}
}

Object* MySQLResultSet::getObject(cstring columnName)
{
	return getObject(columnIndex(columnName));
}

size_t MySQLResultSet::getDataLength(uint32 columnIndex)
{
	checkIndex(columnIndex);
	return mysql_fetch_lengths(m_result)[columnIndex];
}

size_t MySQLResultSet::getDataLength(cstring columnName)
{
	return getDataLength(columnIndex(columnName));
}


/************************MySQLConnection***********************/
MySQLConnection::MySQLConnection()
{
	m_mysql=mysql_init(NULL);
	if (m_mysql == NULL) {
		cstring error=mysql_error(m_mysql);
		throw SQLException(error);
	}
}

MySQLConnection::MySQLConnection(MYSQL *c)
{
	m_mysql=c;
}

void MySQLConnection::open(const String& connInfo)
{
	Url url(connInfo);
	String host = url.getHost();
	uint32 port = (uint32)CodeUtil::str2Int(url.getPort("3306"));
	String db = url.getPath();
	String username = url.getParameter("username");
	String password = url.getParameter("password");
	String charset = url.getParameter("charset");

	if ((mysql_real_connect(m_mysql, host, username, password,
		db, port, NULL, 0)) == NULL)
	{
		cstring error=mysql_error(m_mysql);
		throw SQLException(error);
	}

	if (!charset.empty()) {
		this->setCharset(charset);
	}
}

void MySQLConnection::close()
{
	mysql_close(m_mysql);
}

void MySQLConnection::execute(cstring sql,size_t len/*=0*/)
{
	if(len==0)
		len=strlen(sql);
	int flag = mysql_real_query(m_mysql, sql, len);
	if (flag != 0) {
		throw SQLException(mysql_error(m_mysql));
	}
}

ResultSet* MySQLConnection::executeQuery(cstring sql, size_t len)
{
	this->execute(sql, len);
	MYSQL_RES *result = mysql_store_result(m_mysql);
	return new MySQLResultSet(result);
}

int MySQLConnection::executeUpdate(cstring sql, size_t len)
{
	this->execute(sql, len);
	return (int)mysql_affected_rows(m_mysql);
}

bool MySQLConnection::usingDatabase(cstring dbName)
{
	/************************************************************************/
	/* mysql_select_db
	返回值
	0表示成功，非0值表示出现错误。
	错误
	CR_COMMANDS_OUT_OF_SYNC  以不恰当的顺序执行了命令。
	CR_SERVER_GONE_ERROR  MySQL服务器不可用。
	CR_SERVER_LOST 在查询过程中，与服务器的连接丢失。
	CR_UNKNOWN_ERROR  出现未知错误。                                                                      */
	/************************************************************************/
	int ret=mysql_select_db(m_mysql,dbName);
	if(ret==0)
	{
		return true;
	}
	else
	{
		throw SQLException(mysql_error(m_mysql));
	}
}

void MySQLConnection::setCharset(cstring charset) /*"utf8" */
{
	if(0!=mysql_set_character_set(m_mysql, charset)) {
		throw SQLException(mysql_error(m_mysql));
	}
}

void MySQLConnection::executeSQLWithBytes(cstring sql,cstring buf,size_t len)
{
	//like: bytes2SQLString("insert into tb(name, file) values('peter',?)", buf);
	size_t total=strlen(sql)+2*len+1+1;//加入单引号
	char* newbuf=new char[total];

	cstring tail=strstr(sql,"?");
	size_t pos=tail-sql;//暂时没有判断是否"?"被单引号包起来了(如'a?b')
	memcpy(newbuf,sql,pos);
	newbuf[pos++]='\'';
	pos+=mysql_real_escape_string(m_mysql, newbuf+pos, buf, len);
	newbuf[pos++]='\'';
	++tail;
	strncpy_s(newbuf+pos,total-pos,tail,strlen(tail));//sql"?"号后面的字符串
	total=pos+strlen(tail);
	try
	{
		this->execute(newbuf,total);
		delete[] newbuf;
	}
	catch(SQLException& e)
	{
		delete[] newbuf;
		throw e;
	}
}

void MySQLConnection::executeSQLWithBytesArray(cstring sql,ByteArray bufs[],int num)
{
	size_t total=strlen(sql)+1;//加入单引号
	for(int i=0;i<num;i++)
	{
		total+=2*bufs[i].length+1;
	}
	char* newbuf=new char[total];

	size_t pos=0;
	cstring startSql=sql;
	for(int i=0;i<num;i++)
	{
		cstring tail=strstr(startSql,"?");
		size_t size=tail-startSql;
		memcpy(newbuf+pos,startSql,size);//"?"号前面的字符串
		pos+=size;
		newbuf[pos++]='\'';
		pos+=mysql_real_escape_string(m_mysql, newbuf+pos, (cstring)bufs[i].data, bufs[i].length);
		newbuf[pos++]='\'';

		startSql=tail+1;
	}
	strncpy_s(newbuf+pos,total-pos,startSql,strlen(startSql));//sql最后一个"?"号后面的字符串
	total=pos+strlen(startSql);
	try
	{
		this->execute(newbuf,total);
		delete[] newbuf;
	}
	catch(SQLException& e)
	{
		delete[] newbuf;
		throw e;
	}
}


MySQLConnection* MySQLDriver::connect(const char *host, const char *username,
	const char *password, const char *db, unsigned int port/*=0*/)
{
	MYSQL *conn=NULL;
	conn = mysql_init(NULL);
	if (conn == NULL) {
		cstring error=mysql_error(conn);
		throw SQLException(error);
	}
	if ((mysql_real_connect(conn, host, username, password, db, port,
		NULL, 0)) == NULL) {
		cstring error=mysql_error(conn);
		throw SQLException(error);
	}
	return new MySQLConnection(conn);
}

}//end of namespace brpc