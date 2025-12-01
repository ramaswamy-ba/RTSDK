//Example: EMA Consumer with Parameters
#include "Ema.h"

using namespace refinitiv::ema::access;

int main(int argc, char* argv[])
{
    // Parameters (replace with your values)
    EmaString host("hostname:port");   // e.g. "localhost:14002"
    EmaString appId("256");            // Application ID
    EmaString user("myUser");          // DACS username
    EmaString serviceName("ELEKTRON_DD"); // Service name
    EmaString symbol("TRI.N");         // Instrument / RIC

    // Configure consumer
    OmmConsumerConfig config;
    config.host(host).username(user).applicationId(appId);

    // Create consumer
    OmmConsumer consumer(config);

    // Request market price data
    ReqMsg req;
    req.serviceName(serviceName).name(symbol);

    consumer.registerClient(req, OmmConsumerClient());

    // Run for a while to receive updates
    sleep(60000);  // 60 seconds

    return 0;
}
/*
-----------------------

Key Points
OmmConsumerConfig → sets host, port, appId, and user.

ReqMsg → specifies service and symbol (RIC).

registerClient → sends the request and attaches a callback client.

OmmConsumerClient → you implement this class to handle incoming data (onRefreshMsg, onUpdateMsg, onStatusMsg).
*/

