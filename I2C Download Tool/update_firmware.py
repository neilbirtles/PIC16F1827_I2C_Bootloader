#python port / update PC host code from https://www.microchip.com/wwwAppNotes/AppNotes.aspx?appnote=en546181

import argparse
import device

import_file_path = ""
verbose_output = False
debug_output = False

devices_to_prog_addresses = {0x10, 0x11}

#the offset address where the actual program starts. This is after the bootloader code 
#bootloader_prog_mem_offset = 0x0290
bootloader_prog_mem_offset = 0x0000
prog_mem_start_address = 0x0000
prog_mem_end_address = 0x0FFF
prog_mem_size = 4096 
#2 bytes per word in program memory (14bits) - all addresses in hex file are on bytes so need to divide by
# this to get the prog mem address
bytes_per_word = 2 
eeprom_size = 256
eeprom_start_address = 0xF000
eeprom_end_address = 0xF0FF
user_ids_start_address = 0x8000
user_ids_end_address = 0x8003
device_ids_start_address = 0x8006
device_ids_end_address = 0x8006
config_words_start_address = 0x8007
config_words_end_address = 0x8008

#create a device to store the info read from the hexfile
pic16f1827 = device.Device(prog_mem_size, eeprom_size)

#get the command line arguments
arg_parser = argparse.ArgumentParser(description="Firmware Update Tool for PIC16F1827 chips with I2C bootloader")
arg_parser.add_argument("-H", "--hexfile", required=True, help="Hexfile that contains the updated firmware")
arg_parser.add_argument("-v", "--verbose", action='store_true',help="Provides a verbose output of progress")
# parse and save the command line arguments 
arguments = arg_parser.parse_args()
import_file_path = arguments.hexfile
verbose_output = arguments.verbose

try:
    
    ex_linear_address = 0

    #open the specified hex file for reading
    with open(import_file_path, 'r') as reader:
        print("Opening hex file: " + import_file_path)
        #read each line in the hex file and process the data
        for line in reader:
            # hex file line format
            #:BBAAAATTDDDDCC
            # BB - number of data bytes on line
            # AAAA - address - doubled 
            # TT record type, 00 - program data, 01 - EOF, 04 extended linear address
            # DD data bytes
            # CC checksum
            #if ((fileLine[0] == ':') && (fileLine.Length >= 11))
            if line[0]==":":
                # BB - get the number of bytes on this line, convert from hex to int
                line_byte_count = int(line[1:3],16)
                
                # AAAA - get the address, convert from hex to int
                #all addresses are doubled - allows for two bytes per word, but leave doubling here 
                #as need the byte address to work out if its MS byte or LS byte later 
                line_base_address = ex_linear_address + int(line[3:7],16)

                # TT - get the record type, convert from hex to int
                record_type = int(line[7:9],16)
                
                #got program data on this line - store it
                if record_type == 0:
                    #go thru each byte on the line in turn
                    for byte_number in range(line_byte_count):
                        
                        #address for this byte is built from the lines base address + its position
                        #address still doubled vs prog memory locations as two bytes per location
                        byte_address = line_base_address + byte_number

                        #bytes are stored big endian format, so even numbered bytes are the least 
                        #significant bytes in a two byte word and odd bytes are the most significant
                        if byte_address % bytes_per_word == 0:
                            # DD - starts at position 9 for byte 0
                            #this byte is a least significant byte so get it and make the upper byte
                            #all 1's to allow it to be AND'ed with the most significant byte later
                            memory_word = 0xFF00 | int(line[9+(2*byte_number):11+(2*byte_number)],16)
                            #memory_word = 0xFF{byte}
                        else:
                            # DD - starts at position 9 for byte 0
                            #this byte is a most significant byte
                            memory_word = int(line[9+(2*byte_number):11+(2*byte_number)],16)
                            #shift up by 8 bits to put it into the uppper byte of a two byte word
                            memory_word = memory_word << 8
                            #fill the lower byte with all 1's to allow it to be AND'ed with the
                            #most significant byte later
                            memory_word = memory_word | 0xFF
                            #memory_word = 0x{byte}FF
                        
                        #store in the device 
                        invalid_memory_location = True

                        #program memory locations
                        if (byte_address//bytes_per_word >= prog_mem_start_address) &\
                         (byte_address//bytes_per_word >= bootloader_prog_mem_offset) &\
                         (byte_address//bytes_per_word <= prog_mem_end_address):
                            #first time thru print the reading message, note there are two byte addresses that 
                            # match the word address so only get the even one
                            if (byte_address//bytes_per_word == bootloader_prog_mem_offset) &\
                                (byte_address % bytes_per_word == 0): 
                                print("Reading program memory...")
                            if debug_output:
                                print("reading program memory: " + hex(byte_address//bytes_per_word))
                            #AND the current memory word we have got with the existing word - blank is 0x3FFF
                            #if this is a least significant byte then upper byte is 0xFF and if its most significant byte
                            # then lower byte is 0xFF so the existing upper or lower is left intact by the AND
                            #address in the prog memory array is offset by the program memory start address to make the 
                            # array index start at 0 and is divided by the number of bytes per word (2) as we are now using
                            # word addresses not byte addresses and need the least and most significant bytes to be 
                            # combined into one word
                            pic16f1827.program_memory[int((byte_address-prog_mem_start_address)/bytes_per_word)] &= memory_word
                        #user IDs
                        elif (byte_address//bytes_per_word >= user_ids_start_address) &\
                         (byte_address//bytes_per_word <= user_ids_end_address):
                            #NOT IMPLEMENTED
                            if debug_output:
                                print("reading user ids: " + hex(byte_address//bytes_per_word))
                        #device IDs
                        elif (byte_address//bytes_per_word >= device_ids_start_address) &\
                         (byte_address//bytes_per_word <= device_ids_end_address):
                            #NOT IMPLEMENTED
                            if debug_output:
                                print("reading device ids: " + hex(byte_address//bytes_per_word))
                        #config words
                        elif (byte_address//bytes_per_word >= config_words_start_address) &\
                         (byte_address//bytes_per_word <= config_words_end_address):
                            #NOT IMPLEMENTED
                            if debug_output:
                                print("reading config words: " + hex(byte_address//bytes_per_word))
                        #eeprom locations
                        elif (byte_address/bytes_per_word >= eeprom_start_address) &\
                         (byte_address/bytes_per_word <= eeprom_end_address):
                            #NOT IMPLEMENTED
                            if debug_output:
                                print("reading eeprom: " + hex(byte_address//bytes_per_word))
                        else:
                            print("Error, hex file not compatible with bootloader. Current address: " + hex(int(byte_address / 2)))
                            quit() 
                        
                #got an extended linear address - store it as the upper 16 bits
                #for combining with the lower 16 bits of the address in the 00 record
                #if there has been no 04 record in the file before this line then 
                #the upper 16 bits are 0
                #note: extended addresses are doubled as well
                elif record_type == 4:
                    ex_linear_address = (int(line[9:13],16)<<16)

                #end of file record 
                elif record_type == 1:
                    break
        #end for line in reader
    
except FileNotFoundError as fnf_error:
    print ("Requested hexfile \"" + import_file_path + "\" not found, please specify a valid file")

print("Hex file successfully loaded")

for device in devices_to_prog_addresses:
    pass