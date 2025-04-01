import serial

def calculate_crc16_MODBUS(data: bytes) -> int:
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x0001:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc

FRAME_CONST_SIZE = 3
FRAME_CRC_SIZE = 2
def create_frame(cmd: int,  channels_data: list):
    channels_data_size = len(channels_data)
    assert channels_data_size % 2 == 0
    channels_to_set_size = channels_data_size//2
    data = []
    data.append(0x55)
    frame_size = channels_data_size + FRAME_CONST_SIZE + FRAME_CRC_SIZE
    data.append(frame_size)
    data.append(cmd)
    data.append(channels_to_set_size)
    [data.append(d) for d in channels_data]
    crc = calculate_crc16_MODBUS(bytes(data[1:]))
    high_byte = (crc >> 8) & 0xFF
    low_byte = crc & 0xFF
    data.append(high_byte)
    data.append(low_byte) 
    print(data)
    print(crc)
    return data   