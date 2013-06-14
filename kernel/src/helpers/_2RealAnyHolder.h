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

#include "datatypes/_2RealTypeComparisons.h"
#include "datatypes/_2RealTypeStreamOperators.h"

#ifdef _UNIX
	#include <typeinfo>
#else
	#include <typeinfo.h>
#endif

#include <string>
#include <sstream>

namespace _2Real
{
	class AbstractAnyHolder
	{

	public:

		virtual ~AbstractAnyHolder() {}
		virtual const std::string getTypename() const = 0;
		virtual std::type_info const& getTypeinfo() const = 0;
		virtual void writeTo( std::ostream &out ) const = 0;
		virtual void readFrom( std::istream &in ) = 0;
		virtual bool isEqualTo( AbstractAnyHolder const& other ) const = 0;
		virtual bool isLessThan( AbstractAnyHolder const& other ) const = 0;

		virtual AbstractAnyHolder* clone() const = 0;
		virtual AbstractAnyHolder* create() const = 0;
		virtual void set( AbstractAnyHolder const& other ) = 0;

		template< typename TType >
		TType & extract()
		{
			AnyHolder< TType > *ptr = dynamic_cast< AnyHolder< TType > * >( this );
			if ( ptr )
			{
				return ptr->mData;
			}
			else
			{
				TypeDescriptor *t = createTypeDescriptor< TType >();
				std::ostringstream msg;
				msg << "type of data does not match template parameter" << std::endl;
				throw TypeMismatchException( msg.str() );
			}
		}

		template< typename TType >
		TType const& extract() const
		{
			AnyHolder< TType > const* ptr = dynamic_cast< AnyHolder< TType > const* >( this );
			if ( ptr )
			{
				return ptr->mData;
			}
			else
			{
				TypeDescriptor *t = createTypeDescriptor< TType >();
				std::ostringstream msg;
				msg << "type of data does not match template parameter" << std::endl;
				throw TypeMismatchException( msg.str() );
			}
		}
	};

	template< typename TData >
	class AnyHolder : public AbstractAnyHolder
	{

	public:

		AnyHolder();
		explicit AnyHolder( TData const& value );

		const std::string getTypename() const;
		std::type_info const& getTypeinfo() const;
		AbstractAnyHolder * create() const;
		AbstractAnyHolder * clone() const;
		void set( AbstractAnyHolder const& other );

		void writeTo( std::ostream &out ) const;
		void readFrom( std::istream &in );

		bool isEqualTo( AbstractAnyHolder const& other ) const;
		bool isLessThan( AbstractAnyHolder const& other ) const;

		TData					mData;

	private:

		AnyHolder( AnyHolder< TData > const& src );
		AnyHolder& operator=( AnyHolder< TData > const& src );

	};

	template< typename TData >
	AnyHolder< TData >::AnyHolder() :
		mData()
	{
	}

	template< typename TData >
	AnyHolder< TData >::AnyHolder( TData const& value ) :
		mData( value )
	{
	}

	//template< typename TData >
	//AnyHolder< TData >::AnyHolder( AnyHolder< TData > const& src ) :
	//	m_Data( src.m_Data )
	//{
	//}

	//template< typename TData >
	//AnyHolder< TData >& AnyHolder< TData >::operator=( AnyHolder< TData > const& src )
	//{
	//	if ( this == &src )
	//	{
	//		return *this;
	//	}

	//	m_Data = src.m_Data;

	//	return *this;
	//}

	template< typename TData >
	void AnyHolder< TData >::set( AbstractAnyHolder const& other )
	{
		try
		{
			AnyHolder< TData > const& o = dynamic_cast< AnyHolder< TData > const& >( other );
			mData = o.mData;
		}
		catch ( std::bad_cast const& e )
		{
		}
	}

	template< typename TData >
	std::type_info const& AnyHolder< TData >::getTypeinfo() const
	{
		return typeid( TData );
	}

	template< typename TData >
	const std::string AnyHolder< TData >::getTypename() const
	{
		return typeid( TData ).name();
	}

	template< typename TData >
	bool AnyHolder< TData >::isEqualTo( AbstractAnyHolder const& other ) const
	{
		if ( other.getTypename() == this->getTypename() )
		{
			AnyHolder< TData > const& holder = dynamic_cast< AnyHolder< TData > const& >( other );
			return mData == holder.mData;
		}
		else
		{
			return false;
		}
	}

	template< typename TData >
	bool AnyHolder< TData >::isLessThan( AbstractAnyHolder const& other ) const
	{
		if ( other.getTypename() == this->getTypename() )
		{
			AnyHolder< TData > const& holder = dynamic_cast< AnyHolder< TData > const& >( other );
			return isLess( mData, holder.mData );
		}
		else
		{
			std::ostringstream msg;
			msg << "type of data " << other.getTypename() << " does not match type " << this->getTypename() << std::endl;
			throw TypeMismatchException( msg.str() );
		}
	}

	template< typename TData >
	void AnyHolder< TData >::writeTo( std::ostream &out ) const
	{
		_2Real::writeTo( out, mData );
	}

	template< typename TData >
	void AnyHolder< TData >::readFrom( std::istream &in )
	{
		_2Real::readFrom( in, mData );
	}

	template< typename TData >
	AbstractAnyHolder * AnyHolder< TData >::create() const
	{
		return new AnyHolder< TData >();
	}

	template< typename TData >
	AbstractAnyHolder * AnyHolder< TData >::clone() const
	{
		return new AnyHolder< TData >( mData );
	}
}
