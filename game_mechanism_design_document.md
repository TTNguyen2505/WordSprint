# Vocabulary Quiz Game — Game Mechanism Design

This document defines the core gameplay rules, progression logic, quiz generation constraints, scoring and streak formulas, and leaderboard rankings backed by your 5-table SQLite schema (`users`, `topics`, `stages`, `word_pool`, and `stage_records`).

---

## 1. Core Gameplay Loop

1. **Profile Selection:** The player selects an existing profile (or creates a new username), loading their personal progression and audio preferences (`bgm_enabled`, `sfx_enabled`, `bgm_volume`, `sfx_volume`).
2. **Tier & Topic Selection:** The player chooses any unlocked CEFR tier (`A1` through `C2`) and selects a topic stage.
3. **Quiz Session:** The game draws `question_count` random words from `word_pool` matching `(topic_id, cefr_level)` and presents them in a balanced random mix of **Scramble**, **Fill-in-the-Blank**, and **Audio** prompts within `time_limit`.
4. **Result & Recording:** When all questions are answered or `time_limit` expires, the run is saved to `stage_records` with `score`, `completion_time`, and `played_at`.
5. **Unlock & Ranking Update:** If `score >= pass_score`, the next CEFR tier unlocks across all topics, and the run updates both the **Per-Stage** and **Overall** leaderboards.

---

## 2. Progression & Tier-Based Unlocking

Progression follows a **tier-based CEFR unlock model** rather than a strict linear stage chain:

* **Initial State:** All topics in `A1` are unlocked by default for new profiles. Tiers `A2` through `C2` start locked.
* **Tier Unlock Rule:** Passing **any single topic stage** (`score >= pass_score`) at CEFR level $N$ immediately unlocks **all topics** at CEFR level $N + 1$.
* **Replay Freedom:** Players can replay any unlocked stage at any time to improve their high score, set a faster `completion_time`, or claim multiple spots on a stage's Top 10 leaderboard.

---

## 3. Stage Structure & Pacing

Each stage defines its duration (`time_limit`), length (`question_count`), and clearing threshold (`pass_score`) based on its CEFR difficulty tier:

| CEFR Level | `question_count` | `time_limit` (Seconds) | Base Pts / Word | Base Max Score | `pass_score` (Min Correct) |
| :--- | :--- | :--- | :--- | :--- | :--- |
| **A1** (Beginner) | `10` | `90` ($1\text{m } 30\text{s}$) | $10$ | $100$ | `70` ($7 / 10 = 70\%$) |
| **A2** (Elementary) | `10` | `100` ($1\text{m } 40\text{s}$) | $15$ | $150$ | `105` ($7 / 10 = 70\%$) |
| **B1** (Intermediate) | `12` | `120` ($2\text{m } 00\text{s}$) | $20$ | $240$ | `160` ($8 / 12 \approx 67\%$) |
| **B2** (Upper-Int.) | `12` | `135` ($2\text{m } 15\text{s}$) | $25$ | $300$ | `225` ($9 / 12 = 75\%$) |
| **C1** (Advanced) | `15` | `165` ($2\text{m } 45\text{s}$) | $30$ | $450$ | `330` ($11 / 15 \approx 73\%$) |
| **C2** (Mastery) | `15` | `180` ($3\text{m } 00\text{s}$) | $35$ | $525$ | `385` ($11 / 15 \approx 73\%$) |

### Timer Rules
* **Stage Countdown:** The timer counts down continuously from `time_limit` to $0$.
* **Early Finish:** Completing the final question stops the clock immediately, recording `completion_time = time_limit - remaining_time`.
* **Timeout:** If the timer reaches $0$, the session ends immediately and saves whatever score the player accumulated up to that point.
* **Audio Playback Pause (Optional):** Pause the countdown while an audio prompt is actively playing so longer word recordings do not penalize the player's available thinking time.

---

## 4. Quiz Modes & Difficulty Scaling

Every stage mixes three quiz modes using **bag randomization** (building an evenly distributed list of modes for `question_count` and shuffling it) so players never face long streaks of the same prompt type.

| CEFR Tier | Scramble Rule | Fill-in-the-Blank Hidden Ratio | Audio Manual Replays | Hint Image (`hint_image_path`) |
| :--- | :--- | :--- | :--- | :--- |
| **A1-A2** | Full random shuffle | **$35\%$** hidden (1st letter always shown) | **$3$ replays** ($+1$ auto-play) | Shown in Scramble & Blank; hidden in Audio |
| **B1-B2** | Full random shuffle | **$50\%$** hidden (random positions) | **$2$ replays** ($+1$ auto-play) | Shown in Scramble & Blank; hidden in Audio |
| **C1-C2** | Full random shuffle | **$65\%$** hidden (random positions) | **$1$ replay** ($+1$ auto-play) | Shown in Scramble & Blank; hidden in Audio |

### Mode-Specific Constraints
* **Scramble:**
  * Shuffle letters randomly until $\text{shuffled} \ne \text{target\_word}$ so the prompt never accidentally displays the solved word.
  * For multi-word phrases, scramble letters within each individual word while keeping spaces in place.
* **Fill-in-the-Blank:**
  * Compute hidden letter count dynamically at runtime:
    $$\text{hidden\_count} = \text{clamp}\left(\text{round}(L \times r),\ 1,\ L - 1\right)$$
    where $L$ is the number of alphabetic characters and $r \in \{0.35, 0.50, 0.65\}$.
  * Never hide spaces, hyphens, or apostrophes. In `A1` and `A2`, lock index $0$ (the first letter) as visible before selecting random indices to hide.
* **Audio:**
  * Assign Audio mode to words with a non-empty `target_word`. Use browser speech synthesis to pronounce the word; if synthesis fails, try the optional `audio_path` resource embedded under `qrc:/storage/audio/`.
  * Auto-play the pronunciation once when the question loads (free of charge), then display remaining manual replays on the replay button (e.g., `2/2`, `1/2`, `0/2`) and disable it once exhausted. Manual replays are limited by CEFR tier: 3 for A1–A2, 2 for B1–B2, and 1 for C1–C2.
  * Hide `hint_image_path` during Audio questions so players must rely on listening comprehension.

---

## 5. Scoring & Streak Bonus System

To keep stage progression fair while rewarding consistent accuracy, **base points** scale by CEFR tier, **streak bonuses** reward consecutive correct answers, and **speed** is tracked separately in `completion_time` for leaderboard tie-breaking.

### Formula
Let $B$ be the base points per word for the stage's CEFR tier ($B \in \{10, 15, 20, 25, 30, 35\}$), and let $k$ be the player's current consecutive correct answer streak ($k = 1, 2, 3, \dots$):

$$\text{Points Earned} = \begin{cases} B, & \text{if } 1 \le k \le 2 \\ B + 2, & \text{if } k \ge 3 \end{cases}$$

* **Building a Streak:** Answering a question correctly increments $k \leftarrow k + 1$. Starting on the **3rd consecutive correct answer** ($k \ge 3$), every subsequent correct answer in the streak awards a flat **$+2$ bonus points**.
* **Breaking a Streak:** Answering incorrectly (or skipping a question) awards $0$ points and resets $k \leftarrow 0$.
* **Maximum Possible Score with Full Streak:** For a stage with $Q$ questions ($Q \ge 3$), answering all $Q$ questions correctly yields:
  $$\text{Max Score} = (Q \times B) + 2(Q - 2)$$
  For example, a perfect run on an `A1` stage ($Q = 10, B = 10$) earns $100 + 16 = 116$ points.
* **Calibration Safeguard:** Keep `pass_score` anchored to $\approx 70\%$ of the **base** score ($Q \times B$) so players cannot pass a stage purely through streak luck with low overall accuracy, and keep speed out of `score` so careful learners are never penalized for reading slowly.

---

## 6. Leaderboards & Tie-Breaking

All rankings read directly from `stage_records` without needing a separate progress table:

### A. Per-Stage Top 10 (Classic Arcade Style)
* **Rule:** Every completed run is eligible, allowing a single player to hold multiple spots on the same stage's Top 10 list.
* **Sorting:** Ranked by `score DESC`, with `completion_time ASC` as the primary tie-breaker (and `played_at ASC` if both score and time match).

### B. Overall Top 10 (Sum of Best per Stage)
* **Rule:** Each player appears **at most once** on the global leaderboard. For every stage a player has played, only their single highest-scoring run (`ORDER BY score DESC, completion_time ASC`) is selected, and those best scores are summed together.
* **Why This Works:** Players cannot inflate their global rank by repeatedly farming easy `A1` stages—climbing the Overall Top 10 requires clearing higher CEFR tiers and completing more topics.
* **Sorting:** Ranked by `overall_score DESC`, using `total_time ASC` (the sum of `completion_time` across their best runs) as the tie-breaker.

---

## 7. Multi-Profile Switching & Audio Preferences

* **Shared-Device Profiles:** All records in `stage_records` and audio settings in `users` are tied to `user_id`. Switching profiles in the UI simply updates the active `user_id` in local app state and reloads:
  1. Unlocked CEFR tiers (`MAX(cefr_level)` where `score >= pass_score` for that `user_id`).
  2. Audio preferences (`bgm_enabled`, `sfx_enabled`, `bgm_volume`, `sfx_volume`).
  3. Personal high scores and replay history.