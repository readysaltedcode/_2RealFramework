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

#include "helpers/_2RealException.h"

#include <string>
#include <assert.h>

namespace _2Real
{

	template< typename TData >
	inline bool isEqual( TData const& v1, TData const& v2 )
	{
#ifdef _DEBUG
		assert( NULL );
#endif
		return false;
	}

	template < >
	inline bool isEqual< char >( char const& v1, char const& v2 )
	{
		return ( v1 == v2 );
	}

	template < >
	inline bool isEqual< unsigned char >( unsigned char const& v1, unsigned char const& v2 )
	{
		return ( v1 == v2 );
	}

	template < >
	inline bool isEqual< short >( short const& v1, short const& v2 )
	{
		return ( v1 == v2 );
	}

	template < >
	inline bool isEqual< unsigned short >( unsigned short const& v1, unsigned short const& v2 )
	{
		return ( v1 == v2 );
	}

	template < >
	inline bool isEqual< int >( int const& v1, int const& v2 )
	{
		return ( v1 == v2 );
	}

	template < >
	inline bool isEqual< unsigned int >( unsigned int const& v1, unsigned int const& v2 )
	{
		return ( v1 == v2 );
	}

	template < >
	inline bool isEqual< long >( long const& v1, long const& v2 )
	{
		return ( v1 == v2 );
	}

	template < >
	inline bool isEqual< unsigned long >( unsigned long const& v1, unsigned long const& v2 )
	{
		return ( v1 == v2 );
	}

	template < >
	inline bool isEqual< bool >( bool const& v1, bool const& v2 )
	{
		return ( v1 == v2 );
	}

	template < >
	inline bool isEqual< std::string >( std::string const& v1, std::string const& v2 )
	{
		return ( v1 == v2 );
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	template< typename TData >
	inline bool isLess( TData const& v1, TData const& v2 )
	{
#ifdef _DEBUG
		assert( NULL );
#endif
		return false;
	}

	template < >
	inline bool isLess< char >( char const& v1, char const& v2 )
	{
		return ( v1 < v2 );
	}

	template < >
	inline bool isLess< unsigned char >( unsigned char const& v1, unsigned char const& v2 )
	{
		return ( v1 < v2 );
	}

	template < >
	inline bool isLess< short >( short const& v1, short const& v2 )
	{
		return ( v1 < v2 );
	}

	template < >
	inline bool isLess< unsigned short >( unsigned short const& v1, unsigned short const& v2 )
	{
		return ( v1 < v2 );
	}

	template < >
	inline bool isLess< int >( int const& v1, int const& v2 )
	{
		return ( v1 < v2 );
	}

	template < >
	inline bool isLess< unsigned int >( unsigned int const& v1, unsigned int const& v2 )
	{
		return ( v1 < v2 );
	}

	template < >
	inline bool isLess< long >( long const& v1, long const& v2 )
	{
		return ( v1 < v2 );
	}

	template < >
	inline bool isLess< unsigned long >( unsigned long const& v1, unsigned long const& v2 )
	{
		return ( v1 < v2 );
	}

	template < >
	inline bool isLess< bool >( bool const& v1, bool const& v2 )
	{
		return ( v1 < v2 );
	}

	template < >
	inline bool isLess< std::string >( std::string const& v1, std::string const& v2 )
	{
		return ( v1 < v2 );
	}
}