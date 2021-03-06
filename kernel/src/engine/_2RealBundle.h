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

#include "helpers/_2RealHandleable.h"
#include "helpers/_2RealNonCopyable.h"
#include "helpers/_2RealIdentifiable.h"
#include "app/_2RealBundleInfo.h"
#include "app/_2RealBundleHandle.h"

#include <map>
#include <string>

namespace _2Real
{

	namespace app
	{
		class BundleHandle;
		class BlockHandle;
		class ContextBlockHandle;
	}

	class BundleManager;
	template< typename T >
	class FunctionBlock;

	class Bundle : private NonCopyable< Bundle >, private Identifiable< Bundle >, private Handleable< Bundle, app::BundleHandle >
	{

	public:

		Bundle( app::BundleInfo const& info, BundleManager &bundleManager );
		~Bundle();

		using Handleable< Bundle, app::BundleHandle >::getHandle;
		using Handleable< Bundle, app::BundleHandle >::registerHandle;
		using Handleable< Bundle, app::BundleHandle >::unregisterHandle;

		typedef std::multimap< std::string, FunctionBlock< app::BlockHandle > * >					BlockInstances;
		typedef std::multimap< std::string, FunctionBlock< app::BlockHandle > * >::iterator			BlockInstanceIterator;
		typedef std::multimap< std::string, FunctionBlock< app::BlockHandle > * >::const_iterator	BlockInstanceConstIterator;

		using Identifiable< Bundle >::getIds;
		using Identifiable< Bundle >::getName;

		std::string const& getAbsPath() const;

		void						clear();
		void						unload( const long timeout );
		app::BundleInfo const&		getBundleInfo() const;
		app::ContextBlockHandle &	getContextBlockHandle() const;
		bool						hasContext() const;
		void						setContextBlock( FunctionBlock< app::ContextBlockHandle > &context );
		FunctionBlock< app::BlockHandle > &						createBlockInstance( std::string const& blockName );
		void						removeBlockInstance(  FunctionBlock< app::BlockHandle > &block );
		bool canCreate( std::string const& blockName ) const;
		BlockInstances &							getBlockInstances( BundleManager const& m );
		FunctionBlock< app::ContextBlockHandle > &	getContextBlock( BundleManager const& m );
		void contextBlockRemoved();

	private:

		unsigned int				getBlockInstanceCount( std::string const& blockName ) const;

		BundleManager									&m_BundleManager;
		FunctionBlock< app::ContextBlockHandle >		*m_ContextBlock;
		BlockInstances									m_BlockInstances;
		app::BundleInfo									m_BundleInfo;

	};

}