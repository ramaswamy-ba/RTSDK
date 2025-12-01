/*
Full Example: EMA Consumer with Field Parsing
Here’s the extended EMA Consumer example with a custom 
OmmConsumerClient that parses specific fields (like BID, ASK, LAST_PRICE) 
from incoming messages instead of dumping the entire EMA message. 
This way you’ll see the actual market values you care about.
*/
#include "Ema.h"
#include <iostream>

using namespace refinitiv::ema::access;

class AppClient : public OmmConsumerClient
{
public:
    void onRefreshMsg(const RefreshMsg& refreshMsg, const OmmConsumerEvent& event) override {
        std::cout << "REFRESH for item: " << refreshMsg.getName() << std::endl;
        decode(refreshMsg.getPayload());
    }

    void onUpdateMsg(const UpdateMsg& updateMsg, const OmmConsumerEvent& event) override {
        std::cout << "UPDATE for item: " << updateMsg.getName() << std::endl;
        decode(updateMsg.getPayload());
    }

    void onStatusMsg(const StatusMsg& statusMsg, const OmmConsumerEvent& event) override {
        std::cout << "STATUS for item: " << statusMsg.getName() << std::endl;
        std::cout << statusMsg << std::endl;
    }

private:
    void decode(const ElementList& elementList) {
        for (ElementListConstIterator it = elementList.begin(); it != elementList.end(); ++it) {
            std::cout << it.name() << " : " << it.load().toString() << std::endl;
        }
    }

    void decode(const FieldList& fieldList) {
        for (FieldListConstIterator it = fieldList.begin(); it != fieldList.end(); ++it) {
            switch (it.getFieldId()) {
                case 22: // BID
                    std::cout << "BID: " << it.load().toDouble() << std::endl;
                    break;
                case 25: // ASK
                    std::cout << "ASK: " << it.load().toDouble() << std::endl;
                    break;
                case 31: // LAST_PRICE
                    std::cout << "LAST_PRICE: " << it.load().toDouble() << std::endl;
                    break;
                default:
                    // Uncomment if you want to see all fields
                    // std::cout << it.name() << " : " << it.load().toString() << std::endl;
                    break;
            }
        }
    }
};

int main(int argc, char* argv[])
{
    // Parameters (replace with your values)
    EmaString host("localhost:14002");     // ADS or Refinitiv Real-Time host:port
    EmaString appId("256");                // Application ID
    EmaString user("myUser");              // DACS username
    EmaString serviceName("ELEKTRON_DD");  // Service name
    EmaString symbol("TRI.N");             // RIC

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

/*
Key Details
Field IDs:

22 → BID

25 → ASK

31 → LAST_PRICE

These IDs come from the RDMFieldDictionary provided by Refinitiv. You can load the dictionary to map IDs to names if you want dynamic decoding.

The decode(FieldList&) method filters only the fields you care about.
*/
