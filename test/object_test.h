#ifndef _OBJECT_TEST_H_
#define _OBJECT_TEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include "object.h"

class objectTest : public CppUnit::TestFixture  {
  CPPUNIT_TEST_SUITE( objectTest );
  CPPUNIT_TEST( Constructor );
  CPPUNIT_TEST( Equal );
  CPPUNIT_TEST( CopyConstructor );
  CPPUNIT_TEST( GetSet );
  
  CPPUNIT_TEST_SUITE_END();

  Object m_object;

  public:
  
  void setUp();
  void tearDown();
  
  void Constructor();
  void Equal();
  void CopyConstructor();
  void GetSet();
};

#endif //_OBJECT_TEST_H_