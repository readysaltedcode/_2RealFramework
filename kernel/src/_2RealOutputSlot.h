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

#include "_2RealParameter.h"
#include "_2RealData.h"
#include "_2RealCallbacks.h"

#include "Poco/Mutex.h"
#include "Poco/BasicEvent.h"
#include "Poco/Timestamp.h"

namespace _2Real
{

	class InputSlot;
	class Timer;

	class OutputSlot : public Parameter
	{

	public:

		OutputSlot(ParameterMetadata const& metadata, Poco::Timestamp const& timer);

		EngineData					getData();
		void						update();
		void						createNewDataItem();

		void						registerCallback( DataFunctionCallback &callback );
		void						unregisterCallback( DataFunctionCallback &callback );
		void						registerCallback(AbstractDataCallbackHandler &callback);
		void						unregisterCallback(AbstractDataCallbackHandler &callback);
		void						addListener( InputSlot &slot );
		void						removeListener( InputSlot &slot );

		void						resetLinks();

	private:

		const bool						isLinkedWith(InputSlot &inlet) const;

		Poco::Timestamp					const& m_SystemTime;
		mutable Poco::FastMutex			m_Mutex;
		Data							m_CurrentData;
		EngineData						m_WriteData;
		Poco::BasicEvent< Data >		m_Event;
		std::list< InputSlot * >		m_LinkedInlets;
		std::map< long, EngineData >	m_DataItems;
		bool							m_DiscardLast;

	};

}