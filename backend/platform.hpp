namespace platform
{
    enum DigitalPin {PUMP, NOZZLE, FAN, BOARDLED};
    enum AnalogPin {RED, GREEN, BLUE, DHT};
    enum Mode {INPUT, OUTPUT};
    enum State {LOW, HIGH};

    union Pin {DigitalPin; AnalogPin};

    class TimePoint {
        unsigned int second, minute, hour;
        unsigned int day, month, year;

        public:
            /**
             * Create an object representing a point in time
             *
             * @param hour from 0 to 23
             * @param minute from 0 to 59
             * @param second from 0 to 59
             * @param day from 1 to 31 depending on the month
             * @param month from 1 to 12
             * @param year from 0 to UINT_MAX
             */
            TimePoint(
                unsigned int second,
                unsigned int minute,
                unsigned int hour,
                unsigned int day,
                unsigned int month,
                unsigned int year
                );
            TimePoint(unsigned int unixtimestamp);

            unsigned int getSecond(void);
            unsigned int getMinute(void);
            unsigned int getHour(void);
            unsigned int getDay(void);
            unsigned int getMonth(void);
            unsigned int getYear(void);
    };

    /**
     * Set global clock to a certain time
     *
     * @param TimePoint t
     */
    void setTime(TimePoint t);

    /**
     * Return a point in time for when this function is called.
     *
     * @return TimePoint
     */
    TimePoint now(void);

    /**
     * Set pin to be either input or output
     *
     * @param Pin p
     * @param Mode m
     */
    void pinMode(Pin p, Mode m);

    /**
     * Write state to pin.
     *
     * @param DigitalPin p
     * @param State s
     */
    void digitalWrite(DigitalPin p, State s);

    /**
     * Write value to pin.
     *
     * @param AnalogPin p
     * @param unsigned int value
     */
    void analogWrite(AnalogPin p, unsigned int value);
}
