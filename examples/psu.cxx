#include <pza/core/Client.hxx>
#include <pza/interfaces/Psu.hxx>
#include <thread>

int main(void)
{
    pza::Core::SetLogLevel(pza::Core::LogLevel::Trace);
    pza::Core::LoadAliases(R"({
        "local": {
            "url": "localhost",
            "port": 1883,
            "interfaces":
            {
                "psu" : "pza/machine/py.psu.fake/My Psu"
            }
        }
    })");
    
    pza::Core::ShowAliases();

    auto client = std::make_unique<pza::Client>("local");
    client->connect();

    auto psu = std::make_unique<pza::Psu>("psu");
    psu->bindToClient(client.get());

    psu->volts.goal.set(4.2);

    return 0;
}