import ctypes
import os
import time
import struct
import AFPhotonSS_pb2 as photon


class Message(ctypes.Structure):
    	_fields_ = [("protocol", ctypes.c_int), ("data", ctypes.c_char_p)]

# load dynamic lib
my_dll = ctypes.cdll.LoadLibrary('/Users/qinhan/c++/ARK/build/bin/KcpTool_d.dylib')
 
start_client_func = my_dll.StartClient
send_message_func = my_dll.SendMsg
get_msg_func = my_dll.GetMessage

get_msg_func.restype = Message

for num in range(0, 100):
    # connect to udp server 
    print("------------------------------------------------------------")
    client_id = start_client_func(b'47.101.184.248', 9011, 10)
    if (client_id != -1):
        print("connect, client id = ", client_id)
    else:
        print("connect fail")
        continue

    # req_enter_server message
    message_bytes = photon.req_enter_server(
        open_id = '11123',
        name='11',
        level = 21,
    ).SerializeToString()
    send_message_func(client_id, photon.E_CS_MSG_ID_REQ_ENTER_SERVER, len(message_bytes), message_bytes)
    msg = get_msg_func(client_id, 50)
    if (msg.protocol!=0):
        ack = photon.ack_enter_server()
        ack.ParseFromString(msg.data)
        print("protocol = " ,msg.protocol)
        print("data = ", msg.data)
        print("after parse : ", ack)
    else:
        print("protocol != 0")
        os._exit() 

    # 发送getroomlist请求
    message_bytes = photon.req_room_list().SerializeToString()
    send_message_func(client_id, photon.E_CS_MSG_ID_REQ_ROOM_LIST, len(message_bytes), message_bytes)
    msg = get_msg_func(client_id, 50)
    if (msg.protocol!=0):
        ack = photon.ack_room_list()
        ack.ParseFromString(msg.data)
        print("protocol = " ,msg.protocol)
        print("data = ", msg.data)
        print("after parse : ", ack)
    else:
        print("protocol != 0")
        os._exit()
    
    # 请求服务器
    message_bytes = photon.req_game_lowest().SerializeToString()
    send_message_func(client_id,  photon.E_CS_MSG_ID_REQ_GAME_LOWEST, len(message_bytes), message_bytes)
    msg = get_msg_func(client_id, 5000)
    if (msg.protocol!=0):
        ack = photon.ack_a_game()
        ack.ParseFromString(msg.data)
        print("protocol = " ,msg.protocol)
        print("data = ", msg.data)
        print("after parse : ", ack)
    else:
        print("protocol != 0")
        os._exit() 
