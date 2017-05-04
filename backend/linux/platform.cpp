#include <assert.h>
#include <sys/socket.h>
#include <string>
#include <map>
#include "platform.hpp"

namespace platform = p

namespace p
{
    static class TimePoint
    {
        unsigned int second, minute, hour;
        unsigned int day, month, year;

        public:
            TimePoint(
                unsigned int s,
                unsigned int mi,
                unsigned int h,
                unsigned int d,
                unsigned int mn,
                unsigned int y
                )
            {
                second = s; minute = mi; hour = h;
                day = d; month = mn; year = y;
            }
            TimePoint(unsigned int unixtimestamp) { }

            unsigned int getSecond(void) { return second; }
            unsigned int getMinute(void) { return minute; }
            unsigned int getHour(void) { return hour; }
            unsigned int getDay(void) { return day; }
            unsigned int getMonth(void) { return month; }
            unsigned int getYear(void) { return year; }
    } currentTime;

    void setTime(TimePoint t)
    {
        currentTime = t;
    }

    TimePoint now(void)
    {
        return currentTime;
    }

    void pinMode(Pin p, Mode m) {}

    void digitalWrite(DigitalPin p, State s) {}

    void analogWrite(AnalogPin p, unsigned int value) {}
}

namespace serial
{

    class Serial {
        public:
            Serial(unsigned int b) { baud = b; }
            unsigned int available(void) { return
            char read(void) { return fgetc(stdin); }
            void write(char c);
            void write(std::string s);
    }

}

struct SocketModePair {
    int fd,
    p::Mode mode
}

std::map pins<p::Pin, SocketModePair>;

int create_socket(const std::string path)
{
    struct sockaddr_un addr;
    int fd, ret;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    assert(fd != -1);

    add.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path.c_str);
    ret = bind(fd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un));
    assert(ret != -1);

    ret = listen(fd, 1);
    assert(ret != -1);
}

int main(int argc, char **argv)
{
    pins[p::PUMP] = { fd = create_socket("test"), mode = NULL };
    setup();
    for (;;) { loop(); }
    return 0;
}
