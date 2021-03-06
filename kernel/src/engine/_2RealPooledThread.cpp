/*
	CADET - Center for Advances in Digital Entertainment Technologies
	Copyright 2011 Fachhochschule Salzburg GmbH

		http://www.cadet.at

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#include "engine/_2RealPooledThread.h"
#include "engine/_2RealFunctionBlockStateManager.h"
#include "engine/_2RealThreadPool.h"

namespace _2Real
{

	PooledThread::PooledThread( ThreadPool &pool, unsigned int stackSize ) :
		m_Request( nullptr ),
		m_IsIdle( true ),
		m_Thread( "idle thread" ),
		m_ThreadPool( &pool ),
		m_TargetReady( true ),
		m_TargetCompleted( true ),
		m_ThreadStarted( true ),
		m_ThreadStopped( true )
	{
		m_Thread.setStackSize(stackSize);
		m_Thread.start(*this);
		m_ThreadStarted.wait();
	}

	const bool PooledThread::operator<(PooledThread const& rhs) const
	{
		return m_Thread.id() < rhs.m_Thread.id();
	}

	const bool PooledThread::isIdle() const
	{
		Poco::ScopedLock< Poco::FastMutex > lock(m_Mutex);
		return m_IsIdle;
	}

	const bool PooledThread::join()
	{
		m_Mutex.lock();
		if (m_IsIdle)
		{
			m_Request = nullptr;
			m_IsIdle = false;
			m_TargetCompleted.reset();
			m_TargetReady.set();
			m_Mutex.unlock();
			return m_ThreadStopped.tryWait(500);
		}
		else
		{
			m_Mutex.unlock();
			return false;
		}
	}

	void PooledThread::run( Poco::Thread::Priority const& priority, ThreadExecRequest &request )
	{
		Poco::ScopedLock< Poco::FastMutex > lock( m_Mutex );

		m_Thread.setName( request.block.getName() );
		m_Thread.setPriority( priority );
		m_Request = &request;

		m_TargetReady.set();
	}

	void PooledThread::reactivate()
	{
		Poco::ScopedLock< Poco::FastMutex > lock(m_Mutex);

		m_IsIdle = false;
		m_TargetCompleted.reset();
	}

	void PooledThread::run()
	{
		m_ThreadStarted.set();

		while (true)
		{
			m_TargetReady.wait();

			m_Mutex.lock();
			if ( m_Request )										// if the request is ever null & targetReady is signalled, the thread will kill itself
			{
				m_Mutex.unlock();

				FunctionBlockStateManager &mgr = m_Request->block;
				FunctionToExecute func = m_Request->function;
				( mgr.*func )();									// function pointer syntax sucks :/

				Poco::ScopedLock< Poco::FastMutex > lock(m_Mutex);

				ThreadExecRequest *tmp = m_Request;
				m_Request = nullptr;
				m_IsIdle = true;

				m_TargetCompleted.set();
				Poco::ThreadLocalStorage::clear();
				m_Thread.setName( "unused thread" );
				m_Thread.setPriority( Poco::Thread::PRIO_NORMAL );

				if ( tmp->event != nullptr ) tmp->event->set();		// maybe i could do this immediately?
				delete tmp;
			}
			else
			{
				m_Mutex.unlock();
				break;
			}
		}

		m_ThreadStopped.set();
	}

}