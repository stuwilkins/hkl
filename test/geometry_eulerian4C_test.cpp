// File to test angleconfiguration implementation.
#include "geometry_eulerian4C_test.h"
#include "constants.h"

CPPUNIT_TEST_SUITE_REGISTRATION( GeometryEulerian4CTest );

void
GeometryEulerian4CTest::setUp(void) {}

void 
GeometryEulerian4CTest::tearDown(void) {}

void
GeometryEulerian4CTest::equal(void)
{
    CPPUNIT_ASSERT_EQUAL(m_geometry, m_geometry);
}

void 
GeometryEulerian4CTest::copyConstructor(void)
{
    geometry::Eulerian4C geometry(m_geometry);

    CPPUNIT_ASSERT_EQUAL(m_geometry, geometry);
}

void 
GeometryEulerian4CTest::otherConstructors(void)
{
    double omega = 10 * constant::math::degToRad;
    double chi = 11 * constant::math::degToRad;
    double phi = 12 * constant::math::degToRad;
    double two_theta =13 * constant::math::degToRad;

    geometry::Eulerian4C geometry_ref;
    geometry::Eulerian4C geometry(omega, chi, phi, two_theta);

    geometry_ref.get_axe("omega").set_value(omega);
    geometry_ref.get_axe("chi").set_value(chi);
    geometry_ref.get_axe("phi").set_value(phi);
    geometry_ref.get_axe("2theta").set_value(two_theta);

    CPPUNIT_ASSERT_EQUAL(geometry_ref, geometry);
}
void
GeometryEulerian4CTest::getAxesNames(void)
{
    vector<MyString> v = m_geometry.getAxesNames();
    CPPUNIT_ASSERT_EQUAL(MyString("omega"), v[0]);
    CPPUNIT_ASSERT_EQUAL(MyString("chi"), v[1]);
    CPPUNIT_ASSERT_EQUAL(MyString("phi"), v[2]);
    CPPUNIT_ASSERT_EQUAL(MyString("2theta"), v[3]);
}

void
GeometryEulerian4CTest::getSampleQuaternion(void)
{
    m_geometry.get_axe("omega").set_value(90 * constant::math::degToRad);

    CPPUNIT_ASSERT_EQUAL(Quaternion(1./sqrt(2), 0, -1./sqrt(2), 0.), m_geometry.getSampleQuaternion());
}

void
GeometryEulerian4CTest::getSampleRotationMatrix(void)
{
    m_geometry.get_axe("omega").set_value(90. * constant::math::degToRad);

    smatrix M( 0., 0.,-1.,
               0., 1., 0.,
               1., 0., 0.);

    CPPUNIT_ASSERT_EQUAL(M, m_geometry.getSampleRotationMatrix());
}

void
GeometryEulerian4CTest::getQ(void)
{
    m_geometry.get_axe("2theta").set_value(0. * constant::math::degToRad);
    CPPUNIT_ASSERT_EQUAL(svector(0., 0., 0.), m_geometry.getQ());

    m_geometry.get_axe("2theta").set_value(45. * constant::math::degToRad);
    CPPUNIT_ASSERT_EQUAL(svector(1./sqrt(2)-1., 0, sqrt(2.)/2.), m_geometry.getQ());
}

void
GeometryEulerian4CTest::getDistance(void)
{
    geometry::Eulerian4C g1(10 * constant::math::degToRad,
                            20 * constant::math::degToRad,
                            30 * constant::math::degToRad,
                            40 * constant::math::degToRad);

    geometry::Eulerian4C g2(11 * constant::math::degToRad,
                            21 * constant::math::degToRad,
                            31 * constant::math::degToRad,
                            41 * constant::math::degToRad);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4. * constant::math::degToRad, g1.getDistance(g2), constant::math::epsilon_0);

    g2.get_axe("omega").set_value(10 * constant::math::degToRad);
    g2.get_axe("chi").set_value(20 * constant::math::degToRad);
    g2.get_axe("phi").set_value(30 * constant::math::degToRad);
    g2.get_axe("2theta").set_value(40 * constant::math::degToRad);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0. * constant::math::degToRad, g1.getDistance(g2), constant::math::epsilon_0);
}

void
GeometryEulerian4CTest::persistanceIO(void)
{
    geometry::Eulerian4C geometry1;
    geometry::Eulerian4C geometry2;  
    stringstream flux;

    m_geometry.toStream(flux);
    m_geometry.toStream(flux);  
    geometry1.fromStream(flux);
    geometry2.fromStream(flux);

    CPPUNIT_ASSERT_EQUAL(m_geometry, geometry1);
    CPPUNIT_ASSERT_EQUAL(m_geometry, geometry2);
}