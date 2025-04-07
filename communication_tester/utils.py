import serial

def calculate_crc16_MODBUS(data: bytes) -> int:
    """calculates the checksum crc16 MODBUS"""
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
PING_CMD = 5

def crc16_split_into_two_bytes(crc: int):
    high_byte = (crc >> 8) & 0xFF
    low_byte = crc & 0xFF
    return high_byte, low_byte

#unify the creation of the ping frame and the rest of the functions
def create_frame(cmd: int,  channels_data: list):
    """creates a frame to send from the given dancyh """
    if cmd == PING_CMD:
        data = [0x55, 0x1, PING_CMD]
        crc = calculate_crc16_MODBUS(bytes(data[1:]))
        high_byte, low_byte = crc16_split_into_two_bytes(crc)
        data.append(high_byte)
        data.append(low_byte)
        return data 

    channels_data_size = len(channels_data)
    assert channels_data_size > 0

    #calculate channels data len in bytes
    channels_to_set_size = channels_data_size
    if(channels_data_size != 1):
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
    high_byte, low_byte = crc16_split_into_two_bytes(crc)
    data.append(high_byte)
    data.append(low_byte) 

    return data   