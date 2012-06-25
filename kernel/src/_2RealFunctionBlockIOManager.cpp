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

#include "_2RealFunctionBlockIOManager.h"
#include "_2RealException.h"
#include "_2RealAbstractUberBlock.h"
#include "_2RealSetupParameter.h"
#include "_2RealInlet.h"
#include "_2RealOutlet.h"
#include "_2RealInlet.h"
#include "_2RealBlockData.h"
#include "_2RealParameterData.h"
#include "_2RealUpdatePolicyImpl.h"
#include "_2RealLink.h"
#include "_2RealHelpersInternal.h"
#include "_2RealInletHandle.h"
#include "_2RealOutletHandle.h"


#include <sstream>

using std::map;
using std::string;
using std::make_pair;
using std::ostringstream;

namespace _2Real
{

	FunctionBlockIOManager::FunctionBlockIOManager(AbstractUberBlock &owner) :
		AbstractIOManager(owner)
	{
	}

	FunctionBlockIOManager::~FunctionBlockIOManager()
	{
		clear();

		for ( OutletFunctionCallbacks::iterator it = m_OutletFunctionCallbacks.begin(); it != m_OutletFunctionCallbacks.end(); ++it )
		{
			delete *it;
		}

		for ( OutletCallbackHandlers::iterator it = m_OutletCallbackHandlers.begin(); it != m_OutletCallbackHandlers.end(); ++it )
		{
			delete *it;
		}

		for ( OutputFunctionCallbacks::iterator it = m_OutputFunctionCallbacks.begin(); it != m_OutputFunctionCallbacks.end(); ++it )
		{
			delete *it;
		}

		for ( OutputCallbackHandlers::iterator it = m_OutputCallbackHandlers.begin(); it != m_OutputCallbackHandlers.end(); ++it )
		{
			delete *it;
		}
	}

	void FunctionBlockIOManager::initFrom( BlockData const& meta )
	{
		std::map< std::string, ParameterData > const& setup = meta.getParameters();
		std::map< std::string, ParameterData > const& input = meta.getInlets();
		std::map< std::string, ParameterData > const& output = meta.getOutlets();

		for ( std::map< std::string, ParameterData >::const_iterator it = setup.begin(); it != setup.end(); ++it )
		{
			ParameterData const& meta = it->second;
			SetupParameter &param = createSetupParameterFromParameterData( meta );
			m_Params.insert( make_pair( param.getName(), &param ) );
		}

		for ( std::map< std::string, ParameterData >::const_iterator it = input.begin(); it != input.end(); ++it )
		{
			ParameterData const& meta = it->second;
			Inlet& inlet = createInletFromParameterData( meta );
			m_Inlets.insert( make_pair( inlet.getName(), &inlet ) );
		}

		for (std::map< std::string, ParameterData >::const_iterator it = output.begin(); it != output.end(); ++it )
		{
			ParameterData const& meta = it->second;
			Outlet& outlet = createOutletFromParameterData( meta );
			m_Outlets.insert( make_pair( outlet.getName(), &outlet ) );
		}
	}

	void FunctionBlockIOManager::clear()
	{
		for (InletMap::iterator it = m_Inlets.begin(); it != m_Inlets.end(); ++it )
		{
			it->second->clearLinks();
		}

		for (OutletMap::iterator it = m_Outlets.begin(); it != m_Outlets.end(); ++it )
		{
			it->second->clearLinks();
		}

		for (InletMap::iterator it = m_Inlets.begin(); it != m_Inlets.end(); /**/)
		{
			delete it->second;
			it = m_Inlets.erase(it);
		}

		for (OutletMap::iterator it = m_Outlets.begin(); it != m_Outlets.end(); /**/)
		{
			it->second->clearLinks();
			delete it->second;
			it = m_Outlets.erase(it);
		}

		for (ParamMap::iterator it = m_Params.begin(); it != m_Params.end(); /**/)
		{
			delete it->second;
			it = m_Params.erase(it);
		}
	}

	//InletMap const& FunctionBlockIOManager::getInlets() const
	//{
	//	return m_Inlets;
	//}

	//OutletMap const& FunctionBlockIOManager::getOutlets() const
	//{
	//	return m_Outlets;
	//}

	void FunctionBlockIOManager::registerToNewData( std::string const& outName, OutletCallback callback, void *userData )
	{
		OutletFunctionCallback *cb = new OutletFunctionCallback( callback, userData );
		
		OutletFunctionCallbacks::iterator it = m_OutletFunctionCallbacks.find( cb );
		if ( it == m_OutletFunctionCallbacks.end() )
		{
			m_OutletFunctionCallbacks.insert( cb );
			
			OutletMap::const_iterator outletIt = m_Outlets.find(outName);
			if (outletIt != m_Outlets.end())
			{
				outletIt->second->registerCallback( *cb );
			}
		}
		else
		{
			delete cb;
		}
	}

	void FunctionBlockIOManager::unregisterFromNewData( std::string const& outName, OutletCallback callback, void *userData )
	{
		OutletFunctionCallback *cb = new OutletFunctionCallback( callback, userData );

		OutletFunctionCallbacks::iterator it = m_OutletFunctionCallbacks.find( cb );
		if ( it != m_OutletFunctionCallbacks.end() )
		{
			OutletMap::const_iterator outletIt = m_Outlets.find(outName);
			if (outletIt != m_Outlets.end())
			{
				outletIt->second->unregisterCallback( **it );
			}

			delete *it;
			m_OutletFunctionCallbacks.erase(it);
		}

		delete cb;
	}

	void FunctionBlockIOManager::registerToNewData( std::string const& outName, AbstractOutletCallbackHandler &handler )
	{
		OutletCallbackHandlers::iterator it = m_OutletCallbackHandlers.find( &handler );
		if ( it == m_OutletCallbackHandlers.end() )
		{
			m_OutletCallbackHandlers.insert( &handler );

			OutletMap::const_iterator outletIt = m_Outlets.find(outName);
			if (outletIt != m_Outlets.end())
			{
				outletIt->second->registerCallback( handler );
			}
		}
		else
		{
			delete &handler;
		}
	}

	void FunctionBlockIOManager::unregisterFromNewData( std::string const& outName, AbstractOutletCallbackHandler &handler )
	{
		OutletCallbackHandlers::iterator it = m_OutletCallbackHandlers.find( &handler );
		if ( it != m_OutletCallbackHandlers.end() )
		{
			OutletMap::const_iterator outletIt = m_Outlets.find(outName);
			if (outletIt != m_Outlets.end())
			{
				outletIt->second->unregisterCallback( **it );
			}

			delete *it;
			m_OutletCallbackHandlers.erase(it);
		}

		delete &handler;
	}

	void FunctionBlockIOManager::registerToNewData( OutputCallback callback, void *userData )
	{
		OutputFunctionCallback *cb = new OutputFunctionCallback( callback, userData );
		
		OutputFunctionCallbacks::iterator it = m_OutputFunctionCallbacks.find( cb );
		if ( it == m_OutputFunctionCallbacks.end() )
		{
			m_OutputFunctionCallbacks.insert( cb );
		}
		else
		{
			delete cb;
		}
	}

	void FunctionBlockIOManager::unregisterFromNewData( OutputCallback callback, void *userData )
	{
		OutputFunctionCallback *cb = new OutputFunctionCallback( callback, userData );

		OutputFunctionCallbacks::iterator it = m_OutputFunctionCallbacks.find( cb );
		if ( it != m_OutputFunctionCallbacks.end() )
		{
			delete *it;
			m_OutputFunctionCallbacks.erase(it);
		}

		delete cb;
	}

	void FunctionBlockIOManager::registerToNewData( AbstractOutputCallbackHandler &handler )
	{
		OutputCallbackHandlers::iterator it = m_OutputCallbackHandlers.find( &handler );
		if ( it == m_OutputCallbackHandlers.end() )
		{
			m_OutputCallbackHandlers.insert( &handler );
		}
		else
		{
			delete &handler;
		}
	}

	void FunctionBlockIOManager::unregisterFromNewData( AbstractOutputCallbackHandler &handler )
	{
		OutputCallbackHandlers::iterator it = m_OutputCallbackHandlers.find( &handler );
		if ( it != m_OutputCallbackHandlers.end() )
		{
			delete *it;
			m_OutputCallbackHandlers.erase(it);
		}

		delete &handler;
	}

	EngineData const& FunctionBlockIOManager::getValue( string const& paramName ) const
	{
		// TODO: parameter names have to be unique oO

		SetupParameter const* param = getMapElement< string, SetupParameter >( paramName, m_Params );
		if ( param != nullptr )
		{
			return param->getParameterValue();
		}

		Inlet const* inlet = getMapElement< string, Inlet >( paramName, m_Inlets );
		if ( inlet != nullptr )
		{
			return inlet->getCurrentValue();
		}

		Outlet const* outlet = getMapElement< string, Outlet >( paramName, m_Outlets );
		if ( outlet != nullptr )
		{
			return outlet->getLastData();
		}

		ostringstream msg;
		msg << "no inlet / outlet / setup parameter named " << paramName << " found in " << m_Owner.getName();
		throw NotFoundException( msg.str() );
	}

	void FunctionBlockIOManager::setValue( string const& paramName, TimestampedData const& value )
	{
		SetupParameter *const param = getMapElement< string, SetupParameter >( paramName, m_Params );
		if ( param != nullptr )
		{
			param->setParameterValue( value.getData() );
			return;
		}

		Inlet *const inlet = getMapElement< string, Inlet >( paramName, m_Inlets );
		if ( inlet != nullptr )
		{
			inlet->setToValue( value );
			return;
		}

		ostringstream msg;
		msg << "no inlet / setup parameter named " << paramName << " found in " << m_Owner.getName();
		throw NotFoundException( msg.str() );
	}

	std::string const& FunctionBlockIOManager::getTypename( string const& paramName ) const
	{
		SetupParameter const* param = getMapElement< string, SetupParameter >( paramName, m_Params );
		if ( param != nullptr )
		{
			return param->getTypename();
		}

		Inlet const* inlet = getMapElement< string, Inlet >( paramName, m_Inlets );
		if ( inlet != nullptr )
		{
			return inlet->getTypename();
		}

		Outlet const* outlet = getMapElement< string, Outlet >( paramName, m_Outlets );
		if ( outlet != nullptr )
		{
			return outlet->getTypename();
		}

		ostringstream msg;
		msg << "no inlet / outlet / setup parameter named " << paramName << " found in " << m_Owner.getName();
		throw NotFoundException( msg.str() );
	}

	std::string const& FunctionBlockIOManager::getLongTypename( string const& paramName ) const
	{
		SetupParameter const* param = getMapElement< string, SetupParameter >( paramName, m_Params );
		if ( param != nullptr )
		{
			return param->getLongTypename();
		}

		Inlet const* inlet = getMapElement< string, Inlet >( paramName, m_Inlets );
		if ( inlet != nullptr )
		{
			return inlet->getLongTypename();
		}

		Outlet const* outlet = getMapElement< string, Outlet >( paramName, m_Outlets );
		if ( outlet != nullptr )
		{
			return outlet->getLongTypename();
		}

		ostringstream msg;
		msg << "no inlet / outlet / setup parameter named " << paramName << " found in " << m_Owner.getName();
		throw NotFoundException( msg.str() );
	}

	InletHandle FunctionBlockIOManager::createInletHandle( string const& name )
	{
		Inlet &inlet = getMapElement< string, Inlet >( name, m_Inlets, "inlet" );
		return InletHandle( inlet );
	}

	OutletHandle FunctionBlockIOManager::createOutletHandle(std::string const& name)
	{
		Outlet &outlet = getMapElement< string, Outlet >( name, m_Outlets, "outlet" );
		return OutletHandle( outlet );
	}

	//void FunctionBlockIOManager::linkWith( std::string const& inlet, AbstractUberBlock &out, std::string const& outlet )
	//{
		//InletMap::iterator itIn = m_Inlets.find(inlet);
		//if ( itIn == m_Inlets.end() )
		//{
		//	std::ostringstream msg;
		//	msg << "no inlet named " << inlet << " found in " << m_Owner.getName();
		//	throw NotFoundException(msg.str());
		//}

		//AbstractIOManager &mgr = out.getIOManager();
		//OutletMap const& outlets = mgr.getOutlets();			//might throw!
		//OutletMap::const_iterator itOut = outlets.find(outlet);
		//if (itOut == outlets.end())
		//{
		//	std::ostringstream msg;
		//	msg << "no outlet named " << outlet << " found in " << out.getName();
		//	throw NotFoundException(msg.str());
		//}

		//itIn->second->linkWith(*(itOut->second));
	//}

	void FunctionBlockIOManager::updateInletValues()
	{
		for (InletMap::iterator it = m_Inlets.begin(); it != m_Inlets.end(); ++it)
		{
			it->second->updateCurrentValue();
		}
	}

	void FunctionBlockIOManager::updateOutletValues()
	{
		std::list< OutputData > data;
		for (OutletMap::iterator it = m_Outlets.begin(); it != m_Outlets.end(); ++it)
		{
			it->second->update();
			data.push_back( it->second->getLastOutputData() );
		}

		if ( data.size() > 0 )
		{
			for ( OutputFunctionCallbacks::iterator it = m_OutputFunctionCallbacks.begin(); it != m_OutputFunctionCallbacks.end(); ++it )
			{
				( *it )->invoke( data );
			}

			for ( OutputCallbackHandlers::iterator it = m_OutputCallbackHandlers.begin(); it != m_OutputCallbackHandlers.end(); ++it )
			{
				( *it )->invoke( data );
			}
		}
	}

	void FunctionBlockIOManager::updateInletBuffers()
	{
		for (InletMap::iterator it = m_Inlets.begin(); it != m_Inlets.end(); ++it)
		{
			it->second->resetData();
		}
	}

	Inlet const& FunctionBlockIOManager::getInlet( std::string const& name ) const
	{
		return getMapElement< string, Inlet >( name, m_Inlets, "inlet" );
	}

	Outlet const& FunctionBlockIOManager::getOutlet( std::string const& name ) const
	{
		return getMapElement< string, Outlet >( name, m_Outlets, "outlet" );
	}

	SetupParameter const& FunctionBlockIOManager::getSetupParameter( std::string const& name ) const
	{
		return getMapElement< string, SetupParameter >( name, m_Params, "setup parameter" );
	}

}