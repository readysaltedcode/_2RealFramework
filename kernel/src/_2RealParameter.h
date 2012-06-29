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

#include "_2RealParam.h"
#include "_2RealTimestampedData.h"
#include "_2RealEngineData.h"
#include "_2RealPoco.h"

namespace _2Real
{

	class Parameter : public Param
	{

	public:

		Parameter( AbstractUberBlock &owner, std::string const& name, std::string const& longTypename, std::string const& typeName );

		using Param::getTypename;
		using Param::getLongTypename;
		using Param::getName;
		using Param::getOwningUberBlock;

		void				setData( TimestampedData const& data );
		void				synchronize();
		EngineData			getData() const; // must return a copy, b/c could change anytime

	private:

		mutable Poco::FastMutex		m_DataAccess;
		TimestampedData				m_Data;
		TimestampedData				m_WriteData;

	};

}