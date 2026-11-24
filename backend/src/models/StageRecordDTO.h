#pragma once

#include "StageRecord.h"

#include <string>

struct StageRecordDTO {
    StageRecord record;
    std::string username;
    int rank = 0;
};
