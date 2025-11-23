#pragma once
#include "IAccountQueryService.h"

class AccountQueryService : public IAccountQueryService {
public:
    AccountQueryService() = default;

    Result<AccountSnapshot> getAccountSnapshot() override;
};
