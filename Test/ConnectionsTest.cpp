#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../Core/private/Connections.hpp"
#include "../Core/private/TypeSystem.hpp"
#include "../Core/private/ObjectManager.hpp"
#include "../Core/private/EmptyValueProvider.hpp"

class ConnectionsTest
{
public:
   ConnectionsTest()
   {
      mTypeSystem.add({"object", "objects"});
      system("rm Test.db");
      mDb.reset(new materia::Database("Test.db"));
      mConnections = std::make_unique<materia::Connections>(*mDb);
      mOm = std::make_unique<materia::ObjectManager>(*mDb, mTypeSystem, *mConnections);
      mOm->initialize();
   }

protected:
   std::unique_ptr<materia::Database> mDb;
   std::unique_ptr<materia::Connections> mConnections;
   materia::TypeSystem mTypeSystem;
   std::unique_ptr<materia::ObjectManager> mOm;
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

BOOST_FIXTURE_TEST_CASE( TestHierarchy, ConnectionsTest ) 
{
    materia::Id parent = materia::Id::generate();
    materia::Id child = materia::Id::generate();
    materia::Id grandchild = materia::Id::generate();

    mConnections->create(parent, child, materia::ConnectionType::Hierarchy);    
    mConnections->create(child, grandchild, materia::ConnectionType::Hierarchy);    
    BOOST_CHECK_THROW(mConnections->create(child, parent, materia::ConnectionType::Hierarchy), std::runtime_error);    
    BOOST_CHECK_THROW(mConnections->create(grandchild, parent, materia::ConnectionType::Hierarchy), std::runtime_error);    
}

BOOST_FIXTURE_TEST_CASE( TestHierarchy_ObjectRemove, ConnectionsTest ) 
{
    materia::EmptyValueProvider empty;
    auto p1 = materia::Id::generate();
    auto p2 = materia::Id::generate();
    auto c1 = materia::Id::generate();
    auto c2 = materia::Id::generate();
    auto c3 = materia::Id::generate();
    auto cc1 = materia::Id::generate();
    auto cc2 = materia::Id::generate();

    mOm->create(p1, "object", empty);
    mOm->create(p2, "object", empty);
    mOm->create(c1, "object", empty);
    mOm->create(c2, "object", empty);
    mOm->create(c3, "object", empty);
    mOm->create(cc1, "object", empty);
    mOm->create(cc2, "object", empty);

    mConnections->create(p1, c1, materia::ConnectionType::Hierarchy);    
    mConnections->create(p1, c2, materia::ConnectionType::Hierarchy);    
    mConnections->create(c1, cc1, materia::ConnectionType::Hierarchy);    
    mConnections->create(c2, cc2, materia::ConnectionType::Hierarchy);    
    mConnections->create(p2, c3, materia::ConnectionType::Hierarchy);    

    mOm->destroy(p1);
    BOOST_CHECK(mOm->query({p1}).empty());
    BOOST_CHECK(mOm->query({c1}).empty());
    BOOST_CHECK(mOm->query({c2}).empty());
    BOOST_CHECK(mOm->query({cc1}).empty());
    BOOST_CHECK(mOm->query({cc2}).empty());
    BOOST_CHECK(!mOm->query({p2}).empty());
    BOOST_CHECK(!mOm->query({c3}).empty());

    BOOST_CHECK_EQUAL(0, mConnections->get(p1).size());
    BOOST_CHECK_EQUAL(0, mConnections->get(c1).size());
    BOOST_CHECK_EQUAL(0, mConnections->get(c2).size());
    BOOST_CHECK_EQUAL(0, mConnections->get(cc1).size());
    BOOST_CHECK_EQUAL(0, mConnections->get(cc2).size());
    BOOST_CHECK_EQUAL(1, mConnections->get(p2).size());
    BOOST_CHECK_EQUAL(1, mConnections->get(c3).size());
}

BOOST_FIXTURE_TEST_CASE( TestExtension, ConnectionsTest ) 
{
    materia::Id parent = materia::Id::generate();
    materia::Id ex = materia::Id::generate();
    materia::Id exOfEx = materia::Id::generate();

    mConnections->create(parent, ex, materia::ConnectionType::Extension);    
    mConnections->create(ex, exOfEx, materia::ConnectionType::Extension);    
    BOOST_CHECK_THROW(mConnections->create(ex, parent, materia::ConnectionType::Extension), std::runtime_error);    
    BOOST_CHECK_THROW(mConnections->create(exOfEx, parent, materia::ConnectionType::Extension), std::runtime_error);    
    BOOST_CHECK_THROW(mConnections->create(parent, materia::Id::generate(), materia::ConnectionType::Extension), std::runtime_error);    
}

BOOST_FIXTURE_TEST_CASE( TestExtension_ObjectRemove, ConnectionsTest ) 
{
    materia::EmptyValueProvider empty;
    auto p1 = materia::Id::generate();
    auto p2 = materia::Id::generate();
    auto e1 = materia::Id::generate();
    auto e2 = materia::Id::generate();
    auto e3 = materia::Id::generate();

    mOm->create(p1, "object", empty);
    mOm->create(p2, "object", empty);
    mOm->create(e1, "object", empty);
    mOm->create(e2, "object", empty);
    mOm->create(e3, "object", empty);

    mConnections->create(p1, e1, materia::ConnectionType::Extension);    
    mConnections->create(e1, e2, materia::ConnectionType::Extension);    
    mConnections->create(p2, e3, materia::ConnectionType::Extension);    

    mOm->destroy(p1);
    BOOST_CHECK(mOm->query({p1}).empty());
    BOOST_CHECK(mOm->query({e1}).empty());
    BOOST_CHECK(mOm->query({e2}).empty());
    BOOST_CHECK(!mOm->query({p2}).empty());
    BOOST_CHECK(!mOm->query({e3}).empty());

    BOOST_CHECK_EQUAL(0, mConnections->get(p1).size());
    BOOST_CHECK_EQUAL(0, mConnections->get(e1).size());
    BOOST_CHECK_EQUAL(0, mConnections->get(e2).size());
    BOOST_CHECK_EQUAL(1, mConnections->get(p2).size());
    BOOST_CHECK_EQUAL(1, mConnections->get(e3).size());
}
