#ifndef PROMAKE_PCA95X4_H_INCLUDED
#define PROMAKE_PCA95X4_H_INCLUDED

#include "ProMake_Core.h"

namespace ProMake
{
    template <typename WireType = TwoWire>
    class PCA95x4
    {
        static constexpr uint8_t BASE_I2C_ADDR = 0x38;
        
        WireType *wire{nullptr};
        uint8_t addr{BASE_I2C_ADDR};
        uint8_t input{0x00};
        uint8_t output{0xFF};
        uint8_t pol{0x00};
        uint8_t dir{0xFF};
        uint8_t status{0x00};

    public:
        enum Reg : uint8_t
        {
            INPUT_PORT_0,
            OUTPUT_PORT_0,
            POLARITY_INVERSION_PORT_0,
            CONFIGURATION_PORT_0,
        };

        enum Port : uint8_t
        {
            P0,
            P1,
            P2,
            P3,
            P4,
            P5,
            P6,
            P7
        };

        enum Level : uint8_t
        {
            L,
            H
        };
        enum LevelAll : uint8_t
        {
            L_ALL = 0x00,
            H_ALL = 0xFF
        };

        enum Polarity : uint8_t
        {
            ORIGINAL,
            INVERTED
        };
        enum PolarityAll : uint8_t
        {
            ORIGINAL_ALL = 0x00,
            INVERTED_ALL = 0xFF
        };

        enum Direction : uint8_t
        {
            OUT,
            IN
        };
        enum DirectionAll : uint8_t
        {
            OUT_ALL = 0x00,
            IN_ALL = 0xFF
        };

    public:
        void attach(WireType &wire, uint8_t i2c_addr = BASE_I2C_ADDR)
        {
            this->wire = &wire;
            this->addr = i2c_addr;
        }

        uint8_t read()
        {
            read_byte(this->addr, Reg::INPUT_PORT_0, this->input);
            return this->input;
        }
        Level read(const Port port)
        {
            uint8_t v = read();
            return (v & (1 << port)) ? Level::H : Level::L;
        }

        bool write(const uint8_t value)
        {
            this->output = value;
            return write_impl();
        }
        bool write(const Port port, const Level level)
        {
            if (level == Level::H)
            {
                this->output |= (1 << port);
            }
            else
            {
                this->output &= ~(1 << port);
            }
            return write_impl();
        }

        bool polarity(const uint8_t value)
        {
            this->pol = value;
            return polarity_impl();
        }
        bool polarity(const Port port, const Polarity pol)
        {
            if (pol == Polarity::INVERTED)
            {
                this->pol |= (1 << port);
            }
            else
            {
                this->pol &= ~(1 << port);
            }
            return polarity_impl();
        }

        bool direction(const uint8_t value)
        {
            this->dir = value;
            return direction_impl();
        }

        bool direction(const Port port, const Direction dir)
        {
            if (dir == Direction::IN)
            {
                this->dir |= (1 << port);
            }
            else
            {
                this->dir &= ~(1 << port);
            }
            return direction_impl();
        }

        uint8_t i2c_error() const
        {
            return status;
        }

    private:
        bool write_impl()
        {
            return write_byte(this->addr, Reg::OUTPUT_PORT_0, this->output);
        }

        bool polarity_impl()
        {
            return write_byte(this->addr, Reg::POLARITY_INVERSION_PORT_0, this->pol);
        }

        bool direction_impl()
        {
            return write_byte(this->addr, Reg::CONFIGURATION_PORT_0, this->dir);
        }

        int8_t read_byte(const uint8_t dev, const uint8_t reg, uint8_t &data)
        {
            wire->beginTransmission(dev);
            wire->write(reg);
            wire->endTransmission();
            wire->requestFrom(dev, 1);
            int8_t count = 0;
            while (wire->available())
                data = wire->read();
            return count;
        }

        bool write_byte(const uint8_t dev, const uint8_t reg, const uint8_t data)
        {
            wire->beginTransmission(dev);
            wire->write(reg);
            wire->write(data);
            status = wire->endTransmission();
            return (status == 0);
        }
    };
    using PCA9554 = PCA95x4<>;

} // namespace ProMake

#endif