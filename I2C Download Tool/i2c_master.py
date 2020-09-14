import wiringpi

#I2C master interface for the bootloader
#Commands used by the bootloader are as follows:
GET_FLASH_POINTER_COMMAND = 0x01
SET_FLASH_POINTER_COMMAND = 0x01
RECEIVE_FLASH_DATA_COMMAND = 0x02
READ_FLASH_COMMAND = 0x03
ERASE_FLASH_ROW_COMMAND = 0x04
WRITE_BUFFER_TO_FLASH_COMMAND = 0x05
JUMP_TO_APPLICATION_COMMAND = 0x06
PING_COMMAND = 0xAA

class I2C_Bootloader_Interface():

    def __init__(self, device_address):
        #do initial wiringpi library setup
        self.__wiringpisetup = wiringpi.wiringPiSetup()
        self.__i2c_device = wiringpi.wiringPiI2CSetup(device_address)
    
    def set_address_pointer(self, address_to_set):
        #write the high byte of the address
        wiringpi.wiringPiI2CWriteReg8(self.__i2c_device, SET_FLASH_POINTER_COMMAND, address_to_set >> 8)
        #write the low byte of the address
        wiringpi.wiringPiI2CWriteReg8(self.__i2c_device, address_to_set & 0xFF)
        
    def get_address_pointer(self):
        #read the high byte of the address, shift it up and fill the bottom with 1's 0x{byte}FF
        address_h = 0xFF | (wiringpi.wiringPiI2CReadReg8(self.__i2c_device, GET_FLASH_POINTER_COMMAND) << 8)
        #read the low byte of the address into an address with the high byte all 1's 0xFF{byte}
        address_l = 0xFF00 | wiringpi.wiringPiI2CReadReg8(self.__i2c_device)
        #combine into a 2 byte address and return 
        return address_h & address_l


    