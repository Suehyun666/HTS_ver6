#pragma once
#include "../../model/AccountSnapshot.h"
#include "core/model/Result.h"

class IAccountQueryService {
public:
    virtual ~IAccountQueryService() = default;

    virtual Result<AccountSnapshot> getAccountSnapshot() = 0;
};
