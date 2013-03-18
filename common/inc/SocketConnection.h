/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef _SOCKET_CONNECTION_H_
#define _SOCKET_CONNECTION_H_

#include <dlog.h>
#include <new>
#include <list>
#include <utility>
#include <SocketStream.h>
#include <PrivacyManagerTypes.h>
#include <iostream>
/*
 * This class implements interface for generic read and write from given socket.
 * It does not maintain socket descriptor, so any connecting and disconnecting should be
 */

/*
 * Throws SocketConnectionException when read/write will not succeed or if any bad allocation
 * exception occurs during read.
 */

class EXTERN_API SocketConnection
{

public:

	explicit SocketConnection(int socket_fd) : m_socketStream(socket_fd){
		LOGI("Created");
	}

	template<typename T, typename ...Args>
	void read(T* out, const Args&... args )
	{
		read(out);
		read(args...);
	}

	template<typename T>
	void read(T* out)
	{
		int length = 0;
		int ret = m_socketStream.readStream(sizeof(length), &length);
		char* pBuf = new char[length + 1];
		m_socketStream.readStream(length, pBuf);
		pBuf[length] = 0;

		out = T(pBuf);

		delete[] pBuf;
	}
	
	void read(bool* pB)
	{
		
		int length = 0;
		int ret = m_socketStream.readStream(sizeof(length), &length);
		if (ret < 0)
		{}
		char* pBuf = new char[length + 1];
		m_socketStream.readStream(length, pBuf);
		pBuf[length] = 0;

		*pB = * reinterpret_cast <bool* > (pBuf);

		delete[] pBuf;
	}

	void read(bool& b)
	{
		read(&b);
	}
	
	void read(int& i)
	{
		read(&i);
	}
	void read(int* pI)
	{
		int length = 0;
		int ret = m_socketStream.readStream(sizeof(length), &length);
		if (ret < 0)
		{}
		char* pBuf = new char[length + 1];
		m_socketStream.readStream(length, pBuf);
		pBuf[length] = 0;

		*pI = * reinterpret_cast <int* > (pBuf);

		delete[] pBuf;
	}

	void read(std::string* pStr)
	{
		int length = 0;
		int ret = m_socketStream.readStream(sizeof(length), &length);
		if (ret < 0)
		{}
		char* pBuf = new char[length + 1];
		m_socketStream.readStream(length, pBuf);
		pBuf[length] = 0;

		*pStr = std::string(pBuf);
		delete[] pBuf;
	}
	
	void read(std::string& str)
	{
		read(&str);
	}

	template < typename T >
	void read (std::list<T>& list) 
	{
		int length;
		read(length);
		for (int i =0; i < length; ++i) 
		{
			T obj;
			read (obj);
			list.push_back(obj);
		}
	}

	template < typename T >
	void read (std::list<T>* pList) 
	{
		read(*pList);
	}

	template < typename K, typename P >
	void read (std::pair<K, P>& pair) 
	{
		read( pair.first);
		read( pair.second);
	}

	template < typename K, typename P >
	void read (std::pair<K, P>* pPair) 
	{
		read( *pPair);
	}

	template<typename T, typename ...Args>
	void write(const T& in, const Args&... args)
	{
		write(in);
		write(args...);
	}

	void write(const std::string& in)
	{
		int length = in.size();
		m_socketStream.writeStream(sizeof(length), &length);
		m_socketStream.writeStream(length, in.c_str());
	}
	
	void write(const unsigned int& in)
	{
		int length = sizeof(in);
		m_socketStream.writeStream(sizeof(length), &length);
		m_socketStream.writeStream(length, &in);
	}

	void write(const int& in)
	{
		int length = sizeof(in);
		m_socketStream.writeStream(sizeof(length), &length);
		m_socketStream.writeStream(length, &in);
	}

	void write(const bool& in)
	{
		int length = sizeof(in);
		m_socketStream.writeStream(sizeof(length), &length);
		m_socketStream.writeStream(length, &in);
	}

	void write(const char*& in)
	{
		int length = strlen(in);
		m_socketStream.writeStream(sizeof(length), &length);
		m_socketStream.writeStream(length, in);
	}

	template<typename T, typename ...Args>
	void write(const T* in, const Args&... args)
	{
		write(in);
		write(args...);
	}

	template<typename K, typename T>
	void write(const std::pair<K, T> p)
	{
		write(p.first);
		write(p.second);
	}

	template<typename K, typename T>
	void write(const std::pair<K, T&> p) 
	{
		write(p.first);
		write(p.second);
	}

	template<typename K, typename T>
	void write(const std::pair<K, T&>* pPair) 
	{
		write(*pPair);
	}

	template<typename T>
	void write(const std::list <T> list) 
	{
		int length = list.size();
		write(length);
		for (typename std::list <T>::const_iterator iter = list.begin(); iter != list.end(); iter++)
			write(*iter);
	}

	template<typename T>
	void write(const std::list <T>* pList) 
	{
		write(*pList);
	}


private:
	SocketStream m_socketStream;
};

#endif // _SOCKET_CONNECTION_H_
