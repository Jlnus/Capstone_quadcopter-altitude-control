% 참고 https://kr.mathworks.com/help/matlab/import_export/read-streaming-data-from-arduino.html

clc
close all
clf
s = serialport('COM9', 115200); % 객체 생성(포트 번호, 통신속도 설정)
configureTerminator(s,"CR/LF"); % 종결자 설정
flush(s); % 이전 데이터 초기화

% 유저 데이터 구조체 생성(data1~3과 count(시간))
s.UserData = struct("Data1",[],"Data2",[],"Count",1);

% 콜백 함수 지정(아래 선언한 함수)
configureCallback(s,"terminator",@readAngle);

function readAngle(src, ~)
data0 = readline(src); % 한줄 씩 읽음
data1 = str2double(split(data0,"/"));

% DATA에 하나씩 넣음
src.UserData.Data1(end+1) = data1(1); 
src.UserData.Data2(end+1) = data1(2); 
 
src.UserData.Count = src.UserData.Count + 1; % count 1씩 증가
if (src.UserData.Count>100)    % 50 이상이면 x축 범위 갱신
    xlim([src.UserData.Count-100 src.UserData.Count])
end

plot(src.UserData.Data1,'r' );
hold on
plot(src.UserData.Data2,'b' );
grid on
xlabel("cnt")
ylabel("Altitude (m)")

% drawnow; % 실시간 업데이트
end
