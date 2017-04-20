#include "dht.hpp"

namespace p = platform;

namespace dht
{

    class DHTSensor {

        public:
            DHTSensor(p::AnalogPin p);

            unsigned int readTemperature(void) {
                return 0;
            }

            unsigned int readHumidity(void) {
                return 0;
            }
    };

}
