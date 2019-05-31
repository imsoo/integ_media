import os
import sys
import serial
import signal
import threading
import time

countI = 0
exitThread = False   # 쓰레드 종료용 변수
# 파일 생성
file = open('F:/output/sample.txt', 'wb')

with open('F:/output/sample.jpg', 'rb') as image:
  f = image.read()
  b= bytearray(f)
  file.write("#define IMAGE_LENGTH %d " % len(b))
  file.write("unsigned char sample_image[IMAGE_LENGTH] = {")
  for item in list(b):
    file.write("%s, " % item)
  file.seek(-2, os.SEEK_CUR)
  file.write("};");
  file.flush()

file = open('F:/output/output.jpg', 'wb')
#쓰레드 종료용 시그널 함수
def handler(signum, frame):
     exitThread = True

#데이터 처리할 함수
def readThread(ser):
    global exitThread, file, countI
    while not exitThread:
       #데이터가 있있다면
       for c in ser.read():
          if(countI >= 5):
             print(c)
             file.write(c)
             file.flush()
          if(c == 'i'):
             countI += 1;
          

#종료 시그널 등록
signal.signal(signal.SIGINT, handler)

#시리얼 열기
ser = serial.Serial('COM17', 115200)

#시리얼 읽을 쓰레드 생성
thread = threading.Thread(target=readThread, args=(ser,))

# 시작
thread.start()
