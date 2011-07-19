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

#include "_2RealFramework.h"
#include "_2RealData.h"

/*
	service interface
*/

namespace _2Real
{
	class _2RealIService : public Poco::Runnable
	{
	public:
		
		//virtual _2RealMetaData metadata() = 0;
		
		virtual void setup(_2RealData& _data) = 0;					//service initialization
		virtual void run() = 0;										//run() replaces update() and serves as entry point for poco::thread
		virtual void shutdown() = 0;								//clean up function
		
		virtual void addListener(_2RealServicePtr _listener) = 0;	//adds another service to listerners
		virtual void inputListener(_2RealData& _data) = 0;			//reacts to new data
	};
}