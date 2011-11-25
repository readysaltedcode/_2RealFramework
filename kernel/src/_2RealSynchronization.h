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

#include "_2RealRunnableGraph.h"

#include "Poco/Threadpool.h"

namespace _2Real
{

	class SystemGraph;

	class Synchronization : public RunnableGraph
	{

	public:

		Synchronization(Identifier const& id, SystemGraph *const system);
		virtual ~Synchronization();

		virtual void run();
		virtual void update();
		virtual void shutdown();
		virtual void checkConfiguration();

		virtual void insertChild(Runnable *const child, unsigned int const& index);
		virtual void removeChild(unsigned int const& id);

		virtual Runnable *const getChild(unsigned int const& id);
		virtual Runnable *const findChild(unsigned int const& id);
		virtual Runnable const *const findChild(unsigned int const& id) const;

	private:

		Poco::ThreadPool							m_ThreadPool;

	};

}