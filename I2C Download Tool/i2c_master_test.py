from i2c_master import I2C_Bootloader_Interface

address_to_set = 0x0300
data_to_write = [0x010A, 0x020B, 0x030C, 0x040D, 0x050E, 0x060F, 0x0700, 0x0801]
retries = 0

device_interface = I2C_Bootloader_Interface(3,0x11)

print("Opening bus...",end="")
if device_interface.open_bus() & device_interface.bus_open:
    print("[OK]")
else:
    print("[Error]")
    quit()



print("Pinging device...", end="")
if device_interface.ping_device():
    print("[OK]")
else:
    print("[Error]")

print("Setting address pointer...", end="")
if device_interface.set_address_pointer(address_to_set, retries):
    print("[OK]")
else:
    print("[Error]")

print("Getting address pointer...", end="")
address = device_interface.get_address_pointer()
if address == address_to_set:
    print("[OK]")
elif address == -1:
    print("[Error getting address]")
else:
    print("[Address received does not match address set (" +hex(address)+")]")

print("Erasing row...", end="")
if device_interface.erase_row():
    print("[OK]")
else:
    print("[Error]")

print("Setting address pointer before write...", end="")
if device_interface.set_address_pointer(address_to_set, retries):
    print("[OK]")
else:
    print("[Error]")

print("Sending data to on chip buffer...", end="")
if device_interface.send_data_to_buffer(data_to_write):
    print("[OK]")
else:
    print("[Error]")

print("Writing data to program memory...", end="")
if device_interface.write_buffer():
    print("[OK]")
else:
    print("[Error]")

print("Setting address pointer before read...", end="")
if device_interface.set_address_pointer(address_to_set, retries):
    print("[OK]")
else:
    print("[Error]")

print("Reading program memory...", end="")
data_read = device_interface.read_data()
if data_read == data_to_write:
    print("[OK]")
elif data_read == [-1]:
    print("[Error getting address]")
else:
    print("[Data received does not match data sent: " + str(data_to_write) + ":" +str(data_read))

print("Setting address pointer before verify...", end="")
if device_interface.set_address_pointer(address_to_set, retries):
    print("[OK]")
else:
    print("[Error]")

print("Testing verify...", end="")
if device_interface.verify_data(data_to_write):
    print("[OK]")
else:
    print("[Error]")

print("Testing app mode switch...", end="")
if device_interface.goto_app_mode():
    print("[OK]")
else:
    print("[Error]")





