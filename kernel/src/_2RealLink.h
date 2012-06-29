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

#include <set>

namespace _2Real
{

	class Parameter;
	class Inlet;
	class Outlet;

	class AbstractLink
	{

	public:

		AbstractLink( Parameter &p1, Parameter &p2 );
		virtual ~AbstractLink() {}
		bool operator<( AbstractLink const& other );

		virtual void activate() = 0;
		virtual void deactivate() = 0;

	private:

		Parameter	&m_Link1;
		Parameter	&m_Link2;

	};

	struct LinkCmp
	{
		bool operator()( AbstractLink *l1, AbstractLink *l2 )
		{
			return ( *l1 < *l2 );
		}
	};

	typedef std::set< AbstractLink *, LinkCmp >		LinkSet;

	class IOLink : public AbstractLink
	{

	public:

		IOLink( Inlet &inlet, Outlet &outlet );

		void activate();
		void deactivate();

	private:

		Inlet		&m_Inlet;
		Outlet		&m_Outlet;

	};

}