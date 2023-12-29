#ifndef SimpleButton_MCP23017_h
#define SimpleButton_MCP23017_h

#include "GPIOExpander.h"

namespace simplebutton {
    class MCP23017 : public GPIOExpander {
        public:
            MCP23017(uint8_t адреса);
            MCP23017(uint8_t адреса, TwoWire* провід);

            ~MCP23017();

            void налаштування(uint8_t адреса);
            void налаштування(uint8_t адреса, TwoWire* провід);

            int читання();
            int читання(uint8_t пін);

            void запис(int значення);
            void запис(uint8_t пін, bool значення);

            void змінити();
            void змінити(uint8_t пін);

        private:
            uint16_t даніПінів    = 0x0000;
            uint16_t режимиПінів   = 0x0000;
            uint16_t підтягувачіПінів = 0x0000;

            void встановитиВВ();
            void встановитиПідтягувачі();

            void встановитиРежимПіна(uint8_t пін, uint8_t режим);
            uint8_t отриматиРежимПіна(uint8_t пін);

            bool отриматиСтанПіна(uint8_t пін);

            uint8_t читатиРегістр8(uint8_t адреса);
            uint16_t читатиРегістр16(uint8_t адреса);

            void записатиРегістр(uint8_t адреса, uint16_t значення);
    };
}

#endif // ifndef SimpleButton_MCP23017_h
