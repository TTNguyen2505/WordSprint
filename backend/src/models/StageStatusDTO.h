#pragma once

#include "Stage.h"

struct StageStatusDTO {
    Stage stage;
    bool is_unlocked = false;
    int preview_next_stage_id = 0;
};
