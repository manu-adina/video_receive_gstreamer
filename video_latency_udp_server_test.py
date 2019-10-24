import socket
import pickle
import cv2 as cv
import time
from fps import FPS
import struct

HOST = "192.168.0.124"
PORT = 5005

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((HOST, PORT))

data = b""
payload_size = struct.calcsize(">L")
payload_time = struct.calcsize(">d")

print("Socket set up")
fps = None

while True:
    fps = FPS().start()

    # Receive time info.
    packed_msg_time = sock.recv(8)
    if len(packed_msg_time) !=  8:
        continue
    msg_time = struct.unpack(">d", packed_msg_time)[0]
    print("Received Time: {}".format(msg_time))

    packed_msg_size = sock.recv(4)
    msg_size = struct.unpack(">L", packed_msg_size)[0]
    if msg_size > 100000:
        continue

    print("Received Size: {}".format(msg_size))

    # Receive frame data
    frame_data = sock.recv(msg_size)
    frame = pickle.loads(frame_data)
    frame = cv.imdecode(frame, cv.IMREAD_COLOR)

    # Time in ms
    time_elapsed = time.time() - msg_time
    print("TX Time: {}".format(time_elapsed))

    fps.update()
    fps.stop()

    # Display in window FPS and TX Time
    info = [("TX Time", "{:.2f}".format(time_elapsed)),
            ("FPS: ", "{:.2f}".format(fps.fps()))]

    for (i, (k, v)) in enumerate (info):
        text = "{} : {}".format(k, v)
        cv.putText(frame, text, (10, 480 - ((i * 20) + 20)), cv.FONT_HERSHEY_SIMPLEX, 0.6, (0, 0, 255), 2)

    cv.imshow("frame", frame)

    key = cv.waitKey(1) & 0xFF
    if key == ord("q"):
        sock.close()
        break

cv.destroyAllWindows()
