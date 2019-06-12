import os
import sys
import serial
import signal
import threading
import time

state = 0
offset = 0
rec_len = 0
countI = 0
exitThread = False   # 쓰레드 종료용 변수
# 파일 생성
file = open('F:/output/sample.txt', 'wb')

with open('F:/output/sample.jpg', 'rb') as image:
  f = image.read()
  b= bytearray(f)
  file.write("#define IMAGE_LENGTH %d " % len(b))
  file.write("static unsigned char sample_image[IMAGE_LENGTH] = {")
  for item in list(b):
    file.write("%s, " % item)
  file.seek(-2, os.SEEK_CUR)
  file.write("};");
  file.flush()

file = open('F:/output/output.jpg', 'wb')
#file.write(bytearray(150))
#file.flush()
#쓰레드 종료용 시그널 함수
def handler(signum, frame):
     exitThread = True

#데이터 처리할 함수
def readThread(ser):
    global exitThread, file, countI, offset, rec_len, state
    while not exitThread:
      for c in ser.read():
        if (state == 0):
          if(c == 'i'):
            state = 9
            countI += 1
          else:
            countI = 0
        elif(state == 9):
          print("state 9 countI %d" % countI)
          if(c == 'i'):
            countI += 1
          else:
            countI = 0
            state = 0
          if(countI >= 5):
            countI = 0
            state = 1
        elif(state == 1):
          print("state 1")
          offset = ord(c)
          print("offset %d" % offset)
          file.seek(offset * 8, os.SEEK_SET)
          state = 2
        elif(state == 2):
          rec_len = ord(c)
          print("len %d" % rec_len)
          state = 3
        elif(state == 3):
          print("[3] : rec_len %d" % rec_len)
          file.write(c)
          rec_len = rec_len - 1
          if (rec_len == 0):
            print(rec_len == 0)
            state = 0
            file.flush()
        else:
          print("err")
          

#종료 시그널 등록
signal.signal(signal.SIGINT, handler)

#시리얼 열기
ser = serial.Serial('COM17', 115200)

#시리얼 읽을 쓰레드 생성
thread = threading.Thread(target=readThread, args=(ser,))

# 시작
thread.start()
