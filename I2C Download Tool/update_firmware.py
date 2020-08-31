import argparse

import_file_path = ""
verbose_output = False



#get the command line arguments
arg_parser = argparse.ArgumentParser(description="Firmware Update Tool for PIC16F1827 chips with I2C bootloader")
arg_parser.add_argument("-H", "--hexfile", required=True, help="Hexfile that contains the updated firmware")
arg_parser.add_argument("-v", "--verbose", action='store_true',help="Provides a verbose output of progress")
# parse and save the command line arguments 
arguments = arg_parser.parse_args()
import_file_path = arguments.hexfile
verbose_output = arguments.verbose

try:
    #open the specified hex file for reading
    with open(import_file_path, 'r') as reader:
        #read each line in the hex file and process the data
        for line in reader:
            # hex file line format
            #:BBaaAATTDDDDCC
            # BB - number of data bytes on line
            # aaAA - address, lower byte aa, upper byte AA - doubled 
            # TT record type, 00 - program data, 01 - EOF, 04 extended linear address
            # DD data bytes
            # CC checksum
            #if ((fileLine[0] == ':') && (fileLine.Length >= 11))
            if line[0]==":":
                byte_count=line[1:3]
                address=line[3:7]
                record_type=line[7:9]
                print(line, end='')
                print(byte_count)
                print(address)
                print(record_type)

except FileNotFoundError as fnf_error:
    print ("Requested hexfile \"" + import_file_path + "\" not found, please specify a valid file")
