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
                "psu" : "pza/test/power/psu_1"
            }
        }
    })");
    
    pza::Core::ShowAliases();

    auto client = std::make_unique<pza::Client>("local");
    client->connect();

    auto psu = std::make_unique<pza::Psu>("psu");
    psu->bindToClient(client.get());

    psu->enable.value.set(false);
    psu->enable.value.set(true);
    psu->enable.value.set(false);
    return 0;
}
