#pragma once
#include "../../core/model/Result.h"
#include "../../model/AccountSnapshot.h"

class IAccountQueryService {
public:
    virtual ~IAccountQueryService() = default;

    virtual Result<AccountSnapshot> getAccountSnapshot() = 0;
};
