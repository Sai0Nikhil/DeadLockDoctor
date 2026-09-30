CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -O2 -Iinclude -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -pthread
ASAN_FLAGS = -fsanitize=address,undefined -g

SRC_CORE = src/core/resource_mgr.c src/core/graph.c
SRC_AVOID = src/avoidance/banker.c
SRC_DETECT = src/detection/detector.c
SRC_RECOVER = src/recovery/recovery.c
SRC_PREVENT = src/prevention/prevention.c
SRC_SCENARIO = src/scenario/scenario.c
SRC_DEMO = src/demo/live_demo.c
SRC_UI = src/ui/visualizer.c

ALL_ENGINE_SRCS = $(SRC_CORE) $(SRC_AVOID) $(SRC_DETECT) $(SRC_RECOVER) $(SRC_PREVENT) $(SRC_SCENARIO) $(SRC_DEMO) $(SRC_UI)

TEST_SRCS = tests/test_main.c tests/test_banker.c tests/test_detection.c tests/test_recovery.c tests/test_prevention.c tests/test_scenario.c

TARGET = bin/deadlockdoctor
TEST_TARGET = bin/test_suite

.PHONY: all clean test demo memcheck directories symlinks

all: directories $(TARGET) symlinks

directories:
	@mkdir -p bin build/core build/avoidance build/detection build/recovery build/prevention build/scenario build/demo build/ui build/tests

$(TARGET): $(ALL_ENGINE_SRCS) src/main.c
	$(CC) $(CFLAGS) -o $@ $^

symlinks: $(TARGET)
	@cp -f $(TARGET) deadlockdoctor 2>/dev/null || true
	@cp -f $(TARGET) deadlocker 2>/dev/null || true
	@cp -f $(TARGET) shellforge 2>/dev/null || true

test: directories $(TEST_TARGET)
	@./$(TEST_TARGET)

$(TEST_TARGET): $(ALL_ENGINE_SRCS) $(TEST_SRCS)
	$(CC) $(CFLAGS) -o $@ $^

demo: all
	@./$(TARGET) --demo

memcheck: directories
	$(CC) $(CFLAGS) $(ASAN_FLAGS) -o bin/test_asan $(ALL_ENGINE_SRCS) $(TEST_SRCS)
	@./bin/test_asan

clean:
	rm -rf bin build deadlockdoctor deadlocker shellforge
