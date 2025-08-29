
#include "drv8301.hpp"
#include "utils.hpp"
#include "cmsis_os.h"
#include "board.h"

/**
 * 虽然在启动时 SPI 统一初始化了同一的 SPI 模式，但是那个统一的初始化结构体不能适应所有使用了 SPI 的硬件模块，
 * 所以每个使用了 SPI 的硬件模块中会定义一个属于自己模式的 SPI 初始化结构体，并将这个结构体传递到 stm32_spi_arbiter 对象中使用。
 * 所以这就是为什么要在这里定义一个 SPI 初始化结构体。
 */
const SPI_InitTypeDef Drv8301::spi_config_ = {
    .Mode = SPI_MODE_MASTER,
    .Direction = SPI_DIRECTION_2LINES,
    .DataSize = SPI_DATASIZE_16BIT,
    .CLKPolarity = SPI_POLARITY_LOW,
    .CLKPhase = SPI_PHASE_2EDGE,
    .NSS = SPI_NSS_SOFT,
    .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
    .FirstBit = SPI_FIRSTBIT_MSB,
    .TIMode = SPI_TIMODE_DISABLE,
    .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
    .CRCPolynomial = 10,
};

/**
 * 先配置，后初始化，通过配置函数先计算出 regs_ 两个寄存器值，以及一个实际增益值，
 * 同时判断新配置值与原配置值是否相同，相同不动作
 * 两个寄存器值在初始化时才真正写入到寄存器中
 */
bool Drv8301::config(float requested_gain, float* actual_gain) {
    // Calculate gain setting: Snap down to have equal or larger range as
    // requested or largest possible range otherwise

    // for reference:
    // 20V/V on 500uOhm gives a range of +/- 150A
    // 40V/V on 500uOhm gives a range of +/- 75A
    // 20V/V on 666uOhm gives a range of +/- 110A
    // 40V/V on 666uOhm gives a range of +/- 55A

    uint16_t gain_setting = 3;
    float gain_choices[] = {10.0f, 20.0f, 40.0f, 80.0f};
    while (gain_setting && (gain_choices[gain_setting] > requested_gain)) {
        gain_setting--;
    }

    if (actual_gain) {
        *actual_gain = gain_choices[gain_setting];
    }

    RegisterFile new_config;

    new_config.control_register_1 =
          (21 << 6) // Overcurrent set to approximately 150A at 100degC. This may need tweaking.
        | (0b01 << 4) // OCP_MODE: latch shut down
        | (0b0 << 3) // 6x PWM mode
        | (0b0 << 2) // don't reset latched faults
        | (0b00 << 0); // gate-drive peak current: 1.7A

    new_config.control_register_2 =
          (0b0 << 6) // OC_TOFF: cycle by cycle
        | (0b00 << 4) // calibration off (normal operation)
        | (gain_setting << 2) // select gain
        | (0b00 << 0); // report both over temperature and over current on nOCTW pin

    bool regs_equal = (regs_.control_register_1 == new_config.control_register_1)
                   && (regs_.control_register_2 == new_config.control_register_2);

    if (!regs_equal) {
        regs_ = new_config;
        state_ = kStateUninitialized;
        enable_gpio_.write(false);
    }

    return true;
}

/**
 * 将配置阶段得到的两个寄存器值写入 DRV8301 的寄存器中，以生效
 */
bool Drv8301::init() {
    uint16_t val;

    if (state_ == kStateReady) {
        return true;
    }

    // Reset DRV chip. The enable pin also controls the SPI interface, not only
    // the driver stages.
    enable_gpio_.write(false);
    delay_us(40); // mimumum pull-down time for full reset: 20us
    state_ = kStateUninitialized; // make is_ready() ignore transient errors before registers are set up
    enable_gpio_.write(true);
    osDelay(20); // t_spi_ready, max = 10ms

    /**
     * 特别值得注意的是，kRegNameControl1 和 regs_.control_register_1 被重复了五次。
     * 这可能意味着为了确保写入操作成功，代码重复执行了写入操作。注释中提到：如果只执行一次写入操作，那么该操作往往会被忽略（不确定为什么）。
     * 这可能是硬件或设备的一个特定行为，需要多次写入以确保配置生效。
     */
    // Write current configuration, 看注释部分的内容：如果只执行一次，写入操作往往会被忽略（不知道为什么）。
    bool wrote_regs = write_reg(kRegNameControl1, regs_.control_register_1)
                       && write_reg(kRegNameControl1, regs_.control_register_1)
                       && write_reg(kRegNameControl1, regs_.control_register_1)
                       && write_reg(kRegNameControl1, regs_.control_register_1)
                       && write_reg(kRegNameControl1, regs_.control_register_1) // the write operation tends to be ignored if only done once (not sure why)
                       && write_reg(kRegNameControl2, regs_.control_register_2);
    if (!wrote_regs) {
        return false;
    }

    // Wait for configuration to be applied
    delay_us(100);
    state_ = kStateStartupChecks;

    bool is_read_regs = read_reg(kRegNameControl1, &val) && (val == regs_.control_register_1)
                      && read_reg(kRegNameControl2, &val) && (val == regs_.control_register_2);
    if (!is_read_regs) {
        return false;
    }

    if (get_error() != FaultType_NoFault) {
        return false;
    }

    // There could have been an nFAULT edge meanwhile. In this case we shouldn't
    // consider the driver ready.
    CRITICAL_SECTION() {
        if (state_ == kStateStartupChecks) {
            state_ = kStateReady;
        }
    }

    return state_ == kStateReady;
}

void Drv8301::do_checks() {
    if (state_ != kStateUninitialized && !nfault_gpio_.read()) {
        state_ = kStateUninitialized;
    }
}

bool Drv8301::is_ready() {
    return state_ == kStateReady;
}

Drv8301::FaultType_e Drv8301::get_error() {
    uint16_t fault1, fault2;

    if (!read_reg(kRegNameStatus1, &fault1) ||
        !read_reg(kRegNameStatus2, &fault2)) {
        return (FaultType_e)0xffffffff;
    }

    return (FaultType_e)((uint32_t)fault1 | ((uint32_t)(fault2 & 0x0080) << 16));
}

bool Drv8301::read_reg(const RegName_e regName, uint16_t* data) {
    tx_buf_ = build_ctrl_word(DRV8301_CtrlMode_Read, regName, 0);
    if (!spi_arbiter_->transfer(spi_config_, ncs_gpio_, (uint8_t *)(&tx_buf_), nullptr, 1, 1000)) {
        return false;
    }
    
    delay_us(1);

    tx_buf_ = build_ctrl_word(DRV8301_CtrlMode_Read, regName, 0);
    rx_buf_ = 0xffff;
    if (!spi_arbiter_->transfer(spi_config_, ncs_gpio_, (uint8_t *)(&tx_buf_), (uint8_t *)(&rx_buf_), 1, 1000)) {
        return false;
    }

    delay_us(1);

    if (rx_buf_ == 0xbeef) {
        return false;
    }

    if (data) {
        *data = rx_buf_ & 0x07FF;
    }
    
    return true;
}

bool Drv8301::write_reg(const RegName_e regName, const uint16_t data) {
    // Do blocking write
    tx_buf_ = build_ctrl_word(DRV8301_CtrlMode_Write, regName, data);
    if (!spi_arbiter_->transfer(spi_config_, ncs_gpio_, (uint8_t *)(&tx_buf_), nullptr, 1, 1000)) {
        return false;
    }
    delay_us(1);

    return true;
}
