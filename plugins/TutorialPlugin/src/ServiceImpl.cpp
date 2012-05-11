#include "ServiceImpl.h"
#include "_2RealException.h"
#include "_2RealEnum.h"

using namespace _2Real;
using namespace std;

void Counter::setup(_2Real::ServiceContext &context)
{
	try
	{
		std::cout << "counter setup!" << std::endl;
		m_CurrentCount = 1;
		m_CounterValue = context.getOutputHandle("counter outlet");
		m_Test = context.getOutputHandle("test");
	}
	catch (_2Real::Exception &e)
	{
		e.rethrow();
	}
	catch (std::exception &e)
	{
		std::ostringstream msg;
		msg << "exception during service setup: " << e.what();
		throw _2Real::ServiceException(msg.str());
	}
};

void Counter::update()
{
	try
	{
		std::cout << "counter update" << std::endl;
		m_CounterValue.data<int>() = m_CurrentCount;
		m_CurrentCount+=1;
		std::vector< int > test;
		test.push_back(1);
		test.push_back(2);
		test.push_back(3);
		m_Test.data< std::vector < int > >() = test;
	}
	catch (_2Real::Exception &e)
	{
		e.rethrow();
	}
	catch (std::exception &e)
	{
		std::ostringstream msg;
		msg << "exception during service update: " << e.what();
		throw _2Real::ServiceException(msg.str());
	}
};

void Doubler::setup(_2Real::ServiceContext &context)
{
	try
	{
		std::cout << "doubler setup!" << std::endl;
		m_InputValue = context.getInputHandle("number inlet");
		m_OutputValue = context.getOutputHandle("doubler outlet");
	}
	catch (_2Real::Exception &e)
	{
		e.rethrow();
	}
	catch (std::exception &e)
	{
		std::ostringstream msg;
		msg << "exception during service setup: " << e.what();
		throw _2Real::ServiceException(msg.str());
	}
};

void Doubler::update()
{
	try
	{
		m_OutputValue.data< int>() = 2*m_InputValue.data< int >();
	}
	catch (_2Real::Exception &e)
	{
		e.rethrow();
	}
	catch (std::exception &e)
	{
		std::ostringstream msg;
		msg << "exception during service update: " << e.what();
		throw _2Real::ServiceException(msg.str());
	}
};

void PrintOut::setup(_2Real::ServiceContext &context)
{
	try
	{
		std::cout << "print out setup!" << std::endl;
		m_InputValue = context.getInputHandle("input number");

		Enumeration e = context.getParameterValue< Enumeration >("test enum");
		std::cout << e.valueFor("option 1") << std::endl;
		std::cout << e.valueFor("option 2") << std::endl;
		std::cout << e.valueFor("option 3") << std::endl;
		std::cout << e.valueFor("not there") << std::endl;

		e["thingie"] = "eeeeeeeeeee";
		e.setValues(Strings("a")("b")("c"));
	}
	catch (_2Real::Exception &e)
	{
		e.rethrow();
	}
	catch (std::exception &e)
	{
		std::ostringstream msg;
		msg << "exception during service setup: " << e.what();
		throw _2Real::ServiceException(msg.str());
	}
};

void PrintOut::update()
{
	try
	{
		cout << "PrintOut: " << m_InputValue.data< int >() << endl;
	}
	catch (_2Real::Exception &e)
	{
		e.rethrow();
	}
	catch (std::exception &e)
	{
		std::ostringstream msg;
		msg << "exception during service update: " << e.what();
		throw _2Real::ServiceException(msg.str());
	}
};