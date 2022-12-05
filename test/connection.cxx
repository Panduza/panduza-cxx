#include <gtest/gtest.h>
#include <pza/core/Client.hxx>

using namespace pza;

class BaseClient : public ::testing::Test,
                   public ::testing::WithParamInterface<std::pair<std::string, int>>
{
protected:
    virtual void SetUp()
    {
        auto url = GetParam().first;
        auto port = GetParam().second;
        client = new Client(url, port);
    }

    Client *client;
};

class BaseClientAlias : public ::testing::Test
{
protected:
    
    std::unique_ptr<Client> createClient(const std::string &alias)
    {
        return std::make_unique<Client>(alias);
    }

    void loadAlias(const std::string &json)
    {
        Core::RemoveAliases();
        Core::LoadAliases(json);
    }

    std::unique_ptr<Client> client;
};

using BaseConnSuccess = BaseClient;
using BaseConnFail = BaseClient;

TEST_P(BaseConnSuccess, ConnectSuccess)
{
    EXPECT_EQ(client->connect(), 0);
}

TEST_P(BaseConnFail, ConnectFail)
{
    EXPECT_EQ(client->connect(), -1);
}

TEST_P(BaseConnSuccess, DisconnectSuccess)
{
    EXPECT_EQ(client->connect(), 0);
    EXPECT_EQ(client->disconnect(), 0);
}

TEST_P(BaseConnFail, DisconnectFail)
{
    EXPECT_EQ(client->connect(), -1);
    EXPECT_EQ(client->disconnect(), -1);
}

TEST_P(BaseConnSuccess, ReconnectSuccess)
{
    EXPECT_EQ(client->connect(), 0);
    EXPECT_EQ(client->reconnect(), 0);
    EXPECT_EQ(client->disconnect(), 0);
    EXPECT_EQ(client->reconnect(), 0);
}

TEST_P(BaseConnFail, ReconnectFail)
{
    EXPECT_EQ(client->connect(), -1);
    EXPECT_EQ(client->reconnect(), -1);
    EXPECT_EQ(client->disconnect(), -1);
    EXPECT_EQ(client->reconnect(), -1);
}

INSTANTIATE_TEST_SUITE_P(TestConnectionSuccess, BaseConnSuccess,
                         ::testing::Values(
                             std::make_pair("localhost", 1883),
                             std::make_pair("127.0.0.1", 1883)));

INSTANTIATE_TEST_SUITE_P(TestConnectionFailure, BaseConnFail,
                         ::testing::Values(
                             std::make_pair("badlocalhost", 1883),
                             std::make_pair("", 1883),
                             std::make_pair("localhost", -1)));

TEST_F(BaseClientAlias, ConnectSuccess)
{
    loadAlias(R"({
        "local": {
            "url": "localhost",
            "port": 1883
        }
    })");
    auto client = createClient("local");
    EXPECT_EQ(client->connect(), 0);
}

TEST_F(BaseClientAlias, ConnectBadFormat)
{
    loadAlias(R"({
        "local": {
            "ur": "localhost",
            "port": 1883
        }
    })");
    auto client = createClient("local");
    EXPECT_EQ(client->connect(), -1);
}

TEST_F(BaseClientAlias, ConnectDoesNotExist)
{
    auto client = createClient("nothing");
    EXPECT_EQ(client->connect(), -1);
}

TEST_F(BaseClientAlias, ConnectMultiple)
{
    loadAlias(R"({
        "local": {
            "url": "localhost",
            "port": 1883
        },
        "local2": {
            "url": "localhost",
            "port": 1883
        }
    })");

    auto client = createClient("local");
    auto client2 = createClient("local2");

    EXPECT_EQ(client->connect(), 0);
    EXPECT_EQ(client2->connect(), 0);
}

TEST_F(BaseClientAlias, ConnectToBadAndResetToAlias)
{
    loadAlias(R"({
        "local": {
            "url": "localhost",
            "prt": 1883
        }
    })");

    auto client = createClient("local");
    EXPECT_EQ(client->connect(), -1);

    loadAlias(R"({
        "local": {
            "url": "localhost",
            "port": 1883
        }
    })");

    client->resetAlias("local");
    EXPECT_EQ(client->connect(), 0);
}

TEST_F(BaseClientAlias, ConnectToBadAndResetToRaw)
{
    loadAlias(R"({
        "local": {
            "url": "localhost",
            "prt": 1883
        }
    })");

    auto client = createClient("local");
    EXPECT_EQ(client->connect(), -1);
    client->reset("localhost", "1883");
    EXPECT_EQ(client->connect(), 0);
}