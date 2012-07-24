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

#include "engine/_2RealEngineImpl.h"
#include "engine/_2RealTypetable.h"
#include "engine/_2RealBundleManager.h"
#include "engine/_2RealBundle.h"
#include "engine/_2RealTimer.h"
#include "engine/_2RealThreadPool.h"
#include "engine/_2RealLogger.h"
#include "engine/_2RealSystem.h"
#include "engine/_2RealFunctionBlock.h"
#include "helpers/_2RealCallback.h"
#include "helpers/_2RealEvent.h"
#include "helpers/_2RealSingletonHolder.h"
#include "helpers/_2RealHelpers.h"

#include "datatypes/_2RealImage.h"

#include <sstream>
#include <iostream>

#ifdef _2REAL_WINDOWS
	#ifndef _DEBUG
		#define shared_library_suffix "_32.dll"
	#else
		#define shared_library_suffix "_32d.dll"
	#endif
#elif _2REAL_UNIX
	#ifndef _DEBUG
		#define shared_library_suffix ".so"
	#else
		#define shared_library_suffix "_d.so"
	#endif
#elif _2REAL_MAC
	#ifndef _DEBUG
		#define shared_library_suffix ".dylib"
	#else
		#define shared_library_suffix "_d.dylib"
	#endif
#endif

using std::string;

namespace _2Real
{

	EngineImpl & EngineImpl::instance()
	{
		static SingletonHolder< EngineImpl > holder;
		return holder.instance();
	}

	EngineImpl::EngineImpl() :
		m_Logger( new Logger( "EngineLog.txt" ) ),
		m_Timer( new Timer( *m_Logger ) ),
		m_ThreadPool( new ThreadPool( *this, 15, 0, "2Real threadpool" ) ),
		m_Typetable( new Typetable() ),
		m_BundleManager( new BundleManager( *this ) ),
		m_System( new System( *m_Logger ) )
	{
		m_Typetable->registerType< char >( "char");
		m_Typetable->registerType< unsigned char >( "unsigned char" );
		m_Typetable->registerType< short >( "short");
		m_Typetable->registerType< unsigned short >( "unsigned short" );
		m_Typetable->registerType< int >( "int" );
		m_Typetable->registerType< unsigned int >( "unsigned int" );
		m_Typetable->registerType< long >( "long" );
		m_Typetable->registerType< unsigned long >( "unsigned long" );
		m_Typetable->registerType< float >( "float" );
		m_Typetable->registerType< double >( "double" );
		m_Typetable->registerType< bool >( "bool" );
		m_Typetable->registerType< std::string >( "string" );

		m_Typetable->registerType< ImageSource >( "image_source" );

		m_Timestamp.update();
	}

	EngineImpl::~EngineImpl()
	{
		try
		{
			clearFully();
			delete m_System;
			delete m_BundleManager;
			m_ThreadPool->clear();
			delete m_ThreadPool;
			delete m_Typetable;
			m_Logger->stop();
			delete m_Logger;
			delete m_Timer;
		}
		catch ( std::exception &e )
		{
			std::cout << e.what() << std::endl;
		}
	}

	void EngineImpl::clearFully()
	{
		for ( EngineImpl::LinkIterator it = m_Links.begin(); it != m_Links.end(); ++it )
		{
			( *it )->deactivate();
			delete *it;
		}
		m_Links.clear();
		m_System->clearAll();
		m_BundleManager->clear();
	}

	void EngineImpl::clearBlockInstances()
	{
		for ( EngineImpl::LinkIterator it = m_Links.begin(); it != m_Links.end(); ++it )
		{
			( *it )->deactivate();
			delete *it;
		}
		m_Links.clear();
		m_System->clearBlockInstances();
	}

	void EngineImpl::addBlock( FunctionBlock< app::BlockHandle > &block )
	{
		m_System->addBlock( block );
	}

	void EngineImpl::removeBlock( FunctionBlock< app::BlockHandle > &block, const long timeout )
	{
		m_System->removeBlock( block, timeout );
	}

	void EngineImpl::addBlock( FunctionBlock< app::ContextBlockHandle > &block )
	{
		m_System->addBlock( block );
	}

	void EngineImpl::removeBlock( FunctionBlock< app::ContextBlockHandle > &block, const long timeout )
	{
		m_System->removeBlock( block, timeout );
	}

	const long EngineImpl::getElapsedTime() const
	{
		return static_cast< long >( m_Timestamp.elapsed() );
	}

	Timer & EngineImpl::getTimer()
	{
		return *m_Timer;
	}

	Logger & EngineImpl::getLogger()
	{
		return *m_Logger;
	}

	Typetable const& EngineImpl::getTypetable() const
	{
		return *m_Typetable;
	}

	ThreadPool & EngineImpl::getThreadPool()
	{
		return *m_ThreadPool;
	}

	void EngineImpl::setBaseDirectory( string const& directory )
	{
		m_BundleManager->setBaseDirectory( directory );
	}

	app::BundleHandle & EngineImpl::loadLibrary( string const& libraryPath )
	{
		string path = libraryPath;

		if ( path.find( shared_library_suffix ) == string::npos )
		{
			path.append( shared_library_suffix );
		}
		return m_BundleManager->loadLibrary( path ).getHandle();
	}

	app::BundleHandle & EngineImpl::findBundleByName( string const& name ) const
	{
		return m_BundleManager->findBundleByName( name ).getHandle();
	}

	app::BundleHandle & EngineImpl::findBundleByPath( string const& libraryPath ) const
	{
		return m_BundleManager->findBundleByPath( libraryPath ).getHandle();
	}

	void EngineImpl::registerToException( app::BlockExcCallback &callback )
	{
		m_BlockExceptionEvent.addListener( callback );
	}

	void EngineImpl::unregisterFromException( app::ContextBlockExcCallback &callback )
	{
		m_ContextBlockExceptionEvent.removeListener( callback );
	}

	void EngineImpl::registerToException( app::ContextBlockExcCallback &callback )
	{
		m_ContextBlockExceptionEvent.addListener( callback );
	}

	void EngineImpl::unregisterFromException( app::BlockExcCallback &callback )
	{
		m_BlockExceptionEvent.removeListener( callback );
	}

	void EngineImpl::handleException( app::BlockHandle &block, Exception const& exception ) const
	{
		m_BlockExceptionEvent.notify( make_pair( exception, block ) );
	}

	void EngineImpl::handleException( app::ContextBlockHandle &block, Exception const& exception ) const
	{
		m_ContextBlockExceptionEvent.notify( make_pair( exception, block ) );
	}

	EngineImpl::Links const& EngineImpl::getCurrentLinks() const
	{
		return m_Links;
	}

	EngineImpl::BlockInstances EngineImpl::getCurrentBlockInstances() const
	{
		System::Blocks const& blocks = m_System->getBlockInstances();
		EngineImpl::BlockInstances result;

		for ( System::BlockConstIterator it = blocks.begin(); it != blocks.end(); ++it )
		{
			FunctionBlock< app::BlockHandle > *instance = static_cast< FunctionBlock< app::BlockHandle > * >( *it );
			result.push_back( instance );
		}
		return result;
	}

	EngineImpl::Bundles const& EngineImpl::getCurrentBundles() const
	{
		return m_BundleManager->getBundles();
	}

	void EngineImpl::createLink( InletIO &inlet, OutletIO &outlet )
	{
		IOLink *link = new IOLink( inlet, outlet );
		LinkIterator it = m_Links.find( link );
		if ( it == m_Links.end() )
		{
			link->activate();
			m_Links.insert( link );
		}
		else
		{
			delete link;
		}
	}

	void EngineImpl::destroyLink( InletIO &inlet, OutletIO &outlet )
	{
		IOLink *link = new IOLink( inlet, outlet );
		LinkIterator it = m_Links.find( link );
		if ( it != m_Links.end() )
		{
			link->deactivate();
			delete *it;
			m_Links.erase( it );
		}
		delete link;
	}

}