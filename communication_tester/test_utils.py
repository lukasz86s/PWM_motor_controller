from utils import calculate_crc16_MODBUS, create_frame

def test_crc():
    test_data = [i for i in range(1, 10)]
    assert calculate_crc16_MODBUS(bytes(test_data)) == 0xB20E

SAMPLE_FRAME =[0x55, 0x09, 0x03, 0x02, 0x02, 0x11, 0x01, 0x4F, 0x2A, 0xEA]
#data needed to create the frame
FUNCTION_OFFSET = 2
CHANNELS_DATA_SIZE_OFFSET = 3
CHANNELS_DATA_OFFSET = 4
CHANNELS_DATA_SIZE_MULT = 2
CHANNELS_DATA_SLICE = SAMPLE_FRAME[CHANNELS_DATA_OFFSET : CHANNELS_DATA_OFFSET + SAMPLE_FRAME[CHANNELS_DATA_SIZE_OFFSET]*CHANNELS_DATA_SIZE_MULT]
FUNCTION = SAMPLE_FRAME[FUNCTION_OFFSET]

def test_size_create_frame():
    frame = create_frame(FUNCTION, CHANNELS_DATA_SLICE)
    assert len(frame) == len(SAMPLE_FRAME)

def test_coorect_create_frame():
    frame = create_frame(FUNCTION, CHANNELS_DATA_SLICE)
    for f, d in zip(frame, SAMPLE_FRAME):
        assert f == d