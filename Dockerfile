# 사전 빌드된 베이스 이미지 사용
FROM threebody-base:latest

WORKDIR /app

# 소스 코드 복사
COPY . .

# 빌드 실행
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j4

# 실행 권한 설정
RUN chmod +x build/sim build/testfont

# 기본 명령어 - testfont로 폰트 확인 후 sim 실행
CMD ["bash", "-c", "echo '폰트 테스트 실행 중...' && ./build/testfont && echo '시뮬레이션 시작...' && ./build/sim"]