from smbus2 import SMBus

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

    def __init__(self, i2c_bus, device_address):
        #do initial smbus2 library setup
        self.__i2c_bus = i2c_bus
        self.__i2c_device_address = device_address
        self.__i2c_bus_open = False
    
    def __del__(self):
        if self.__i2c_bus_open:
            self.__i2c_bus.close()

    
    def open_bus(self) -> bool:
        try:
            self.__i2c_bus = SMBus(self.__i2c_bus)
            self.__i2c_bus_open = True
            return True
        except:
            self.__i2c_bus_open = False
            return False

    def close_bus(self):
        if self.__i2c_bus_open:
            self.__i2c_bus.close()
            self.__i2c_bus_open = False

    def set_address_pointer(self, address_to_set, retries) -> bool:
        if self.__i2c_bus_open:
            #split the address into bytes
            address = [address_to_set >> 8, address_to_set & 0xFF]
            try:
                #write the address 
                self.__i2c_bus.write_i2c_block_data(self.__i2c_device_address,SET_FLASH_POINTER_COMMAND,address)
                #validate that the address has been set correctly 
                if address_to_set == self.get_address_pointer():
                    return True
                else:
                    while retries > 0:
                        #write the address 
                        self.__i2c_bus.write_i2c_block_data(self.__i2c_device_address,SET_FLASH_POINTER_COMMAND,address)
                        if address_to_set == self.get_address_pointer():
                            print("retry...", end="")
                            return True
                        retries -= 1
                    return False
            except:
                return False
        else:
            return False
        
    def get_address_pointer(self) -> int:
        if self.__i2c_bus_open:
            try:
                #read the address
                address = self.__i2c_bus.read_i2c_block_data(self.__i2c_device_address,GET_FLASH_POINTER_COMMAND,2)

                #grab high byte of the address, shift it up and fill the bottom with 1's 0x{byte}FF
                address_h = 0xFF | (address[0] << 8)
                #grab the low byte of the address into an address with the high byte all 1's 0xFF{byte}
                address_l = 0xFF00 | address[1]
                #combine into a 2 byte address and return 
                return address_h & address_l
            except:
                return -1
        else:
            return -1

    #note that this read will move the address pointer forward by 8 words
    def read_data(self) -> list:
        if self.__i2c_bus_open:
            try:
                #read data - 16 bytes or 8 words will be read from the current address pointer 
                bytes_read = self.__i2c_bus.read_i2c_block_data(self.__i2c_device_address,READ_FLASH_COMMAND,16)
                #convert the bytes to words and return
                words_to_return = []
                index=0
                while index < len(bytes_read):
                    #grab high byte of the data, shift it up and fill the bottom with 1's 0x{byte}FF
                    #grab the low byte of the data and put in with a high byte all 1's 0xFF{byte}
                    #AND the two parts into a 2 byte word
                    words_to_return.append((0xFF | (bytes_read[index] << 8)) & (0xFF00 | (bytes_read[index+1])))
                    index += 2
                return words_to_return
            except:
                return [-1]
        else:
            return [-1]

    #note that this erase will erase a whole row (32 words on PIC16F1827) and move the address pointer on by this much
    def erase_row(self) -> bool:
        if self.__i2c_bus_open:
            try:
                if self.__i2c_bus.read_i2c_block_data(self.__i2c_device_address,ERASE_FLASH_ROW_COMMAND,1) != [0x00]:
                    return False
                else:
                    return True
            except:
                return False
        else:
            return False
    
    def send_data_to_buffer(self, data_to_send) -> bool:
        if self.__i2c_bus_open:
            #need 8 words to send 
            if len(data_to_send) != 8:
                return False
            
            #split the 8 words into 16 bytes, most significant byte first 
            index=0
            data_to_send_in_bytes=[]
            while index < len(data_to_send):
                #grab most significant byte first and append
                data_to_send_in_bytes.append(data_to_send[index] >> 8)
                #then get least significant byte and append
                data_to_send_in_bytes.append(data_to_send[index] & 0xFF)
                index += 1
            try:
                self.__i2c_bus.write_i2c_block_data(self.__i2c_device_address,RECEIVE_FLASH_DATA_COMMAND,data_to_send_in_bytes)
                return True
            except:
                return False
        else:
            return False

    #note that this write will move the address pointer on by 8 words
    def write_buffer(self) -> bool:
        if self.__i2c_bus_open:
            try:
                if self.__i2c_bus.read_i2c_block_data(self.__i2c_device_address,WRITE_BUFFER_TO_FLASH_COMMAND,1) != [0x00]:
                    return False
                else:
                    return True
            except:
                return False
        else:
            return False
    
    #note that this verify will move the address pointer on by 8 words
    def verify_data(self, chunk_to_verify) -> bool:
        #get the data to verify
        current_data = self.read_data()
        if current_data != [-1]:
            if current_data == chunk_to_verify:
                return True
            else:
                return False
        else:
            return False
    
    def goto_app_mode(self) -> bool:
        if self.__i2c_bus_open:
            try:
                if self.__i2c_bus.read_i2c_block_data(self.__i2c_device_address,JUMP_TO_APPLICATION_COMMAND,1) != [0x00]:
                    return False
                else:
                    return True
            except:
                return False
        else:
            return False

    def ping_device(self) -> bool:
        if self.__i2c_bus_open:
            try:
                if self.__i2c_bus.read_i2c_block_data(self.__i2c_device_address,PING_COMMAND,1) != [0xAA]:
                    return False
                return True
            except:
                return False
        else:
            return False

    @property
    def bus_open(self) -> bool:
        return self.__i2c_bus_open
