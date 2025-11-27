# HTS ver6

Qt6 기반 크로스 플랫폼 HTS 애플리케이션

## 목차
- [개발 환경 설정](#개발-환경-설정)
- [빌드 방법](#빌드-방법)
- [배포 방법](#배포-방법)
- [문제 해결](#문제-해결)

---

## 개발 환경 설정

### 공통 요구사항
- CMake 3.16 이상
- Qt 6.7.3 이상
- C++17 지원 컴파일러

### Windows

#### 1. Qt 설치
1. [Qt 공식 사이트](https://www.qt.io/download)에서 Qt Online Installer 다운로드
2. Qt 6.7.3 (MSVC 2022) 설치
3. Qt Creator 설치 (선택사항, 권장)

#### 2. 환경 변수 설정
```powershell
# Qt 경로를 PATH에 추가 (예시)
$env:PATH += ";C:\Qt\6.7.3\msvc2022_64\bin"
$env:Qt6_DIR = "C:\Qt\6.7.3\msvc2022_64"
```

영구 설정:
- 시스템 환경 변수에 `Qt6_DIR` 추가: `C:\Qt\6.7.3\msvc2022_64`
- PATH에 `%Qt6_DIR%\bin` 추가

#### 3. CMake & 컴파일러
- Visual Studio 2019/2022 설치 (MSVC 사용 시)
- 또는 [CMake 공식 사이트](https://cmake.org/download/)에서 설치

### Linux (Ubuntu/Debian)

#### 1. 시스템 패키지 설치
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libxcb-xinerama0 \
    libxcb-cursor0
```

#### 2. Qt 설치

**방법 A: Qt Online Installer (권장)**
```bash
# Qt 공식 설치 프로그램 다운로드
wget https://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run
chmod +x qt-unified-linux-x64-online.run
./qt-unified-linux-x64-online.run

# Qt 6.7.3 선택하여 설치 (예: ~/Qt/6.7.3/gcc_64)
```

**방법 B: 시스템 패키지 (간단하지만 버전이 오래될 수 있음)**
```bash
sudo apt-get install -y qt6-base-dev qt6-tools-dev qt6-tools-dev-tools
```

#### 3. 환경 변수 설정
```bash
# ~/.bashrc 또는 ~/.zshrc에 추가
export Qt6_DIR="$HOME/Qt/6.7.3/gcc_64"
export PATH="$Qt6_DIR/bin:$PATH"
export LD_LIBRARY_PATH="$Qt6_DIR/lib:$LD_LIBRARY_PATH"

# 적용
source ~/.bashrc
```

---

## 빌드 방법

### Windows

#### Qt Creator 사용 (권장)
1. Qt Creator 실행
2. `File > Open File or Project` → `CMakeLists.txt` 선택
3. Kit 선택 (Desktop Qt 6.7.3 MSVC2022 64bit)
4. Configure 완료 후 Build 버튼 클릭

#### 명령줄 (PowerShell)
```powershell
# 프로젝트 클론
git clone <repository-url>
cd HTS_ver6

# Debug 빌드
cmake --preset windows-debug
cmake --build build/windows-debug

# Release 빌드
cmake --preset windows-release
cmake --build build/windows-release --config Release

# 실행
.\build\windows-release\Release\HTS_ver6.exe
```

### Linux

#### Qt Creator 사용
1. Qt Creator 실행: `qtcreator`
2. `File > Open File or Project` → `CMakeLists.txt` 선택
3. Kit 선택 (Desktop Qt 6.7.3 GCC 64bit)
4. Configure 완료 후 Build 버튼 클릭

#### 명령줄
```bash
# 프로젝트 클론
git clone <repository-url>
cd HTS_ver6

# Debug 빌드
cmake --preset linux-debug
cmake --build build/linux-debug

# Release 빌드
cmake --preset linux-release
cmake --build build/linux-release

# 실행
./build/linux-release/HTS_ver6
```

---

## 배포 방법

실행 파일만으로는 Qt 라이브러리가 없어 실행되지 않습니다. Qt 의존성을 함께 배포해야 합니다.

### Windows 배포

#### 1. Release 빌드
```powershell
cmake --preset windows-release
cmake --build build/windows-release --config Release
```

#### 2. 배포 패키지 생성
```powershell
# deploy 폴더 생성
mkdir deploy
Copy-Item build\windows-release\Release\HTS_ver6.exe deploy\

# Qt 의존성 자동 수집
cd deploy
& "C:\Qt\6.7.3\msvc2022_64\bin\windeployqt.exe" HTS_ver6.exe

# 결과: deploy 폴더에 실행 가능한 독립 패키지 생성
```

#### 3. 배포
- `deploy` 폴더 전체를 ZIP으로 압축하여 배포
- 사용자는 압축 해제 후 `HTS_ver6.exe` 실행

### Linux 배포

#### 1. Release 빌드
```bash
cmake --preset linux-release
cmake --build build/linux-release
```

#### 2. 배포 패키지 생성

**방법 A: linuxdeployqt 사용 (권장)**
```bash
# linuxdeployqt 다운로드
wget https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage
chmod +x linuxdeployqt-continuous-x86_64.AppImage

# deploy 폴더 준비
mkdir -p deploy/usr/bin
cp build/linux-release/HTS_ver6 deploy/usr/bin/

# Qt 의존성 수집
./linuxdeployqt-continuous-x86_64.AppImage deploy/usr/bin/HTS_ver6 -bundle-non-qt-libs

# 실행 스크립트 생성
cat > deploy/run.sh << 'EOF'
#!/bin/bash
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export LD_LIBRARY_PATH="$SCRIPT_DIR/usr/lib:$LD_LIBRARY_PATH"
"$SCRIPT_DIR/usr/bin/HTS_ver6" "$@"
EOF
chmod +x deploy/run.sh
```

**방법 B: AppImage 생성 (단일 파일 배포)**
```bash
# 위의 방법 A 실행 후
./linuxdeployqt-continuous-x86_64.AppImage deploy/usr/bin/HTS_ver6 -appimage

# 결과: HTS_ver6-x86_64.AppImage 생성 (단일 실행 파일)
```

**방법 C: 수동 배포 (간단)**
```bash
# 배포 스크립트 사용 (아래 deploy-linux.sh 참조)
chmod +x deploy-linux.sh
./deploy-linux.sh
```

#### 3. 배포
- **AppImage**: 단일 파일 배포, 실행 권한 부여 후 실행
- **폴더**: `deploy` 폴더 압축 후 배포, `run.sh` 스크립트로 실행

---

## 배포 스크립트

프로젝트 루트에 배포 자동화 스크립트가 포함되어 있습니다.

### Windows: `deploy-windows.ps1`
```powershell
.\deploy-windows.ps1
```
결과: `deploy-windows` 폴더에 배포 패키지 생성

### Linux: `deploy-linux.sh`
```bash
chmod +x deploy-linux.sh
./deploy-linux.sh
```
결과: `deploy-linux` 폴더에 배포 패키지 생성

---

## 문제 해결

### Windows

#### "Qt6Widgets.dll을 찾을 수 없습니다"
- `windeployqt.exe` 실행 확인
- Qt bin 폴더가 PATH에 있는지 확인

#### "VCRUNTIME140.dll을 찾을 수 없습니다"
- Visual C++ Redistributable 설치:
  https://aka.ms/vs/17/release/vc_redist.x64.exe

### Linux

#### "error while loading shared libraries: libQt6Widgets.so.6"
```bash
# LD_LIBRARY_PATH 설정 확인
export LD_LIBRARY_PATH="$HOME/Qt/6.7.3/gcc_64/lib:$LD_LIBRARY_PATH"

# 또는 run.sh 스크립트 사용
```

#### "Could NOT find OpenGL"
```bash
sudo apt-get install -y libgl1-mesa-dev libglu1-mesa-dev
```

#### Qt Creator에서 소스 파일이 보이지 않음
1. `Build > Clear CMake Configuration`
2. `File > Close All Projects and Editors`
3. `CMakeLists.txt` 다시 열기

### 공통

#### CMake configure 실패
```bash
# build 폴더 완전 삭제 후 재시도
rm -rf build/*
cmake --preset <platform>-debug
```

#### Qt 버전 충돌
```bash
# Qt6_DIR 환경변수 명시적 설정
# Windows
set Qt6_DIR=C:\Qt\6.7.3\msvc2022_64

# Linux
export Qt6_DIR=$HOME/Qt/6.7.3/gcc_64
```

---

## 프로젝트 구조

```
HTS_ver6/
├── CMakeLists.txt          # CMake 빌드 설정
├── CMakePresets.json       # 플랫폼별 빌드 프리셋
├── README.md               # 이 파일
├── deploy-windows.ps1      # Windows 배포 스크립트
├── deploy-linux.sh         # Linux 배포 스크립트
├── src/                    # 소스 코드
│   ├── main.cpp
│   ├── mainwindow.h
│   └── mainwindow.cpp
└── resources/              # 리소스 파일
    ├── mainwindow.ui
    └── lan/                # 번역 파일
        ├── HTS_ver6_ko_KR.ts
        └── HTS_ver6_en.ts
```

---

## 라이센스

[라이센스 정보 추가]

## 기여

[기여 가이드 추가]
