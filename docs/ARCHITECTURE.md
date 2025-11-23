# HTS ver6 Architecture

## 금융 시스템식 구조 설명

### 핵심 원칙
1. **Exception 없음**: Result<T> 패턴으로 명확한 에러 처리
2. **Command/Query 분리**: 상태 변경과 조회를 명확히 분리
3. **순차적 초기화**: 각 단계별 실패 시 롤백 가능
4. **의존성 주입**: 테스트 가능한 구조

---

## 디렉터리 구조

```
src/
├── core/                    # 핵심 타입 및 Application
│   ├── Result.h             # Result<T>, ErrorCode
│   └── Application.h/cpp    # 초기화 시퀀스 관리
│
├── domain/                  # 순수 비즈니스 모델 (Qt 의존성 없음)
│   ├── Session.h
│   ├── AccountSnapshot.h
│   └── MarketTick.h
│
├── infrastructure/          # 외부 시스템 연동
│   ├── Config.h/cpp         # 설정 로드
│   ├── NetworkExecutor.h/cpp # 비동기 네트워크 쓰레드 풀
│   └── SessionManager.h/cpp  # 세션 관리 (싱글톤)
│
├── application/             # Command/Query 서비스 (유즈케이스)
│   ├── IAuthCommandService.h
│   ├── AuthCommandService.h/cpp        # 로그인/로그아웃
│   ├── IAccountQueryService.h
│   └── AccountQueryService.h/cpp       # 계좌 조회
│
├── viewmodel/               # UI 로직 (MVVM)
│   └── LoginViewModel.h/cpp
│
└── ui/                      # Qt Widgets
    └── LoginWindow.h/cpp
```

---

## 초기화 시퀀스 (main.cpp)

```cpp
int main(int argc, char *argv[]) {
    Application app(argc, argv);

    // 1. 초기화
    auto initResult = app.initialize();
    if (initResult.isError()) {
        // 에러 처리 (메시지 박스 + 에러 코드 리턴)
        return static_cast<int>(initResult.error().code);
    }

    // 2. 실행
    int exitCode = app.run();

    // 3. 정리
    app.shutdown();

    return exitCode;
}
```

### Application::initialize() 내부 순서

```
1. loadConfig()
   ├─ 성공 → 다음 단계
   └─ 실패 → 에러 리턴

2. initializeInfrastructure()
   ├─ SessionManager 준비
   ├─ 성공 → 다음 단계
   └─ 실패 → 롤백 + 에러 리턴

3. initializeMemoryPools()
   ├─ (향후 구현: 메모리 풀)
   ├─ 성공 → 다음 단계
   └─ 실패 → 롤백 + 에러 리턴

4. initializeNetwork()
   ├─ NetworkExecutor 초기화 (쓰레드 풀)
   ├─ (향후: GrpcChannelPool 초기화)
   ├─ 성공 → 다음 단계
   └─ 실패 → 롤백 + 에러 리턴

5. initializeViews()
   ├─ LoginViewModel 생성 (의존성 주입)
   ├─ LoginWindow 생성
   ├─ 시그널 연결
   ├─ 성공 → 초기화 완료
   └─ 실패 → 롤백 + 에러 리턴
```

---

## Command/Query 분리

### Command (상태 변경)
- `IAuthCommandService::login()` → 세션 생성
- `IAuthCommandService::logout()` → 세션 삭제
- 특징: 실패 가능, 멱등성 고려, 비동기 실행

### Query (상태 조회)
- `IAccountQueryService::getAccountSnapshot()` → 계좌 정보
- 특징: 캐싱 가능, 배치 처리 가능, 부작용 없음

---

## 의존성 흐름

```
main()
 └─> Application
      ├─> Config (싱글톤)
      ├─> NetworkExecutor (싱글톤)
      ├─> SessionManager (싱글톤)
      └─> LoginWindow
           └─> LoginViewModel
                └─> AuthCommandService (인터페이스 주입)
```

---

## 에러 처리 패턴

### Exception 대신 Result<T> 사용

```cpp
Result<Session> login(const QString& userId, const QString& password) {
    if (userId.isEmpty()) {
        return Result<Session>::failure(
            ErrorCode::InvalidInput,
            "User ID cannot be empty"
        );
    }

    // ... gRPC 호출

    return Result<Session>::success(std::move(session));
}
```

### 사용처에서:

```cpp
auto result = authService->login(id, pw);
if (result.isError()) {
    qDebug() << "Login failed:" << result.error().message;
    return;
}

Session session = result.value();
```

---

## 비동기 처리

### NetworkExecutor로 UI 블로킹 방지

```cpp
NetworkExecutor::instance().execute([this, userId, password]() {
    // 백그라운드 쓰레드에서 실행
    auto result = authService_->login(userId, password);

    // UI 쓰레드로 결과 전달
    QMetaObject::invokeMethod(this, [this, result]() {
        // UI 업데이트
    }, Qt::QueuedConnection);
});
```

---

## 향후 추가할 부분

### 1. gRPC 통합
```cpp
class GrpcChannelPool {
    std::shared_ptr<grpc::Channel> getAuthChannel();
    std::shared_ptr<grpc::Channel> getTradingChannel();
};
```

### 2. 스트리밍 서버 연결
```cpp
class StreamClient : public QObject {
    void connectAndAuth(const Session& session);
    void subscribe(const QString& symbol);

signals:
    void tickReceived(const QString& symbol, const MarketTick& tick);
};
```

### 3. MainWindow & 실시간 데이터
```cpp
// 로그인 성공 후
loginWindow_->loginSucceeded() -> [this]() {
    // 1. StreamClient 연결
    streamClient_->connectAndAuth(session);

    // 2. MainWindow 생성
    mainWindow_ = createMainWindow();
    mainWindow_->show();

    // 3. LoginWindow 숨김
    loginWindow_->hide();
}
```

---

## 빌드 방법

```bash
# CMake configure
cmake -B build/linux-debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=/home/suehyun/Qt/6.7.3/gcc_64

# 빌드
cmake --build build/linux-debug

# 실행
./build/linux-debug/HTS_ver6
```

---

## 테스트 가능성

모든 서비스가 인터페이스로 주입되므로 Mock으로 대체 가능:

```cpp
class MockAuthService : public IAuthCommandService {
    Result<Session> login(const QString&, const QString&) override {
        return Result<Session>::success(mockSession);
    }
};

// 테스트에서
auto mockAuth = std::make_unique<MockAuthService>();
LoginViewModel viewModel(std::move(mockAuth));
```

---

## 정리

이 구조의 핵심은:
1. **계층 분리**: Domain / Application / ViewModel / View
2. **명확한 책임**: Command는 변경, Query는 조회
3. **에러 처리**: Exception 없이 Result<T>
4. **초기화 순서**: Config → Infrastructure → Network → Views
5. **롤백 가능**: 각 단계 실패 시 역순 정리

금융 시스템답게 **안정성, 추적 가능성, 테스트 용이성**을 최우선으로 설계했다.
