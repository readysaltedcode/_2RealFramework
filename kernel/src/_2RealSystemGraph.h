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

#pragma once

#include "_2RealGraph.h"
#include "_2RealEntity.h"
#include "_2RealExceptionHandler.h"
#include "_2RealThreadPool.h"

#include <map>
#include <list>

namespace _2Real
{
	class SystemGraph : public Graph, public Entity
	{

	public:

		SystemGraph(Identifier const& id);
		virtual ~SystemGraph();

		void startChild(unsigned int const& id);
		void stopChild(unsigned int const& id);
		void stopAll();
		void startAll();

		void insertChild(Runnable *const child, unsigned int const& index);
		void removeChild(unsigned int const& id);

		void handleException(Runnable &child, Exception &exception);

		void registerExceptionCallback(ExceptionCallback callback);
		void unregisterExceptionCallback(ExceptionCallback callback);

	private:

		ThreadPool			m_Threads;
		ExceptionHandler	m_ExceptionHandler;

	};

}