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

#include "_2RealUpdatePolicyHandle.h"
#include "_2RealUpdatePolicy.h"
#include "_2RealException.h"

using std::string;

namespace _2Real
{
	namespace app
	{
		UpdatePolicyHandle::UpdatePolicyHandle() :
			m_Policy( nullptr )
		{
		}

		UpdatePolicyHandle::UpdatePolicyHandle( UpdatePolicy &policy ) :
			m_Policy( &policy )
		{
		}

		void UpdatePolicyHandle::updateWithFixedRate( const double updatesPerSecond )
		{
			if ( m_Policy == nullptr )
			{
				throw UninitializedHandleException( "update policy handle not initialized" );
			}

			if ( updatesPerSecond == 0. )
			{
				m_Policy->setNewUpdateTime( -1 );
			}

			//timeslice is in microseconds, since that's the resolution of a poco::timestamp
			//however, the maximum timer update rate obtainable is ~ 2 milliseconds,
			//and that's with a totally empty system -> silly update rates greater than 60 hertz won't be met, probably
			double micros = 1000000/updatesPerSecond;
			m_Policy->setNewUpdateTime( static_cast< long >(micros) );
		}

		void UpdatePolicyHandle::updateWhenAllInletDataNew()
		{
			if ( m_Policy == nullptr )
			{
				throw UninitializedHandleException( "update policy handle not initialized" );
			}

			m_Policy->setNewInletDefaultPolicy( UpdatePolicy::InletTriggerCtor( new InletTriggerCreator< NewerTimestamp >() ) );
		}

		void UpdatePolicyHandle::updateWhenInletDataNew( string const& inletName )
		{
			if ( m_Policy == nullptr )
			{
				throw UninitializedHandleException( "update policy handle not initialized" );
			}

			m_Policy->setNewInletPolicy( inletName, UpdatePolicy::InletTriggerCtor( new InletTriggerCreator< NewerTimestamp >() ) );
		}

		void UpdatePolicyHandle::updateWhenAllInletDataValid()
		{
			if ( m_Policy == nullptr )
			{
				throw UninitializedHandleException( "update policy handle not initialized" );
			}

			m_Policy->setNewInletDefaultPolicy( UpdatePolicy::InletTriggerCtor( new InletTriggerCreator< ValidData >() ) );
		}

		void UpdatePolicyHandle::updateWhenInletDataValid( string const& inletName )
		{
			if ( m_Policy == nullptr )
			{
				throw UninitializedHandleException( "update policy handle not initialized" );
			}

			m_Policy->setNewInletPolicy( inletName, UpdatePolicy::InletTriggerCtor( new InletTriggerCreator< ValidData >() ) );
		}
	}
}