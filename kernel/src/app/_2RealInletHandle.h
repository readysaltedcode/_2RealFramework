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

#include "helpers/_2RealAny.h"
#include "helpers/_2RealOptions.h"
#include "engine/_2RealInletPolicy.h"

namespace _2Real
{
	class AbstractInletIO;
	class AnyOptionSet;

	namespace app
	{
		class AppData;
		class OutletHandle;
		class BlockHandle;

		class InletHandle
		{

		public:

			InletHandle();
			InletHandle( AbstractInletIO &inletIO );
			~InletHandle();
			InletHandle( InletHandle const& other );
			InletHandle& operator=( InletHandle const& other );

			std::string const&	getName() const;
			const std::string	getLongTypename() const;
			std::string const&	getTypename() const;

			bool isValid() const;
			void invalidate();
			bool operator==( InletHandle const& other ) const;
			bool operator!=( InletHandle const& other ) const;
			bool operator<( InletHandle const& other ) const;
			bool operator<=( InletHandle const& other ) const;
			bool operator>( InletHandle const& other ) const;
			bool operator>=( InletHandle const& other ) const;

			struct InletState
			{
				std::string defaultValue;
				std::string currentValue;
				std::string updatePolicy;
				std::string bufferSize;
			};

			InletState getCurrentState() const;

			void setUpdatePolicy( InletPolicy const& policy );

			bool link( OutletHandle &outletHandle );
			bool linkWithConversion( OutletHandle &outletHandle );
			void unlinkFrom( OutletHandle &outletHandle );

			// if the inlet is linked, the value might be overwritten of course
			template< typename TData >
			void setValue( TData const& value )
			{
				setValue( Any( value ) );
			}

			template< typename TData >
			void setDefaultValue( TData const& value )
			{
				setDefaultValue( Any( value ) );
			}

			void setValueToString( std::string const& value );
			void setDefaultValueToString( std::string const& value );

			// returns the inlet's most recent input data
			// updates right before an update() -> stays the same until next update()
			AppData				getCurrentInput() const;

			void				setBufferSize( const unsigned int size );

			template< typename TData >
			std::set< Option< TData > > getOptionMapping() const
			{
				AnyOptionSet const& anyOptions = this->getOptionSet();
				return anyOptions.extract< TData >();
			}

			bool isMultiInlet() const;
			unsigned int getSize() const;
			InletHandle operator[]( const unsigned int index );

			InletHandle add();
			void remove( InletHandle &handle );

			// new 06/05/13
			BlockHandle getOwningBlock();

		private:

			friend class OutletHandle;

			AnyOptionSet const& getOptionSet() const;

			void				setValue( Any const& data );
			void				setDefaultValue( Any const& data );
			AbstractInletIO		*m_InletIO;

		};
	}
}
