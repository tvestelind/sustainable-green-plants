#include <string>

namespace serial
{

    class Serial {

        public:
            Serial(unsigned int baud);

            unsigned int available(void);
            char read(void);
            void write(char c);
            void write(std::string s);
    }

}
