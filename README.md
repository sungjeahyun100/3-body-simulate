# 3-BODY-SIMULATE

3체 문제 시뮬레이션을 OpenGL/GLFW로 구현한 프로젝트입니다.

## 🚀 Docker를 사용한 실행 (추천)

Docker를 사용하면 의존성 설치 없이 바로 실행할 수 있습니다.

### 빠른 시작

```bash
# 1. 처음 빌드 (베이스 이미지 + 애플리케이션)
./build-docker.sh

# 2. 시뮬레이션 실행
./run-docker.sh
```

### 상세 사용법

```bash
# 베이스 이미지 빌드 (최초 1회만 - 시간이 오래 걸림)
docker compose --profile base up base

# 애플리케이션 빌드 (빠름)
docker compose build sim

# 시뮬레이션 실행
docker compose up sim

# 개발 모드 (소스코드 실시간 반영)
docker compose --profile dev up dev

# 새 터미널에서 실행
docker exec -it threebody-simulation-dev-1 bash
```

## 🛠️ 로컬 빌드 방법

Docker 없이 직접 빌드하려면:

```bash
# 필수 패키지들 설치
sudo apt update
sudo apt install build-essential cmake

# OpenGL 개발 라이브러리
sudo apt install libgl-dev libglu1-mesa-dev

# GLFW3 설치
sudo apt install libglfw3-dev

# GLEW 설치 (OpenGL Extension Wrangler)
sudo apt install libglew-dev

# pkg-config (CMake에서 라이브러리 찾기용)
sudo apt install pkg-config

# 추가로 필요할 수 있는 패키지들
sudo apt install libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev

# 프로젝트 폴더 (README.md가 있는 루트)로 이동 후
mkdir build
cd build
cmake ..
make -j4

# 폰트 테스트 후 시뮬레이션 실행
./testfont  # 폰트가 깨진다면 rp로 연락 바람
./sim       # 시뮬레이션 실행
```

## 📁 프로젝트 구조

```
├── src/                    # 소스 코드
│   ├── physics.cpp/h       # 물리 엔진
│   ├── ui.cpp/h           # 사용자 인터페이스
│   └── font.cpp/h         # 폰트 렌더링
├── simul.cpp              # 메인 시뮬레이션
├── testfont.cpp           # 폰트 테스트
├── CMakeLists.txt         # CMake 설정
├── Dockerfile.base        # 베이스 Docker 이미지
├── Dockerfile             # 애플리케이션 Docker 이미지
├── compose.yaml           # Docker Compose 설정
├── build-docker.sh        # Docker 빌드 스크립트
└── run-docker.sh          # Docker 실행 스크립트
```

## 🎮 사용법

시뮬레이션이 시작되면 3개의 천체가 중력 상호작용을 하며 움직이는 모습을 볼 수 있습니다.

## ⚠️ 문제 해결

- **폰트가 깨지는 경우**: `./testfont`로 확인 후 rp에게 연락
- **Docker GUI 문제**: X11 포워딩이 제대로 설정되었는지 확인
- **빌드 에러**: 필요한 패키지들이 모두 설치되었는지 확인

