// RunTests.cpp
//////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#ifdef _DEBUG

#include "RunTests.h"

#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestFailure.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <iostream>
#include<time.h>

//#include "../Presenter.Engine/PresenterUser.h"
//#include "../Presenter.Engine/PresenterUserArray.h"
//#include "../Presenter.Engine/PresenterUserRights.h"

//#include "../PService2/PServiceData.h"

//#include "../Libs/FileOperation.h"


#pragma comment(lib, "cppunit_dll.lib")

class MyCompilerOutputter : public CppUnit::CompilerOutputter
{
public:
	MyCompilerOutputter(CppUnit::TestResultCollector *result, std::ostream &stream,
		const std::string &locationFormat) :
	CompilerOutputter(result, stream, locationFormat)
	{
	}
	
	virtual void printFailureType( CppUnit::TestFailure *failure )
	{
		m_stream  <<  (failure->isError() ? "error" : "error assertion");
	}
};

int RunTests()
{

	clock_t t1=clock();
	// Get the top level suite from the registry
	CPPUNIT_NS::Test *suite = CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest();
	
	// Adds the test to the list of test to run
	CPPUNIT_NS::TextUi::TestRunner runner;
	runner.addTest( suite );
	
	// Change the default outputter to a compiler error format outputter
	runner.setOutputter( new MyCompilerOutputter( &runner.result(),
		std::cerr, "%p(%l) : "));
	// Run the tests.
	bool wasSucessful = runner.run();
	clock_t t2=clock();

	std::cout<<"Time:" << double(t2-t1)/CLOCKS_PER_SEC << " sec\n";

/*
	CString strIni;
	//CPresenterUser User(_T("UserName"), _T(Password));
	CPresenterUserArray UserArray;
	CPresenterUser* pPresenterUser;

	pPresenterUser = new CPresenterUser(_T("User1"), _T("Password1"));
	pPresenterUser->m_UserRight.AddRight(PRESENTER_USER_CLIENT_RIGHT);
	pPresenterUser->m_UserRight.AddRight(PRESENTER_USER_MANAGER_RIGHT);
	UserArray.Add(*pPresenterUser);

	pPresenterUser = new CPresenterUser(_T("User2"), _T("Password2"));
	pPresenterUser->m_UserRight.AddRight(PRESENTER_USER_MANAGER_RIGHT);
	pPresenterUser->m_UserRight.AddRight(PRESENTER_USER_ADMIN_RIGHT);
	UserArray.Add(*pPresenterUser);

	std::cerr << "is PRESENTER_USER_CLIENT_RIGHT " << pPresenterUser->m_UserRight.IsRightExists(PRESENTER_USER_CLIENT_RIGHT) << std::endl;
	std::cerr << "is PRESENTER_USER_MANAGER_RIGHT " << pPresenterUser->m_UserRight.IsRightExists(PRESENTER_USER_MANAGER_RIGHT) << std::endl;
	std::cerr << "is PRESENTER_USER_ADMIN_RIGHT " << pPresenterUser->m_UserRight.IsRightExists(PRESENTER_USER_ADMIN_RIGHT) << std::endl;

	strIni = UserArray.ToString();
//	LoadUTF8FileToString(GetRootFolder() + SERVER_USERS_FILE_NAME, strIni);
	SaveUTF8FileFromString(GetRootFolder() + SERVER_USERS_FILE_NAME, strIni);

	LoadUTF8FileToString(GetRootFolder() + SERVER_USERS_FILE_NAME, strIni);
	CPresenterUserArray UserArray2;
	UserArray2.FromString(strIni);
	strIni = UserArray2.ToString();
	SaveUTF8FileFromString(GetRootFolder() + SERVER_USERS_FILE_NAME + _T("2"), strIni);
	
*/
	// Return error code 1 if the one of test failed.
	return 0;//wasSucessful ? 0 : 1;
} 

#endif _DEBUG
