import assert from "node:assert/strict";
import test from "node:test";
import { formatTime } from "./formatTime.js";

test("formats seconds as m:ss and handles missing or invalid values", () => {
    assert.equal(formatTime(0), "0:00");
    assert.equal(formatTime(9), "0:09");
    assert.equal(formatTime(65), "1:05");
    assert.equal(formatTime(600), "10:00");
    assert.equal(formatTime(null), "-");
    assert.equal(formatTime(undefined), "-");
    assert.equal(formatTime(Number.NaN), "-");
    assert.equal(formatTime(-5), "0:00");
    assert.equal(formatTime(null, "0:00"), "0:00");
});
