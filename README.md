## 다중 매체 통신

	3종류의 무선 통신 매체 (LiFi, Bluetooth, ZigBee) 통합을 진행하는 임베디드 소프트웨어 프로젝트. 
	통합을 통해 각 무선 통신 매체가 열악한 통신환경으로 통신이 단절되는 경우, 매체를 변경하여 지속적인 통신이 가능토록 함. 
	
* 사용 통신 모듈
<center> <img src="https://i.ibb.co/8B6r2JC/1.png" width="70%" height="70%"> </center>

* Li-Fi -> Bluetooth 매체 변경
<center><img src="https://media.giphy.com/media/XFoi5aaJAWdTUYVE1h/giphy.gif" width="80%" height="70%"> </center>

* 단편화
<center><img src="https://media.giphy.com/media/RgywT7V7V9JxZViIpU/giphy.gif" width="80%" height="70%"> </center>


## 프로젝트 구조
<pre><code>
Root
  ├── Driver
  │  ├── BLUETOOTH : HM-10 제어 관련
  │  ├── CC2530 : CC2530 제어 관련
  │  ├── LIFI : LIFI 제어 관련
  │  ├── INTEG_MAC : 통합 계층 제어 관련
  │  ├── CMSIS : STM32 드라이버
  │  └── STM32_HAL_Driver : STM32 HAL 관련
  ├── EWARM : IAR 프로젝트 파일
  ├── Inc : STM32 제어 관련 헤더
  └── Src : STM32 제어 관련

</code></pre>


## 하드웨어 인터페이스

#### GPIO
	PC9   CC2530 RESET PIN
	PC10  BLUETOOTH STATE PIN

#### UART
	            UART     TX    RX  
	BLUETOOTH   USART2   PD5   PD6  
	PC          USART3   PD8   PD9  
	CC2530      UART4    PC10  PC11  
	LI-FI       UART5    PC12  PD2  
