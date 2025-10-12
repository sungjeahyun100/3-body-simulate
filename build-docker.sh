#!/bin/bash

echo "=== 3-Body Simulate Docker 빌드 ==="

# 베이스 이미지가 있는지 확인
if ! docker image ls | grep -q "threebody-base"; then
    echo "베이스 이미지를 생성합니다..."
    docker compose --profile base up base
    echo "베이스 이미지 생성 완료!"
else
    echo "베이스 이미지가 이미 존재합니다."
fi

# 애플리케이션 빌드
echo "애플리케이션을 빌드합니다..."
docker compose build sim

echo "빌드 완료! 다음 명령어로 실행하세요:"
echo "  docker compose up sim"
echo ""
echo "개발 모드로 실행하려면:"
echo "  docker compose --profile dev up dev"