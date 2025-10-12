#!/bin/bash

# X11 포워딩 허용
echo "X11 포워딩을 허용합니다..."
xhost +local:docker

echo "=== 3-Body 시뮬레이션 실행 ==="
echo "폰트 테스트 후 시뮬레이션이 시작됩니다."
echo "폰트가 깨진다면 rp로 연락하세요."

# 시뮬레이션 실행
docker compose up sim

# 정리
xhost -local:docker