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

#include "engine/_2RealMetainfo.h"
#include "bundle/_2RealCreationPolicy.h"
#include "helpers/_2RealHelpers.h"
#include "helpers/_2RealException.h"
#include "engine/_2RealBlockMetadata.h"
#include "bundle/_2RealBlockMetainfo.h"
#include "bundle/_2RealContextBlockMetainfo.h"
#include "engine/_2RealParameterMetadata.h"

#include <sstream>
#include <assert.h>

using std::string;
using std::ostringstream;
using std::make_pair;
using std::map;

namespace _2Real
{

	Metainfo::Metainfo() :
		m_HasContext( false )
	{
	}

	Metainfo::~Metainfo()
	{
		for ( BlockInfoIterator it = m_BlockInfos.begin(); it != m_BlockInfos.end(); ++it )
		{
			safeDelete( it->second.ctor );
			safeDelete( it->second.data );
			safeDelete( it->second.meta );
		}

		safeDelete( m_ContextInfo.ctor );
		safeDelete( m_ContextInfo.data );
		safeDelete( m_ContextInfo.meta );
	}

	bool Metainfo::hasContext() const
	{
		return m_HasContext;
	}

	BundleMetadata const& Metainfo::getBundleData() const
	{
		return m_BundleData;
	}

	void Metainfo::setAuthor( string const& author )
	{
		m_BundleData.setAuthor( author );
	}

	void Metainfo::setDescription( string const& description )
	{
		m_BundleData.setDescription( description );
	}

	void Metainfo::setCategory( string const& category )
	{
		m_BundleData.setCategory( category );
	}

	void Metainfo::setVersion( Version const& version )
	{
		m_BundleData.setVersion( version );
	}

	void Metainfo::setContact( string const& contact )
	{
		m_BundleData.setContact( contact );
	}

	void Metainfo::setInstallDirectory( string const& path )
	{
		m_BundleData.setInstallDirectory( path );
	}

	void Metainfo::setName( string const& name )
	{
		m_BundleData.setName( name );
	}

	bundle::ContextBlockMetainfo & Metainfo::setContextBlockCreator( bundle::AbstractBlockCreator &obj )
	{
		if ( m_HasContext )
		{
			ostringstream msg;
			msg << "context block is already defined in bundle " << m_BundleData.getName();
			throw AlreadyExistsException( msg.str() );
		}

		m_ContextInfo.ctor = &obj;
		m_ContextInfo.data = new BlockMetadata( "contextblock" );
		m_ContextInfo.data->setDescription( "context block" );
		m_ContextInfo.meta = new bundle::ContextBlockMetainfo( *m_ContextInfo.data );

		m_HasContext = true;

		return *m_ContextInfo.meta;
	}

	bundle::BlockMetainfo & Metainfo::setBlockCreator( string const& blockName, bundle::AbstractBlockCreator &obj )
	{
		for ( BlockInfoConstIterator it = m_BlockInfos.begin(); it != m_BlockInfos.end(); ++it )
		{
			if ( toLower( it->first ) == toLower( blockName ) )
			{
				ostringstream msg;
				msg << "block " << blockName << " is already defined in bundle " << m_BundleData.getName();
				throw AlreadyExistsException( msg.str() );
			}
		}

		m_BlockInfos[ blockName ].ctor = &obj;
		BlockMetadata *data = new BlockMetadata( blockName );
		m_BlockInfos[ blockName ].data = data;
		bundle::BlockMetainfo *meta = new bundle::BlockMetainfo( *data );
		m_BlockInfos[ blockName ].meta = meta;

		return *meta;
	}

	bundle::Block & Metainfo::createContextBlock() const
	{
#ifdef _DEBUG
		if ( m_ContextInfo.ctor == nullptr )
		{
			assert( NULL );
		}
#endif

		return m_ContextInfo.ctor->create( nullptr );
	}

	bundle::Block & Metainfo::createBlock( string const& blockName ) const
	{
		bundle::ContextBlock *context = nullptr;

		if ( m_HasContext )
		{
			context = dynamic_cast< bundle::ContextBlock * > ( &( m_ContextInfo.ctor->create( nullptr ) ) );
		}

		//BlockInfoConstIterator it = m_BlockInfos.find( blockName );
		//if ( it != m_BlockInfos.end() )
		//{
		//	return it->second.ctor->create( context );
		//}
		//else
		//{
		//	ostringstream msg;
		//	msg << "block " << blockName << " is not exported by " << m_BundleData.getName();
		//	throw NotFoundException( msg.str() );
		//}

		for ( BlockInfoConstIterator it = m_BlockInfos.begin(); it != m_BlockInfos.end(); ++it )
		{
			if ( toLower( it->first ) == toLower( blockName ) )
			{
				return it->second.ctor->create( context );
			}
		}

		ostringstream msg;
		msg << "block " << blockName << " is not exported by " << m_BundleData.getName();
		throw NotFoundException( msg.str() );
	}

	void Metainfo::removeContextBlock()
	{
		m_ContextInfo.ctor->reset();
	}

	void Metainfo::cleanup()
	{
		for ( BlockInfoIterator it = m_BlockInfos.begin(); it != m_BlockInfos.end(); ++it )
		{
			m_BundleData.addBlockData( *it->second.data );
		}

		if ( m_HasContext )
		{
			m_BundleData.addBlockData( *m_ContextInfo.data );
		}
	}

}