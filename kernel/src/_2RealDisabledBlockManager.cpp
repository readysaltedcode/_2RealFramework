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

#include "_2RealDisabledBlockManager.h"
#include "_2RealException.h"
#include "_2RealAbstractUberBlock.h"

#include <sstream>

using std::ostringstream;

namespace _2Real
{

	DisabledBlockManager::DisabledBlockManager(AbstractUberBlock &owner) :
		AbstractBlockManager(owner)
	{
	}

	DisabledBlockManager::~DisabledBlockManager()
	{
	}

	void DisabledBlockManager::clear()
	{
	}

	AbstractUberBlock & DisabledBlockManager::getBlock( BlockIdentifier const& id )
	{
		ostringstream msg;
		msg << "block " << id.getName() << " not found in " << m_Owner.getName();
		throw NotFoundException( msg.str() );
	}

	AbstractUberBlock const& DisabledBlockManager::getBlock( BlockIdentifier const& id ) const
	{
		ostringstream msg;
		msg << "block " << id.getName() << " not found in " << m_Owner.getName();
		throw NotFoundException( msg.str() );
	}

	void DisabledBlockManager::addBlock( AbstractUberBlock &block )
	{
	}

	void DisabledBlockManager::removeBlock( AbstractUberBlock &block )
	{
	}

}