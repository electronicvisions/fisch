#pragma once
#include "fisch/vx/omnibus.h"
#include "halco/hicann-dls/vx/omnibus.h"

namespace fisch::vx {

// cf. omnibus_top bus splits
constexpr halco::hicann_dls::vx::OmnibusAddress fpga_omnibus_mask{0x8000'0000};
constexpr halco::hicann_dls::vx::OmnibusAddress external_ppu_memory_base_address{
    0x0000'0000 | fpga_omnibus_mask};
constexpr halco::hicann_dls::vx::OmnibusAddress executor_omnibus_mask{
    0x0800'0000 | external_ppu_memory_base_address};
constexpr halco::hicann_dls::vx::OmnibusAddress ut_omnibus_mask{
    0x0000'4000 | executor_omnibus_mask};
constexpr halco::hicann_dls::vx::OmnibusAddress phy_omnibus_mask{0x0000'2000 | ut_omnibus_mask};
constexpr halco::hicann_dls::vx::OmnibusAddress spi_over_omnibus_mask{
    0x0000'1000 | phy_omnibus_mask};
constexpr halco::hicann_dls::vx::OmnibusAddress i2c_over_omnibus_mask{
    0x0000'0800 | spi_over_omnibus_mask};
constexpr halco::hicann_dls::vx::OmnibusAddress event_switch_mask{
    0x0000'0400 | i2c_over_omnibus_mask};

constexpr halco::hicann_dls::vx::OmnibusAddress external_omnibus_bit_switch{0x0400'0000};
constexpr halco::hicann_dls::vx::OmnibusAddress external_omnibus_base_address{
    external_omnibus_bit_switch | executor_omnibus_mask};

constexpr halco::hicann_dls::vx::OmnibusAddress i2c_prescaler_base_address{i2c_over_omnibus_mask +
                                                                           0x200};

constexpr halco::hicann_dls::vx::OmnibusAddress i2c_eeprom_base_address{i2c_over_omnibus_mask +
                                                                        0x50};

constexpr halco::hicann_dls::vx::OmnibusAddress i2c_ina219_base_address{i2c_over_omnibus_mask +
                                                                        0x40};

constexpr halco::hicann_dls::vx::OmnibusAddress i2c_tca9554_base_address{i2c_over_omnibus_mask +
                                                                         0x38};

constexpr halco::hicann_dls::vx::OmnibusAddress i2c_ad5252_base_address{i2c_over_omnibus_mask +
                                                                        0x2c};

constexpr halco::hicann_dls::vx::OmnibusAddress i2c_dac6573_base_address{i2c_over_omnibus_mask +
                                                                         0x4c};

constexpr Omnibus::Value::Word spi_over_omnibus_stop_bit{0x8000'0000};

/**
 * General Information about the I2C Omnibus interface
 *
 * When the I2C controller receives repeated messages with the same mode and address, it will
 * not re-send the address. This is quite useful for multi-byte read/write operations as it
 * conforms to the spec. of most slave devices.
 *
 * Should two messages with different mode or address follow each other, a repeat-start is
 * automatically inserted. To force a repeat-start, use the flags below.
 * See https://gerrit.bioai.eu/gitweb?p=verilog-i2c.git;a=blob;f=rtl/i2c_master.v;hb=HEAD;js=1#l376
 */

/**
 * Add this to the I2C address to force a repeat start condition before the address byte of the next
 * message.
 */
constexpr Omnibus::Value::Word i2c_over_omnibus_repeat_start{0x100};

/**
 * Add this to the I2C address to force a stop after the data byte of the next message.
 */
constexpr Omnibus::Value::Word i2c_over_omnibus_stop{0x80};

} // namespace fisch::vx
