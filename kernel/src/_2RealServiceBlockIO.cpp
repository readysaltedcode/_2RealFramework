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

#include "_2RealServiceBlockIO.h"
#include "_2RealException.h"
#include "_2RealAbstractUberBlock.h"
#include "_2RealSetupParameter.h"
#include "_2RealInlet.h"
#include "_2RealOutlet.h"
#include "_2RealInlet.h"
#include "_2RealBlockData.h"
#include "_2RealParameterData.h"

#include <sstream>

namespace _2Real
{

	ServiceIO::ServiceIO(AbstractBlock &owner) :
		AbstractIOManager(owner),
		m_Policy(new AlwaysInsertRemoveOldest())
	{
	}

	ServiceIO::~ServiceIO()
	{
		clear();

		for ( DataFunctionCallbacks::iterator it = m_DataFunctionCallbacks.begin(); it != m_DataFunctionCallbacks.end(); ++it )
		{
			delete *it;
		}

		for ( DataCallbackHandlers::iterator it = m_DataCallbackHandlers.begin(); it != m_DataCallbackHandlers.end(); ++it )
		{
			delete *it;
		}
	}

	void ServiceIO::initFrom(BlockData const& meta, Poco::Timestamp const& time)
	{
		std::map< std::string, ParameterData const* > const& setup = meta.getParameters();
		std::map< std::string, ParameterData const* > const& input = meta.getInlets();
		std::map< std::string, ParameterData const* > const& output = meta.getOutlets();

		for (std::map< std::string, ParameterData const* >::const_iterator it = setup.begin(); it != setup.end(); ++it)
		{
			ParameterData const& meta = *it->second;
			SetupParameter *param = new SetupParameter(meta);
			m_Params.insert(std::make_pair(param->getName(), param));
		}

		for (std::map< std::string, ParameterData const* >::const_iterator it = input.begin(); it != input.end(); ++it)
		{
			ParameterData const& meta = *it->second;
			Inlet *inlet = new Inlet(meta, *m_Policy, 50);
			m_Inlets.insert(std::make_pair(inlet->getName(), inlet));
		}

		for (std::map< std::string, ParameterData const* >::const_iterator it = output.begin(); it != output.end(); ++it)
		{
			ParameterData const& meta = *it->second;
			Outlet *outlet = new Outlet(meta, time);
			m_Outlets.insert(std::make_pair(outlet->getName(), outlet));
		}


	}

	void ServiceIO::clear()
	{
		for (InletMap::iterator it = m_Inlets.begin(); it != m_Inlets.end(); ++it )
		{
			it->second->clearLinks();
		}

		for (OutletMap::iterator it = m_Outlets.begin(); it != m_Outlets.end(); ++it )
		{
			it->second->clearLinks();
		}

		if (m_Policy != NULL)
		{
			delete m_Policy;
			m_Policy = NULL;
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

	InletMap const& ServiceIO::getInlets() const
	{
		return m_Inlets;
	}

	OutletMap const& ServiceIO::getOutlets() const
	{
		return m_Outlets;
	}

	//void ServiceIO::addParam(ParameterData const& meta)
	//{
	//	SetupParameter *param = new SetupParameter(meta);
	//	m_Params.insert(std::make_pair(param->getName(), param));
	//}

	//void ServiceIO::addInlet(ParameterData const& meta)
	//{
	//	Inlet *inlet = new Inlet(meta, *m_Policy, 50);
	//	m_Inlets.insert(std::make_pair(inlet->getName(), inlet));
	//}

	//void ServiceIO::addOutlet(ParameterData const& meta, Poco::Timestamp const& timestamp)
	//{
	//	Outlet *outlet = new Outlet(meta, timestamp);
	//	m_Outlets.insert(std::make_pair(outlet->getName(), outlet));
	//}

	void ServiceIO::registerToNewData( std::string const& outName, DataCallback callback, void *userData )
	{
		DataFunctionCallback *cb = new DataFunctionCallback( callback, userData );
		
		DataFunctionCallbacks::iterator it = m_DataFunctionCallbacks.find( cb );
		if ( it == m_DataFunctionCallbacks.end() )
		{
			m_DataFunctionCallbacks.insert( cb );
			
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

	void ServiceIO::unregisterFromNewData( std::string const& outName, DataCallback callback, void *userData )
	{
		DataFunctionCallback *cb = new DataFunctionCallback( callback, userData );

		DataFunctionCallbacks::iterator it = m_DataFunctionCallbacks.find( cb );
		if ( it != m_DataFunctionCallbacks.end() )
		{
			OutletMap::const_iterator outletIt = m_Outlets.find(outName);
			if (outletIt != m_Outlets.end())
			{
				outletIt->second->unregisterCallback( **it );
			}

			delete *it;
			m_DataFunctionCallbacks.erase(it);
		}

		delete cb;
	}

	void ServiceIO::registerToNewData( std::string const& outName, AbstractDataCallbackHandler &handler )
	{
		DataCallbackHandlers::iterator it = m_DataCallbackHandlers.find( &handler );
		if ( it == m_DataCallbackHandlers.end() )
		{
			m_DataCallbackHandlers.insert( &handler );

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

	void ServiceIO::unregisterFromNewData( std::string const& outName, AbstractDataCallbackHandler &handler )
	{
		DataCallbackHandlers::iterator it = m_DataCallbackHandlers.find( &handler );
		if ( it != m_DataCallbackHandlers.end() )
		{
			OutletMap::const_iterator outletIt = m_Outlets.find(outName);
			if (outletIt != m_Outlets.end())
			{
				outletIt->second->unregisterCallback( **it );
			}

			delete *it;
			m_DataCallbackHandlers.erase(it);
		}

		delete &handler;
	}

	const EngineData ServiceIO::getValue(std::string const& name) const
	{
		ParamMap::const_iterator paramIt = m_Params.find(name);
		if (paramIt != m_Params.end())
		{
			return paramIt->second->getData();
		}

		InletMap::const_iterator inletIt = m_Inlets.find(name);
		if (inletIt != m_Inlets.end())
		{
			return inletIt->second->getNewest();
		}

		std::ostringstream msg;
		msg << "no inlet or setup parameter named " << name << " found in " << m_Owner.getName();
		throw NotFoundException(msg.str());
	}

	std::string const& ServiceIO::getKey(std::string const& name) const
	{
		ParamMap::const_iterator paramIt = m_Params.find(name);
		if (paramIt != m_Params.end())
		{
			return paramIt->second->getKeyword();
		}

		InletMap::const_iterator inletIt = m_Inlets.find(name);
		if (inletIt != m_Inlets.end())
		{
			return inletIt->second->getKeyword();
		}

		std::ostringstream msg;
		msg << "no inlet or setup parameter named " << name << " found in " << m_Owner.getName();
		throw NotFoundException(msg.str());
	}

	void ServiceIO::setValue(std::string const& name, Data const& value)
	{
		ParamMap::iterator paramIt = m_Params.find(name);
		if (paramIt != m_Params.end())
		{
			paramIt->second->setData(value.data());
			return;
		}

		InletMap::iterator inletIt = m_Inlets.find( name );
		if (inletIt != m_Inlets.end())
		{
			inletIt->second->setData(TimestampedData(value.getTimestamp(), value.data()));
			return;
		}

		std::ostringstream msg;
		msg << "no inlet or setup parameter named " << name << " found in " << m_Owner.getName();
		throw NotFoundException(msg.str());
	}

	void ServiceIO::insertValue(std::string const& name, Data const& value)
	{
		InletMap::iterator inletIt = m_Inlets.find(name);
		if (inletIt != m_Inlets.end())
		{
			inletIt->second->insertData(TimestampedData(value.getTimestamp(), value.data()));
		}

		std::ostringstream msg;
		msg << "no inlet named " << name << " found in " << m_Owner.getName();
		throw NotFoundException(msg.str());
	}

	InletHandle ServiceIO::createInletHandle(std::string const& name)
	{
		InletMap::iterator it = m_Inlets.find(name);
		if (it != m_Inlets.end())
		{
			return InletHandle(*(it->second));
		}

		std::ostringstream msg;
		msg << "no inlet named " << name << " found in " << m_Owner.getName();
		throw NotFoundException(msg.str());
	}

	OutletHandle ServiceIO::createOutletHandle(std::string const& name)
	{
		OutletMap::iterator it = m_Outlets.find(name);
		if (it != m_Outlets.end())
		{
			return  OutletHandle(*(it->second));
		}

		std::ostringstream msg;
		msg << "no outlet named " << name << " found in " << m_Owner.getName();
		throw NotFoundException(msg.str());
	}

	void ServiceIO::linkWith(std::string const& inlet, AbstractBlock &out, std::string const& outlet)
	{
		InletMap::iterator itIn = m_Inlets.find(inlet);
		if (itIn == m_Inlets.end())
		{
			std::ostringstream msg;
			msg << "no inlet named " << inlet << " found in " << m_Owner.getName();
			throw NotFoundException(msg.str());
		}

		AbstractIOManager &mgr = out.getIOManager();
		OutletMap const& outlets = mgr.getOutlets();			//might throw!
		OutletMap::const_iterator itOut = outlets.find(outlet);
		if (itOut == outlets.end())
		{
			std::ostringstream msg;
			msg << "no outlet named " << outlet << " found in " << out.getName();
			throw NotFoundException(msg.str());
		}

		itIn->second->linkWith(*(itOut->second));
	}

	void ServiceIO::syncInlets()
	{
		for (InletMap::iterator it = m_Inlets.begin(); it != m_Inlets.end(); ++it)
		{
			it->second->syncBuffers();
		}
	}

	void ServiceIO::updateOutlets()
	{
		for (OutletMap::iterator it = m_Outlets.begin(); it != m_Outlets.end(); ++it)
		{
			it->second->update();
		}
	}

	void ServiceIO::updateInlets()
	{
		for (InletMap::iterator it = m_Inlets.begin(); it != m_Inlets.end(); ++it)
		{
			it->second->removeConsumedItems();
			it->second->resetData();
		}
	}

	void ServiceIO::subBlockAdded(AbstractBlock &subBlock)
	{
	}

	void ServiceIO::subBlockRemoved(AbstractBlock &subBlock)
	{
	}

	void ServiceIO::uberBlockAdded(AbstractBlock &uberBlock)
	{
	}

	void ServiceIO::uberBlockRemoved(AbstractBlock &uberBlock)
	{
	}
}