#include "platform.hpp"

namespace p = platform;

namespace dht
{

    class DHTSensor {

        public:
            DHTSensor(p::AnalogPin p);

            unsigned int readTemperature(void);
            unsigned int readHumidity(void);
    };

}
