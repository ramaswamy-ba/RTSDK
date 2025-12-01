/*Perfect — let’s extend the consumer example with a custom OmmConsumerClient 
implementation so you can actually see the market data flowing in. 
This client will print refresh, update, and status messages to stdout.
Full Example: EMA Consumer with Callback
*/

#include "Ema.h"
#include <iostream>

using namespace refinitiv::ema::access;

// Custom client to handle incoming messages
class AppClient : public OmmConsumerClient
{
public:
    void onRefreshMsg(const RefreshMsg& refreshMsg, const OmmConsumerEvent& event) override {
        std::cout << "Received RefreshMsg for item: " 
                  << refreshMsg.getName() << std::endl;
        std::cout << refreshMsg << std::endl;
    }

    void onUpdateMsg(const UpdateMsg& updateMsg, const OmmConsumerEvent& event) override {
        std::cout << "Received UpdateMsg for item: " 
                  << updateMsg.getName() << std::endl;
        std::cout << updateMsg << std::endl;
    }

    void onStatusMsg(const StatusMsg& statusMsg, const OmmConsumerEvent& event) override {
        std::cout << "Received StatusMsg for item: " 
                  << statusMsg.getName() << std::endl;
        std::cout << statusMsg << std::endl;
    }
};

int main(int argc, char* argv[])
{
    // Parameters (replace with your values)
    EmaString host("localhost:14002");     // Host:Port of ADS or Refinitiv Real-Time
    EmaString appId("256");                // Application ID
    EmaString user("myUser");              // DACS username
    EmaString serviceName("ELEKTRON_DD");  // Service name
    EmaString symbol("TRI.N");             // RIC (instrument)

    // Configure consumer
    OmmConsumerConfig config;
    config.host(host).username(user).applicationId(appId);

    // Create consumer
    OmmConsumer consumer(config);

    // Create client
    AppClient client;

    // Request market price data
    ReqMsg req;
    req.serviceName(serviceName).name(symbol);

    consumer.registerClient(req, client);

    // Run for a while to receive updates
    sleep(60000);  // 60 seconds

    return 0;
}
