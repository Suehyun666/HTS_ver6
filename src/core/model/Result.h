#pragma once
#include <QString>
#include <variant>
#include <optional>

enum class ErrorCode {
    Success = 0,

    // Infrastructure
    ConfigLoadFailed = 1000,
    NetworkInitFailed = 1001,
    MemoryPoolInitFailed = 1002,

    // Connection
    GrpcConnectionFailed = 2000,
    StreamConnectionFailed = 2001,
    AuthenticationFailed = 2002,
    SessionExpired = 2003,

    // Business
    InvalidInput = 3000,
    OrderRejected = 3001,
    InsufficientBalance = 3002,

    // System
    Unknown = 9999
};

struct Error {
    ErrorCode code;
    QString message;

    Error(ErrorCode c, const QString& msg) : code(c), message(msg) {}

    bool isRecoverable() const {
        return code == ErrorCode::NetworkInitFailed ||
               code == ErrorCode::GrpcConnectionFailed;
    }
};

template<typename T>
class Result {
public:
    static Result success(T&& value) {
        return Result(std::forward<T>(value));
    }

    static Result failure(ErrorCode code, const QString& message) {
        return Result(Error(code, message));
    }

    bool isOk() const { return std::holds_alternative<T>(data_); }
    bool isError() const { return !isOk(); }

    const T& value() const { return std::get<T>(data_); }
    T& value() { return std::get<T>(data_); }

    const Error& error() const { return std::get<Error>(data_); }

    T valueOr(T&& defaultValue) const {
        return isOk() ? value() : std::forward<T>(defaultValue);
    }

private:
    explicit Result(T&& val) : data_(std::forward<T>(val)) {}
    explicit Result(const Error& err) : data_(err) {}

    std::variant<T, Error> data_;
};

// void 특수화
template<>
class Result<void> {
public:
    static Result success() {
        return Result(true);
    }

    static Result failure(ErrorCode code, const QString& message) {
        return Result(Error(code, message));
    }

    bool isOk() const { return success_; }
    bool isError() const { return !success_; }

    const Error& error() const { return error_.value(); }

private:
    explicit Result(bool success) : success_(success) {}
    explicit Result(const Error& err) : success_(false), error_(err) {}

    bool success_;
    std::optional<Error> error_;
};
