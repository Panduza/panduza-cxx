#include <core/Client.hxx>
#include <core/Core.hxx>
#include <interfaces/Psu.hxx>

int main(void)
{
    Core::SetLogLevel(Core::LogLevel::Trace);

    Core::LoadAliases(
    R"({
        "local": {
            "url": "localhost",
            "port": 1883,
            "interfaces": {
                "test" : "pza/mymachine/py.psu.fake/My Psu"
            }
        }
    })");

    Core::ShowAliases();

    Client client("local");
    
    if (client.connect() == -1)
        return 1;

    client.scan();

    Psu psu(client, "test");

    psu.settings.ovp.set();
    psu.state.value.get();

    while (1)
        ;
    return 0;
}