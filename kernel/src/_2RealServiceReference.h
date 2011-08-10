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

/*
	used in service factory;
	stores info about services that can be created:

	- human readable name of service
	- pointer to registering plugin
	- pointer to factory function
	- if service is a singleton
	- # of times a service obj was created
*/

#pragma once

#include "_2RealTypedefs.h"

namespace _2Real
{
	class ServiceReference
	{

	public:

		ServiceReference(Plugin const *const _pluginPtr, UserServiceCreator _creator, bool const& _singleton);
		
		bool const& isSingleton() const;
		const bool canCreate() const;
		unsigned int const& creationCount() const;
		
		UserServicePtr create();

	private:

		Plugin					*m_PluginPtr;
		UserServiceCreator		m_ServiceCreator;
		unsigned int			m_iNrOfCreations;
		bool					m_bIsSingleton;

	};
}