# Makefile for Flight Simulator (Linux/Ubuntu)
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Wno-unused-parameter
DEBUGFLAGS = -g -DDEBUG

# インクルードパス（pkg-configで自動取得）
INCLUDES = -Isrc $(shell pkg-config --cflags glfw3 glew)

# リンクライブラリ
LIBS = $(shell pkg-config --libs glfw3 glew) -lGL -lGLU -lm

# ディレクトリ
SRC_DIR = src
BUILD_DIR = build

# ターゲット（.exeなし）
TARGET = flight_sim
TARGET_DEBUG = flight_sim_debug

# ソースファイル
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/plane.cpp \
          $(SRC_DIR)/target.cpp \
          $(SRC_DIR)/camera.cpp \
          $(SRC_DIR)/terrain.cpp \
          $(SRC_DIR)/renderer.cpp

# オブジェクトファイル
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
OBJECTS_DEBUG = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%_debug.o,$(SOURCES))

# ヘッダーファイル
HEADERS = $(SRC_DIR)/plane.h \
          $(SRC_DIR)/target.h \
          $(SRC_DIR)/camera.h \
          $(SRC_DIR)/terrain.h \
          $(SRC_DIR)/renderer.h \
          $(SRC_DIR)/model.h \
          $(SRC_DIR)/vector3.h \
          $(SRC_DIR)/common.h

.PHONY: all clean debug run help

all: $(BUILD_DIR) $(TARGET)

debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(BUILD_DIR) $(TARGET_DEBUG)

# ビルドディレクトリ作成
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# リリースビルド
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)
	@echo "Build complete: $(TARGET)"

# デバッグビルド
$(TARGET_DEBUG): $(OBJECTS_DEBUG)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) -o $(TARGET_DEBUG) $(OBJECTS_DEBUG) $(LIBS)
	@echo "Debug build complete: $(TARGET_DEBUG)"

# オブジェクトファイルのコンパイル（リリース）
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# オブジェクトファイルのコンパイル（デバッグ）
$(BUILD_DIR)/%_debug.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(DEBUGFLAGS) $(INCLUDES) -c $< -o $@

# クリーンアップ
clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TARGET_DEBUG)
	@echo "Cleanup complete"

# 実行
run: $(TARGET)
	./$(TARGET)

# ヘルプ
help:
	@echo "Available targets:"
	@echo "  all    - Build release version (default)"
	@echo "  debug  - Build debug version"
	@echo "  clean  - Remove build files"
	@echo "  run    - Build and run the program"
	
