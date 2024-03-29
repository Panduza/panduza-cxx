#include <gtest/gtest.h>
#include <pza/core/Client.hxx>
#include <pza/interfaces/Psu.hxx>

using namespace pza;

// Using PSU as an example, but this applies to all interfaces

class InterfaceTest : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        const char *props = std::getenv("PROPS_PATH");

        if (!props)
            props = "";
        Core::RemoveAliases();
        Core::LoadAliasesFromFile(props + std::string("alias/good.json"));
        client = std::make_unique<Client>("good");
        ASSERT_EQ(client->connect(), 0);
        psu = std::make_unique<Psu>("psu");
        std::cout << client.get() << std::endl;
        psu->bindToClient(client.get());
        ASSERT_TRUE(psu->isRunning());
    }

    std::unique_ptr<Client> client;
    std::unique_ptr<Psu> psu;
};

TEST_F(InterfaceTest, Disconnect)
{
    EXPECT_EQ(client->disconnect(), 0);
    EXPECT_FALSE(psu->isRunning());
}

TEST_F(InterfaceTest, DisconnectAndConnect)
{
    EXPECT_EQ(client->disconnect(), 0);
    EXPECT_FALSE(psu->isRunning());
    EXPECT_EQ(client->connect(), 0);
    EXPECT_TRUE(psu->isRunning());
}

TEST_F(InterfaceTest, Reconnect)
{
    EXPECT_EQ(client->disconnect(), 0);
    EXPECT_FALSE(psu->isRunning());
    EXPECT_EQ(client->reconnect(), 0);
    EXPECT_TRUE(psu->isRunning());
}