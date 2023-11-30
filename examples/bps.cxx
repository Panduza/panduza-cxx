#include <unistd.h>

#include <pza/core/client.hxx>

int main()
{
    pza::client::s_ptr cli = std::make_shared<pza::client>("localhost", 1883);

    if (cli->connect() < 0)
        return -1;

    auto dev = cli->register_device("default", "Panduza_FakeBps");
}
