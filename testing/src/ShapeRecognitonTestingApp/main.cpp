/*
	CADET - Center for Advances in Digital Entertainment Technologies
	Copyright 2012 Fachhochschule Salzburg GmbH

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

#include "_2RealApplication.h"

#include <iostream>
#include <sstream>
#include <map>
#include <list>
#include <conio.h>

#include "Poco/Mutex.h"

/*
#ifndef _UNIX
	#include "vld.h"
#endif
*/
#include <sstream>

using std::string;
using std::cout;
using std::endl;
using std::cin;

using _2Real::Exception;

using _2Real::app::Engine;
using _2Real::app::BundleInfo;
using _2Real::app::BlockInfo;
using _2Real::app::BlockHandle;
using _2Real::app::BundleHandle;
using _2Real::app::InletHandle;
using _2Real::app::OutletHandle;
using _2Real::app::AppData;

using Poco::ScopedLock;
using Poco::FastMutex;

using namespace _2Real;

bool run = true;


class Receiver
{
public:
	Receiver()
	{
	}

	~Receiver()
	{
	}

	void receiveResult( AppData const &data )
	{
		m_Access.lock();

		result = data.getData<std::string>();
		
		m_Access.unlock();
	}

	void printValues()
	{
		m_Access.lock();

		std::cout << "Recognition Result: " << result << std::endl;

		m_Access.unlock();
	}

private:

	Poco::FastMutex			m_Access;
	string					result;
};

int main( int argc, char *argv[] )
{
	try
	{
		Receiver receiver;

		// start engine
		Engine &engine = Engine::instance();
		engine.setBaseDirectory( "../../bundles/bin" );

		// load bundles
		BundleHandle koniBundle = engine.loadBundle( "KinectOpenNIBundle" );		
		BundleHandle srBundle = engine.loadBundle( "ShapeRecognitionBundle" );

		// create blocks
		BlockHandle &koniBlock = koniBundle.createBlockInstance("KinectOpenNIDepthBlock");		
		BlockHandle &shapeRecordingBlock = srBundle.createBlockInstance( "ShapeRecordingBlock" );
		BlockHandle &shapeRecognitionBlock = srBundle.createBlockInstance( "ShapeRecognitionBlock" );
		
		// set update rates
		shapeRecordingBlock.setUpdateRate( 30.0f );
		koniBlock.setUpdateRate( 30.0f );
		shapeRecognitionBlock.setUpdateRate( 30.0f );
		
		// link inlet with outlet handles
		InletHandle recordingDepthImageIn = shapeRecordingBlock.getInletHandle( "depth_image");
		InletHandle recognitionDepthImageIn = shapeRecognitionBlock.getInletHandle( "depth_image");

		shapeRecordingBlock.getInletHandle( "depth_image").link(koniBlock.getOutletHandle("ImageData"));
		shapeRecognitionBlock.getInletHandle( "depth_image").link(koniBlock.getOutletHandle("ImageData"));

		// set inital inlet handle values
		shapeRecordingBlock.getInletHandle( "output_path").setValue< string >("./data"); /** pfad angeben **/
		shapeRecordingBlock.getInletHandle( "save").setValue< bool >("false");

		shapeRecognitionBlock.getInletHandle( "data_path" ).setValue< string >("./data" ); /** pfad angeben **/

		// register outlet handles
		shapeRecognitionBlock.getOutletHandle("file_name").registerToNewData( receiver, &Receiver::receiveResult );
		
		// setup blocks
		koniBlock.setup();
		shapeRecordingBlock.setup();
		shapeRecognitionBlock.setup();
						

		// start blocks
		shapeRecognitionBlock.start();
		shapeRecordingBlock.start();
		koniBlock.start();


		while( ::run )
		{
			if( kbhit() )
				if( getch() == 'q' )
					break;
				else if( getch() == 's' )
					shapeRecordingBlock.getInletHandle( "save" ).setValue< bool >( "true" );

			receiver.printValues();
			Sleep( 100 );
			shapeRecordingBlock.getInletHandle( "save").setValue< bool >("false"); 
		}

		// stop blocks
		koniBlock.stop();
		shapeRecordingBlock.stop();
		shapeRecognitionBlock.stop();

		// unregister outlet handles
		shapeRecognitionBlock.getOutletHandle( "file_name" ).unregisterFromNewData( receiver, &Receiver::receiveResult );

		// save config
		engine.safeConfig( "img_test.xml" );

		// unload bundles
		koniBundle.unload();
		srBundle.unload();
	}
	catch ( Exception &e )
	{
		cout << e.what() << " " << e.message() << endl;

#ifdef _DEBUG
		getchar();
#endif
	}

	return 0;
}