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

#include "engine/_2RealFunctionBlockIOManager.h"
#include "engine/_2RealFunctionBlockUpdatePolicy.h"
#include "helpers/_2RealException.h"
#include "engine/_2RealAbstractUberBlock.h"
#include "engine/_2RealInlet.h"
#include "engine/_2RealOutlet.h"
#include "engine/_2RealInletBuffer.h"
#include "app/_2RealInletHandle.h"
#include "app/_2RealOutletHandle.h"
#include "bundle/_2RealInletHandle.h"
#include "bundle/_2RealOutletHandle.h"
#include "helpers/_2RealStringHelpers.h"

#include <sstream>

using std::list;
using std::string;
using std::ostringstream;

namespace _2Real
{

	FunctionBlockIOManager::FunctionBlockIOManager( AbstractUberBlock &owner ) :
		Handleable< FunctionBlockIOManager, bundle::BlockHandle >( *this ),
		AbstractIOManager( owner )
	{
	}

	FunctionBlockIOManager::~FunctionBlockIOManager()
	{
		for ( InletIterator it = m_Inlets.begin(); it != m_Inlets.end(); ++it )
		{
			delete *it;
		}

		for ( OutletIterator it = m_Outlets.begin(); it != m_Outlets.end(); ++it )
		{
			delete *it;
		}

		for ( ParameterIterator it = m_Parameters.begin(); it != m_Parameters.end(); ++it )
		{
			delete *it;
		}
	}

	AppInletHandles const& FunctionBlockIOManager::getAppInletHandles() const
	{
		return m_AppInletHandles;
	}

	AppOutletHandles const& FunctionBlockIOManager::getAppOutletHandles() const
	{
		return m_AppOutletHandles;
	}

	AppParameterHandles const& FunctionBlockIOManager::getAppParameterHandles() const
	{
		return m_AppParameterHandles;
	}

	BundleInletHandles const& FunctionBlockIOManager::getBundleInletHandles() const
	{
		return m_BundleInletHandles;
	}

	BundleOutletHandles const& FunctionBlockIOManager::getBundleOutletHandles() const
	{
		return m_BundleOutletHandles;
	}

	BundleParameterHandles const& FunctionBlockIOManager::getBundleParameterHandles() const
	{
		return m_BundleParameterHandles;
	}

	void FunctionBlockIOManager::registerToNewData( AbstractCallback< std::vector< std::shared_ptr< const CustomType > > > &cb )
	{
		m_AppEvent.addListener( cb );
	}

	void FunctionBlockIOManager::unregisterFromNewData( AbstractCallback< std::vector< std::shared_ptr< const CustomType > > > &cb )
	{
		m_AppEvent.removeListener( cb );
	}

	app::InletHandle & FunctionBlockIOManager::getAppInletHandle( string const& name )
	{
		return getInletIO( name ).getHandle();
	}

	app::OutletHandle & FunctionBlockIOManager::getAppOutletHandle( string const& name )
	{
		return getOutletIO( name ).getHandle();
	}

	app::ParameterHandle & FunctionBlockIOManager::getAppParameterHandle( string const& name )
	{
		return getParameterIO( name ).getHandle();
	}

	bundle::InletHandle & FunctionBlockIOManager::getBundleInletHandle( string const& name ) const
	{
		return getInletIO( name ).getBundleInletHandle();
	}

	bundle::OutletHandle & FunctionBlockIOManager::getBundleOutletHandle( string const& name ) const
	{
		return getOutletIO( name ).m_Outlet->getHandle();
	}

	bundle::ParameterHandle & FunctionBlockIOManager::getBundleParameterHandle( string const& name ) const
	{
		return getParameterIO( name ).m_Parameter->getHandle();
	}

	AbstractInletIO & FunctionBlockIOManager::getInletIO( string const& name )
	{
		for ( InletIterator it = m_Inlets.begin(); it != m_Inlets.end(); ++it )
		{
			if ( toLower( ( *it )->info().baseName ) == toLower( name ) )
			{
				return **it;
			}
		}

		ostringstream msg;
		msg << "inlet " << name<< " not found in" << m_Owner.getFullName();
		throw NotFoundException( msg.str() );
	}

	OutletIO & FunctionBlockIOManager::getOutletIO( string const& name )
	{
		for ( OutletIterator it = m_Outlets.begin(); it != m_Outlets.end(); ++it )
		{
			if ( toLower( ( *it )->m_Outlet->getName() ) == toLower( name ) )
			{
				return **it;
			}
		}

		ostringstream msg;
		msg << "outlet " << name<< " not found in" << m_Owner.getFullName();
		throw NotFoundException( msg.str() );
	}

	ParameterIO & FunctionBlockIOManager::getParameterIO( string const& name )
	{
		for ( ParameterIterator it = m_Parameters.begin(); it != m_Parameters.end(); ++it )
		{
			if ( toLower( ( *it )->m_Parameter->getName() ) == toLower( name ) )
			{
				return **it;
			}
		}

		ostringstream msg;
		msg << "parameter " << name<< " not found in" << m_Owner.getFullName();
		throw NotFoundException( msg.str() );
	}

	AbstractInletIO const& FunctionBlockIOManager::getInletIO( string const& name ) const
	{
		for ( InletConstIterator it = m_Inlets.begin(); it != m_Inlets.end(); ++it )
		{
			if ( toLower( ( *it )->info().baseName ) == toLower( name ) )
			{
				return **it;
			}
		}

		ostringstream msg;
		msg << "inlet " << name<< " not found in" << m_Owner.getFullName();
		throw NotFoundException( msg.str() );
	}

	OutletIO const& FunctionBlockIOManager::getOutletIO( string const& name ) const
	{
		for ( OutletConstIterator it = m_Outlets.begin(); it != m_Outlets.end(); ++it )
		{
			if ( toLower( ( *it )->m_Outlet->getName() ) == toLower( name ) )
			{
				return **it;
			}
		}

		ostringstream msg;
		msg << "outlet " << name<< " not found in" << m_Owner.getFullName();
		throw NotFoundException( msg.str() );
	}

	ParameterIO const& FunctionBlockIOManager::getParameterIO( string const& name ) const
	{
		for ( ParameterConstIterator it = m_Parameters.begin(); it != m_Parameters.end(); ++it )
		{
			if ( toLower( ( *it )->m_Parameter->getName() ) == toLower( name ) )
			{
				return **it;
			}
		}

		ostringstream msg;
		msg << "parameter " << name<< " not found in" << m_Owner.getFullName();
		throw NotFoundException( msg.str() );
	}

	void FunctionBlockIOManager::addBasicInlet( InletInfo const& info )
	{
		BasicInletIO *io = new BasicInletIO( m_Owner, *m_UpdatePolicy, info );
		m_UpdatePolicy->addInlet( *io, info.policy );					// creates a trigger and adds it
		io->receiveData( info.initializer );							// receive the first data item
		m_Inlets.push_back( io );
		m_AppInletHandles.push_back( io->getHandle() );
		m_BundleInletHandles.push_back( io->getBundleInletHandle() );
	}

	void FunctionBlockIOManager::addMultiInlet( InletInfo const& info )
	{
		MultiInletIO *io = new MultiInletIO( m_Owner, *m_UpdatePolicy, info );
		m_Inlets.push_back( io );
		m_AppInletHandles.push_back( io->getHandle() );
		m_BundleInletHandles.push_back( io->getBundleInletHandle() );
	}

	void FunctionBlockIOManager::addParameter( ParameterInfo const& info )
	{
		ParameterIO *io = new ParameterIO( m_Owner, info );
		io->m_Parameter->synchronize();
		m_Parameters.push_back( io );
		m_AppParameterHandles.push_back( io->getHandle() );
		m_BundleParameterHandles.push_back( io->m_Parameter->getHandle() );
	}

	void FunctionBlockIOManager::addOutlet( OutletInfo const& info )
	{
		// TODO: lookup type name -> create two type copies: buffer & init

		OutletIO *io = new OutletIO( m_Owner, info );
		io->m_Outlet->synchronize();
		m_Outlets.push_back( io );
		m_AppOutletHandles.push_back( io->getHandle() );
		m_BundleOutletHandles.push_back( io->m_Outlet->getHandle() );
	}

	void FunctionBlockIOManager::updateInletData()
	{
		for ( InletIterator it = m_Inlets.begin(); it != m_Inlets.end(); ++it )
		{
			( *it )->syncInletChanges();		// makes changes to multiinlet visible

			for ( unsigned int i = 0; i<( *it )->getSize(); ++i )
			{
				BasicInletIO &inletIO = ( **it )[ i ];
				inletIO.syncInletData();		// makes changed data visible
			}
		}
	}

	void FunctionBlockIOManager::updateSetupParameters()
	{
		for ( ParameterIterator it = m_Parameters.begin(); it != m_Parameters.end(); ++it )
		{
			( *it )->m_Parameter->synchronize();		// makes changes to visible
		}
	}

	void FunctionBlockIOManager::updateOutletData()
	{
		std::vector< std::shared_ptr< const CustomType > > blockData;
		for ( OutletIterator it = m_Outlets.begin(); it != m_Outlets.end(); ++it )
		{
			Outlet &outlet = *( ( *it )->m_Outlet );

			// ARGH this could be better
			// synchronize & append to app data list or sth??
			bool wasDiscarded = outlet.Outlet::synchronize();

			if ( !wasDiscarded )
			{
				std::shared_ptr< const CustomType > data = outlet.getData();
				// make shit unique within system right now?
				// i.e. create a unique key... sigh
				TimestampedData tmp( data, EngineImpl::instance().getElapsedTime() );
				( *it )->m_InletEvent->notify( tmp );

				( *it )->m_AppEvent->notify( data );
				blockData.push_back( data );
			}
		}

		if ( !blockData.empty() )
			m_AppEvent.notify( blockData );
	}

	void FunctionBlockIOManager::updateInletBuffers( const bool enableTriggering )
	{
		for ( InletIterator it = m_Inlets.begin(); it != m_Inlets.end(); ++it )
		{
			for ( unsigned int i = 0; i<( *it )->getSize(); ++i )
			{
				BasicInletIO &inletIO = ( **it )[ i ];
				inletIO.processBufferedData( enableTriggering );
			}
		}
	}

}