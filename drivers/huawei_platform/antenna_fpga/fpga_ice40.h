#ifndef _FPGA_ICE40_SPI_H_
#define _FPGA_ICE40_SPI_H_

#define DRIVER_NAME "fpga_ice40_spi"
#define FPGA_SPI__FIFO_SIZE 512
#define FPGA_SPI_TRAN_SIZE 8192
#define FAIL 1
#define SUCCESS 0
#define ENABLE 1
#define DISABLE 0
#define GPIO_OEM_UNKNOW (-1)
#define GPIO_OEM_VALID(gpio) ((gpio == GPIO_OEM_UNKNOW) ? 0 : 1)

int ice40_set_cs_callback(int value, int cs_gpio);
int ice40_cs_set(u32 control);
int ice40_spi_exit(void);
int ice40_data_spi_write(u8 *data, u32 data_size);
int ice40_data_spi_write_then_read(u8 *indata, u32 indata_size, u8* outdata, u32 outdata_size);
int ice40_data_spi_recv(u8 *data, u32 len);

#endif
