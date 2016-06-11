#pragma once
#include "orm/drivers/mysql/include/mysql.h"
#include "SqlExpression.h"
#include "Driver.h"


namespace brpc{


//结果集
class MySQLResultSet : public ResultSet
{
protected:
	MySQLResultSet();
private:
	friend class MySQLConnection;
	MySQLResultSet(MYSQL_RES *r);
public:
	virtual ~MySQLResultSet();
public:
	//取下一条记录
	virtual bool next();
	virtual bool previous();

	//定位
	virtual bool absolute(uint64 row);
	virtual void afterLast();
	virtual void beforeFirst();

	//结果集合中列的数量
	virtual uint32 fieldsCount() const;
	//结果集合中行的数量
	virtual uint64 rowsCount() const;

	virtual uint32 columnIndex(cstring columnName) const;

	//关闭
	virtual void close();
	virtual bool isClosed() const;

	//取字符串数据
	virtual cstring getString(uint32 columnIndex);
	virtual cstring getString(cstring columnName);

	//取int型数据
	virtual int getInt(uint32 columnIndex);
	virtual int getInt(cstring columnName);

	//取double型数据
	virtual double getDouble(uint32 columnIndex);
	virtual double getDouble(cstring columnName);

	//取字节数据
	virtual ByteArray getBytes(uint32 columnIndex);
	virtual ByteArray getBytes(cstring columnName);

	//取字Object据
	virtual Object* getObject(uint32 columnIndex);
	virtual Object* getObject(cstring columnName);

	//取数据长度
	virtual size_t getDataLength(uint32 columnIndex);
	virtual size_t getDataLength(cstring columnName);
protected:
	void checkIndex(uint32 columnIndex);
protected:
	MYSQL_RES *m_result;
	uint64 m_previousIndex;
};


//连接
class MySQLConnection : public DriverConnection
{
public:
	MySQLConnection();
private:
	friend class MySQLDriver;
	MySQLConnection(MYSQL *c);
public:
	virtual void open(const String& connInfo);
	virtual void close();

	virtual void execute(cstring sql, size_t len=0);
	virtual ResultSet* executeQuery(cstring sql, size_t len=0);
	//update,delete,insert等影响的记录数
	virtual int executeUpdate(cstring sql, size_t len=0);

	virtual bool usingDatabase(cstring dbName);
	virtual void setCharset(cstring charset);//"utf8"

	//包含二进制数据
	virtual void executeSQLWithBytes(cstring sql,cstring buf,size_t len);
	//包含多项二进制数据
	virtual void executeSQLWithBytesArray(cstring sql,ByteArray bufs[],int num);
protected:
	MYSQL *m_mysql;
};

//驱动
class MySQLDriver
{
public:
	static MySQLConnection* connect(
		const char *host,
		const char *username,
		const char *password,
		const char *db,
		unsigned int port=0)
	{
		MYSQL *conn=NULL;
		conn = mysql_init(NULL);
		if (conn == NULL) {
			cstring error=mysql_error(conn);
			throw SQLException(error);
		}
		if ((mysql_real_connect(conn, host, username, password, db, port, NULL, 0)) == NULL) {
			cstring error=mysql_error(conn);
			throw SQLException(error);
		}
		return new MySQLConnection(conn);
	}
};


}//end of namespace brpc