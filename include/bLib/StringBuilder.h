/* StringBuilder.java -- Unsynchronized growable strings
	 Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2004, 2005
	 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.	If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.	Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.	An independent module is a module which is not derived from
or based on this library.	If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.	If you do not wish to do so, delete this
exception statement from your version. */

//package java.lang;
//import java.io.Serializable;
#pragma once
#include "RuntimeException.h"
#include "ArrayList.h"
#include "BString.h"

namespace blib{

/**
 * <code>StringBuilder</code> represents a changeable <code>String</code>.
 * It provides the operations required to modify the
 * <code>StringBuilder</code>, including insert, replace, delete, append,
 * and reverse. It like <code>StringBuffer</code>, but is not
 * synchronized.	It is ideal for use when it is known that the
 * object will only be used from a single thread.
 *
 * <p><code>StringBuilder</code>s are variable-length in nature, so even if
 * you initialize them to a certain size, they can still grow larger than
 * that. <em>Capacity</em> indicates the number of characters the
 * <code>StringBuilder</code> can have in it before it has to grow (growing
 * the char_t array is an expensive operation involving <code>new</code>).
 *
 * <p>Incidentally, compilers often implement the String operator "+"
 * by using a <code>StringBuilder</code> operation:<br>
 * <code>a + b</code><br>
 * is the same as<br>
 * <code>new StringBuilder().append(a).append(b).toString()</code>.
 *
 * <p>Classpath's StringBuilder is capable of sharing memory with Strings for
 * efficiency.	This will help when a StringBuilder is converted to a String
 * and the StringBuilder is not changed after that (quite common when
 * performing string concatenation).
 *
 * @author blib(javaloveme@gmai.com)(update)
 * @author Paul Fisher
 * @author John Keiser
 * @author Tom Tromey
 * @author Eric Blake (ebb9@email.byu.edu)
 * @see String
 * @see StringBuffer
 *
 * @since 1.5
 */
class BLUEMEILIB_API StringBuilder : public Object
{
	// Implementation note: if you change this class, you usually will
	// want to change StringBuffer as well.
private:
	//容量
	unsigned int capacity;
	/**
	 * Index of next available character (and thus the size of the current
	 * string contents).	Note that this has permissions set this way so that
	 * String can get the value.
	 *
	 * @serial the number of characters in the buffer
	 */
	unsigned int avail;

	/**
	 * The buffer.	Note that this has permissions set this way so that String
	 * can get the value.
	 *
	 * @serial the buffer
	 */
	typedef char char_t;
	char_t* buffer;

public:
	/**
	 * The default capacity of a buffer.
	 */
	enum {DEFAULT_CAPACITY = 16};


	/**
	 * Create an empty <code>StringBuilder</code> with the specified initial
	 * capacity.
	 *
	 * @param capacity the initial capacity
	 * @throws NegativeArraySizeException if capacity is negative
	 */
	StringBuilder(unsigned int capacity=DEFAULT_CAPACITY);

	/**
	 * Create a new <code>StringBuilder</code> with the characters in the
	 * specified <code>String</code>. Initial capacity will be the size of the
	 * String plus 16.
	 *
	 * @param str the <code>String</code> to convert
	 * @throws NullPointerException if str is null
	 */
	StringBuilder(const String& str);

	virtual ~StringBuilder();

public:
	StringBuilder(const StringBuilder &src);
	StringBuilder(StringBuilder&& src);//move

	StringBuilder& operator= (const StringBuilder &src);
	StringBuilder& operator= (StringBuilder&& src);//move

	bool operator== (const StringBuilder& other) const;

public:
	/**
	 * Get the length of the <code>String</code> this <code>StringBuilder</code>
	 * would create. Not to be confused with the <em>capacity</em> of the
	 * <code>StringBuilder</code>.
	 *
	 * @return the length of this <code>StringBuilder</code>
	 * @see #capacity()
	 * @see #setLength(int)
	 */
	unsigned int length() const
	{
		return avail;
	}

	/**
	 * Get the total number of characters this <code>StringBuilder</code> can
	 * support before it must be grown.	Not to be confused with <em>length</em>.
	 *
	 * @return the capacity of this <code>StringBuilder</code>
	 * @see #length()
	 * @see #ensureCapacity(int)
	 */
	unsigned int getCapacity() const
	{
		return this->capacity;
	}

	/**
	 * Increase the capacity of this <code>StringBuilder</code>. This will
	 * ensure that an expensive growing operation will not occur until
	 * <code>minimumCapacity</code> is reached. The buffer is grown to the
	 * larger of <code>minimumCapacity</code> and
	 * <code>capacity() * 2 + 2</code>, if it is not already large enough.
	 *
	 * @param minimumCapacity the new capacity
	 * @see #capacity()
	 */
	bool ensureCapacity(unsigned int minimumCapacity);

	/**
	 * Set the length of this StringBuilder. If the new length is greater than
	 * the current length, all the new characters are set to '\0'. If the new
	 * length is less than the current length, the first <code>newLength</code>
	 * characters of the old array will be preserved, and the remaining
	 * characters are truncated.
	 *
	 * @param newLength the new length
	 * @throws IndexOutOfBoundsException if the new length is negative
	 *				 (while unspecified, this is a StringIndexOutOfBoundsException)
	 * @see #length()
	 */
	void setLength(unsigned int newLength);

	//clear all content
	void clear();

	/**
	 * Get the character at the specified index.
	 *
	 * @param index the index of the character to get, starting at 0
	 * @return the character at the specified index
	 * @throws IndexOutOfBoundsException if index is negative or &gt;= length()
	 *				 (while unspecified, this is a StringIndexOutOfBoundsException)
	 */
	char_t charAt(unsigned int index) const
	{
		checkBound(index);
		return buffer[index];
	}

	/**
	 * Get the specified array of characters. <code>srcOffset - srcEnd</code>
	 * characters will be copied into the array you pass in.
	 *
	 * @param srcOffset the index to start copying from (inclusive)
	 * @param srcEnd the index to stop copying from (exclusive)
	 * @param dst the array to copy into
	 * @param dstOffset the index to start copying into
	 * @throws NullPointerException if dst is null
	 * @throws IndexOutOfBoundsException if any source or target indices are
	 *				 out of range (while unspecified, source problems cause a
	 *				 StringIndexOutOfBoundsException, and dest problems cause an
	 *				 ArrayIndexOutOfBoundsException)
	 * @see System#arraycopy(Object, unsigned int, Object, unsigned int, unsigned int)
	 */
	void getChars(unsigned int srcOffset, unsigned int srcEnd,
		char_t dst[], unsigned int dstOffset=0) const;

	/**
	 * Set the character at the specified index.
	 *
	 * @param index the index of the character to set starting at 0
	 * @param ch the value to set that character to
	 * @throws IndexOutOfBoundsException if index is negative or &gt;= length()
	 *				 (while unspecified, this is a StringIndexOutOfBoundsException)
	 */
	void setCharAt(unsigned int index, char_t ch);

	/**
	 * Append the <code>String</code> value of the argument to this
	 * <code>StringBuilder</code>. Uses <code>String.valueOf()</code> to convert
	 * to <code>String</code>.
	 *
	 * @param obj the <code>Object</code> to convert and append
	 * @return this <code>StringBuilder</code>
	 * @see String#valueOf(Object)
	 * @see #append(String)
	 */
	StringBuilder& append(Object* obj)
	{
		return append(obj==nullptr?"null":obj->toString());
	}

	/**
	 * Append the <code>String</code> to this <code>StringBuilder</code>. If
	 * str is null, the String "null" is appended.
	 *
	 * @param str the <code>String</code> to append
	 * @return this <code>StringBuilder</code>
	 */
	StringBuilder& append(const String& str);

	/**
	 * Append the <code>StringBuilder</code> value of the argument to this
	 * <code>StringBuilder</code>. This behaves the same as
	 * <code>append((Object) stringBuffer)</code>, except it is more efficient.
	 *
	 * @param stringBuffer the <code>StringBuilder</code> to convert and append
	 * @return this <code>StringBuilder</code>
	 * @see #append(Object)
	 */
	StringBuilder& append(const StringBuilder& stringBuffer);

	/**
	 * Append the <code>char_t</code> array to this <code>StringBuilder</code>.
	 * This is similar (but more efficient) than
	 * <code>append(new String(data))</code>, except in the case of null.
	 *
	 * @param data the <code>char_t[]</code> to append
	 * @return this <code>StringBuilder</code>
	 * @throws NullPointerException if <code>str</code> is <code>null</code>
	 * @see #append(char_t[], unsigned int, unsigned int)
	 */
	StringBuilder& append(const char_t data[])
	{
		return append(data,strlen(data));
	}
	StringBuilder& append(const char_t data[],unsigned int length)
	{
		return append(data, 0, length);
	}

	/**
	 * Append part of the <code>char_t</code> array to this
	 * <code>StringBuilder</code>. This is similar (but more efficient) than
	 * <code>append(new String(data, offset, count))</code>, except in the case
	 * of null.
	 *
	 * @param data the <code>char_t[]</code> to append
	 * @param offset the start location in <code>str</code>
	 * @param count the number of characters to get from <code>str</code>
	 * @return this <code>StringBuilder</code>
	 * @throws NullPointerException if <code>str</code> is <code>null</code>
	 * @throws IndexOutOfBoundsException if offset or count is out of range
	 *				 (while unspecified, this is a StringIndexOutOfBoundsException)
	 */
	StringBuilder& append(const char_t data[], unsigned int offset, unsigned int count);

	/**
	 * Append the <code>String</code> value of the argument to this
	 * <code>StringBuilder</code>. Uses <code>String.valueOf()</code> to convert
	 * to <code>String</code>.
	 *
	 * @param bool the <code>boolean</code> to convert and append
	 * @return this <code>StringBuilder</code>
	 * @see String#valueOf(boolean)
	 */
	StringBuilder& append(bool b)
	{
		cstring str = (b ? "true" : "false");
		//return append(str);//为何调用append(bool)?
		return append(str);
	}

	/**
	 * Append the <code>char_t</code> to this <code>StringBuilder</code>.
	 *
	 * @param ch the <code>char_t</code> to append
	 * @return this <code>StringBuilder</code>
	 */
	StringBuilder& append(char_t ch);


	/**
	 * Append the <code>String</code> value of the argument to this
	 * <code>StringBuilder</code>. Uses <code>String.valueOf()</code> to convert
	 * to <code>String</code>.
	 *
	 * @param inum the <code>unsigned int</code> to convert and append
	 * @return this <code>StringBuilder</code>
	 * @see String#valueOf(unsigned int)
	 */
	// This is native in libgcj, for efficiency.
	StringBuilder& append(int i)
	{
		return append(Value2String<int>(i));
	}

	/**
	 * Append the <code>String</code> value of the argument to this
	 * <code>StringBuilder</code>. Uses <code>String.valueOf()</code> to convert
	 * to <code>String</code>.
	 *
	 * @param lnum the <code>long</code> to convert and append
	 * @return this <code>StringBuilder</code>
	 * @see String#valueOf(long)
	 */
	StringBuilder& append(long l)
	{
		return append(Value2String<long>(l));
	}

	/**
	 * Append the <code>String</code> value of the argument to this
	 * <code>StringBuilder</code>. Uses <code>String.valueOf()</code> to convert
	 * to <code>String</code>.
	 *
	 * @param fnum the <code>float</code> to convert and append
	 * @return this <code>StringBuilder</code>
	 * @see String#valueOf(float)
	 */
	StringBuilder& append(float f)
	{
		return append(Value2String<float>(f));
	}

	/**
	 * Append the <code>String</code> value of the argument to this
	 * <code>StringBuilder</code>. Uses <code>String.valueOf()</code> to convert
	 * to <code>String</code>.
	 *
	 * @param dnum the <code>double</code> to convert and append
	 * @return this <code>StringBuilder</code>
	 * @see String#valueOf(double)
	 */
	StringBuilder& append(double df)
	{
		return append(Value2String<double>(df));
	}

	/**
	 * Delete characters from this <code>StringBuilder</code>.
	 * <code>delete(10, 12)</code> will delete 10 and 11, but not 12. It is
	 * harmless for end to be larger than length().
	 *
	 * @param start the first character to delete
	 * @param end the index after the last character to delete
	 * @return this <code>StringBuilder</code>
	 * @throws StringIndexOutOfBoundsException if start or end are out of bounds
	 */
	StringBuilder& deleteSub(unsigned int start, unsigned int end);

	/**
	 * Delete a character from this <code>StringBuilder</code>.
	 *
	 * @param index the index of the character to delete
	 * @return this <code>StringBuilder</code>
	 * @throws StringIndexOutOfBoundsException if index is out of bounds
	 */
	StringBuilder& deleteCharAt(unsigned int index)
	{
		return deleteSub(index, index + 1);
	}
	
	/**
	 * Delete the last character from this <code>StringBuilder</code>.
	 */
	StringBuilder& pop()
	{
		return deleteCharAt(length()-1);
	}
	
	/**
	 * Is this <code>StringBuilder</code> started with the char `ch`.
	 */
	bool startWith(char_t ch)
	{
		if(length() > 0 && ch == charAt(0))
			return true;
		return false;
	}
	
	/**
	 * Is this <code>StringBuilder</code> ended with the char `ch`.
	 */
	bool endWith(char_t ch)
	{
		if(length() > 0 && ch == charAt(length()-1))
			return true;
		return false;
	}

	/**
	 * Replace characters between index <code>start</code> (inclusive) and
	 * <code>end</code> (exclusive) with <code>str</code>. If <code>end</code>
	 * is larger than the size of this StringBuilder, all characters after
	 * <code>start</code> are replaced.
	 *
	 * @param start the beginning index of characters to delete (inclusive)
	 * @param end the ending index of characters to delete (exclusive)
	 * @param str the new <code>String</code> to insert
	 * @return this <code>StringBuilder</code>
	 * @throws StringIndexOutOfBoundsException if start or end are out of bounds
	 * @throws NullPointerException if str is null
	 */
	StringBuilder& replace(unsigned int start, unsigned int end, const String& str);

	/**
	 * Creates a substring of this StringBuilder, starting at a specified index
	 * and ending at the end of this StringBuilder.
	 *
	 * @param beginIndex index to start substring (base 0)
	 * @return new String which is a substring of this StringBuilder
	 * @throws StringIndexOutOfBoundsException if beginIndex is out of bounds
	 * @see #substring(unsigned int, unsigned int)
	 */
	String substring(unsigned int beginIndex) const
	{
		return substring(beginIndex, avail);
	}


	/**
	 * Creates a substring of this StringBuilder, starting at a specified index
	 * and ending at one character before a specified index.
	 *
	 * @param beginIndex index to start at (inclusive, base 0)
	 * @param endIndex index to end at (exclusive)
	 * @return new String which is a substring of this StringBuilder
	 * @throws StringIndexOutOfBoundsException if beginIndex or endIndex is out
	 *				 of bounds
	 */
	String substring(unsigned int beginIndex, unsigned int endIndex) const;

	/**
	 * Insert a subarray of the <code>char_t[]</code> argument into this
	 * <code>StringBuilder</code>.
	 *
	 * @param offset the place to insert in this buffer
	 * @param str the <code>char_t[]</code> to insert
	 * @param str_offset the index in <code>str</code> to start inserting from
	 * @param len the number of characters to insert
	 * @return this <code>StringBuilder</code>
	 * @throws NullPointerException if <code>str</code> is <code>null</code>
	 * @throws StringIndexOutOfBoundsException if any index is out of bounds
	 */
	StringBuilder& insert(unsigned int offset, const char_t str[],
		unsigned int strOffset, unsigned int len);

	/**
	 * Insert the <code>String</code> value of the argument into this
	 * <code>StringBuilder</code>. Uses <code>String.valueOf()</code> to convert
	 * to <code>String</code>.
	 *
	 * @param offset the place to insert in this buffer
	 * @param obj the <code>Object</code> to convert and insert
	 * @return this <code>StringBuilder</code>
	 * @exception StringIndexOutOfBoundsException if offset is out of bounds
	 * @see String#valueOf(Object)
	 */
	StringBuilder& insert(unsigned int offset, Object* obj)
	{
		return insert(offset, obj == nullptr ? "null" : obj->toString());
	}

	/**
	 * Insert the <code>String</code> argument into this
	 * <code>StringBuilder</code>. If str is null, the String "null" is used
	 * instead.
	 *
	 * @param offset the place to insert in this buffer
	 * @param str the <code>String</code> to insert
	 * @return this <code>StringBuilder</code>
	 * @throws StringIndexOutOfBoundsException if offset is out of bounds
	 */
	StringBuilder& insert(unsigned int offset, const String& str)
	{
		return insert(offset,str.c_str(), 0, str.length());
	}

	StringBuilder& insert(unsigned int offset, cstring str)
	{
		return insert(offset,str, 0, strlen(str));
	}
	/**
	 * Insert the <code>char_t[]</code> argument into this
	 * <code>StringBuilder</code>.
	 *
	 * @param offset the place to insert in this buffer
	 * @param data the <code>char_t[]</code> to insert
	 * @return this <code>StringBuilder</code>
	 * @throws NullPointerException if <code>data</code> is <code>null</code>
	 * @throws StringIndexOutOfBoundsException if offset is out of bounds
	 * @see #insert(unsigned int, char_t[], unsigned int, unsigned int)
	 */
	StringBuilder& insert(unsigned int offset, const char_t data[], unsigned int len)
	{
		return insert(offset, data, 0, len);
	}

	/**
	 * Insert the <code>String</code> value of the argument into this
	 * <code>StringBuilder</code>. Uses <code>String.valueOf()</code> to convert
	 * to <code>String</code>.
	 *
	 * @param offset the place to insert in this buffer
	 * @param bool the <code>boolean</code> to convert and insert
	 * @return this <code>StringBuilder</code>
	 * @throws StringIndexOutOfBoundsException if offset is out of bounds
	 * @see String#valueOf(boolean)
	 */
	StringBuilder& insert(unsigned int offset, bool b)
	{
		cstring str=b ? "true" : "false";
		return insert(offset,str,strlen(str));
	}

	/**
	 * Insert the <code>char_t</code> argument into this <code>StringBuilder</code>.
	 *
	 * @param offset the place to insert in this buffer
	 * @param ch the <code>char_t</code> to insert
	 * @return this <code>StringBuilder</code>
	 * @throws StringIndexOutOfBoundsException if offset is out of bounds
	 */
	StringBuilder& insert(unsigned int offset, char_t ch)
	{
		cstring str=&ch;
		return insert(offset,str,1);
	}

	/**
	 * Insert the <code>String</code> value of the argument into this
	 * <code>StringBuilder</code>. Uses <code>String.valueOf()</code> to convert
	 * to <code>String</code>.
	 *
	 * @param offset the place to insert in this buffer
	 * @param inum the <code>unsigned int</code> to convert and insert
	 * @return this <code>StringBuilder</code>
	 * @throws StringIndexOutOfBoundsException if offset is out of bounds
	 * @see String#valueOf(unsigned int)
	 */
	StringBuilder& insert(unsigned int offset, int inum)
	{
		return insert(offset, Value2String<int>(inum));
	}

	/**
	 * Insert the <code>String</code> value of the argument into this
	 * <code>StringBuilder</code>. Uses <code>String.valueOf()</code> to convert
	 * to <code>String</code>.
	 *
	 * @param offset the place to insert in this buffer
	 * @param lnum the <code>long</code> to convert and insert
	 * @return this <code>StringBuilder</code>
	 * @throws StringIndexOutOfBoundsException if offset is out of bounds
	 * @see String#valueOf(long)
	 */
	StringBuilder& insert(unsigned int offset, long lnum)
	{
		return insert(offset, Value2String<long>(lnum));
	}

	/**
	 * Insert the <code>String</code> value of the argument into this
	 * <code>StringBuilder</code>. Uses <code>String.valueOf()</code> to convert
	 * to <code>String</code>.
	 *
	 * @param offset the place to insert in this buffer
	 * @param fnum the <code>float</code> to convert and insert
	 * @return this <code>StringBuilder</code>
	 * @throws StringIndexOutOfBoundsException if offset is out of bounds
	 * @see String#valueOf(float)
	 */
	StringBuilder& insert(unsigned int offset, float fnum)
	{
		return insert(offset, Value2String<float>(fnum));
	}

	/**
	 * Insert the <code>String</code> value of the argument into this
	 * <code>StringBuilder</code>. Uses <code>String.valueOf()</code> to convert
	 * to <code>String</code>.
	 *
	 * @param offset the place to insert in this buffer
	 * @param dnum the <code>double</code> to convert and insert
	 * @return this <code>StringBuilder</code>
	 * @throws StringIndexOutOfBoundsException if offset is out of bounds
	 * @see String#valueOf(double)
	 */
	StringBuilder& insert(unsigned int offset, double dnum)
	{
		return insert(offset, Value2String<double>(dnum));
	}

	/**
	 * Finds the first instance of a substring in this StringBuilder.
	 *
	 * @param str String to find
	 * @return location (base 0) of the String, or -1 if not found
	 * @throws NullPointerException if str is null
	 * @see #indexOf(String, unsigned int)
	 */
	int indexOf(const String& str) const
	{
		return indexOf(str, 0);
	}

	/**
	 * Finds the first instance of a String in this StringBuilder, starting at
	 * a given index.	If starting index is less than 0, the search starts at
	 * the beginning of this String.	If the starting index is greater than the
	 * length of this String, or the substring is not found, -1 is returned.
	 *
	 * @param str String to find
	 * @param fromIndex index to start the search
	 * @return location (base 0) of the String, or -1 if not found
	 * @throws NullPointerException if str is null
	 */
	int indexOf(const String& str, unsigned int fromIndex) const;

	/**
	 * Finds the last instance of a substring in this StringBuilder.
	 *
	 * @param str String to find
	 * @return location (base 0) of the String, or -1 if not found
	 * @throws NullPointerException if str is null
	 * @see #lastIndexOf(String, unsigned int)
	 */
	int lastIndexOf(const String& str) const
	{
		return lastIndexOf(str, avail - str.length());
	}

	/**
	 * Finds the last instance of a String in this StringBuilder, starting at a
	 * given index.	If starting index is greater than the maximum valid index,
	 * then the search begins at the end of this String.	If the starting index
	 * is less than zero, or the substring is not found, -1 is returned.
	 *
	 * @param str String to find
	 * @param fromIndex index to start the search
	 * @return location (base 0) of the String, or -1 if not found
	 * @throws NullPointerException if str is null
	 */
	int lastIndexOf(const String& str, unsigned int fromIndex) const;

	/**
	 * Reverse the characters in this StringBuilder. The same sequence of
	 * characters exists, but in the reverse index ordering.
	 *
	 * @return this <code>StringBuilder</code>
	 */
	StringBuilder& reverse();

	/**
	 * Convert this <code>StringBuilder</code> to a <code>String</code>. The
	 * String is composed of the characters currently in this StringBuilder. Note
	 * that the result is a copy, and that future modifications to this buffer
	 * do not affect the String.
	 *
	 * @return the characters in this StringBuilder
	 */
	String toString() const
	{
		return String(buffer,avail);
	}

	//set capacity to real size
	void trimToSize();
private:
	/**
	 * Predicate which determines if a substring of this matches another String
	 * starting at a specified offset for each String and continuing for a
	 * specified length. This is more efficient than creating a String to call
	 * indexOf on.
	 *
	 * @param toffset index to start comparison at for this String
	 * @param other non-null String to compare to region of this
	 * @return true if regions match, false otherwise
	 * @see #indexOf(String, unsigned int)
	 * @see #lastIndexOf(String, unsigned int)
	 * @see String#regionMatches(boolean, unsigned int, String, unsigned int, unsigned int)
	 */
	bool regionMatches(unsigned int toffset, const String& other) const;

	//check offset is out of bound
	void checkBound(unsigned int offset) const
	{
		if (offset >= avail)
			throwpe(OutOfBoundException(offset,avail));
	}
};

/*
StringBuilder sb(1024);
sb.append("I am ").append(5).append(" years old.");
sb.append("do you know? \n-response:").append(true).append('.');
sb.append(String("pi=")).append(3.1415926).append('\n');
String s=sb.substring(2,4);
sb.replace(0,4,"You are");
sb.deleteSub(8,9);
sb.insert(8,80);
printf(sb.toString().c_str());//*/

}//end of namespace blib