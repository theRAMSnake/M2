#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../Core/private/Connections.hpp"

class ConnectionsTest
{
public:
   ConnectionsTest()
   {
      system("rm Test.db");
      mDb.reset(new materia::Database("Test.db"));
      mConnections = std::make_unique<materia::Connections>(*mDb);
   }

protected:
   std::unique_ptr<materia::Database> mDb;
   std::unique_ptr<materia::Connections> mConnections;
};

BOOST_FIXTURE_TEST_CASE( TestSimpleSuccess, ConnectionsTest ) 
{ 
    materia::Id parent = materia::Id::generate();
    mConnections->create(parent, materia::Id::generate(), materia::ConnectionType::Hierarchy);    
    mConnections->create(parent, materia::Id::generate(), materia::ConnectionType::Hierarchy);    
    mConnections->create(parent, materia::Id::generate(), materia::ConnectionType::Hierarchy);    
    mConnections->create(parent, materia::Id::generate(), materia::ConnectionType::Hierarchy);    
    
    BOOST_CHECK_EQUAL(4, mConnections->get(parent).size());
}

BOOST_FIXTURE_TEST_CASE( TestSaveRestore, ConnectionsTest ) 
{
    materia::Id parent = materia::Id::generate();
    mConnections->create(parent, materia::Id::generate(), materia::ConnectionType::Hierarchy);    
    mConnections->create(parent, materia::Id::generate(), materia::ConnectionType::Extension);    
    mConnections->create(parent, materia::Id::generate(), materia::ConnectionType::Reference);    
    mConnections->create(parent, materia::Id::generate(), materia::ConnectionType::Requirement);    

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    materia::Connections restored(*mDb);
    BOOST_CHECK_EQUAL(4, restored.get(parent).size());

    auto dst = restored.get(parent);
    auto src = mConnections->get(parent);
    for(std::size_t i = 0; i < dst.size(); ++i)
    {
        BOOST_CHECK(src[i].id == dst[i].id);
        BOOST_CHECK(src[i].a == dst[i].a);
        BOOST_CHECK(src[i].b == dst[i].b);
        BOOST_CHECK(src[i].type == dst[i].type);
    }
}

BOOST_FIXTURE_TEST_CASE( TestNonunique, ConnectionsTest ) 
{
    materia::Id parent = materia::Id::generate();
    materia::Id child = materia::Id::generate();
    mConnections->create(parent, child, materia::ConnectionType::Hierarchy);
    BOOST_CHECK_THROW(mConnections->create(parent, child, materia::ConnectionType::Hierarchy), std::runtime_error);    
    
    BOOST_CHECK_EQUAL(1, mConnections->get(parent).size());
}

BOOST_FIXTURE_TEST_CASE( TestAisB, ConnectionsTest ) 
{
    materia::Id parent = materia::Id::generate();

    BOOST_CHECK_THROW(mConnections->create(parent, parent, materia::ConnectionType::Hierarchy), std::runtime_error);    
    
    BOOST_CHECK_EQUAL(0, mConnections->get(parent).size());
}
