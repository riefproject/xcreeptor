# Qt specific settings
QMAKE = qmake
MOC = moc
QT_INCLUDE = $(shell pkg-config --cflags Qt5Widgets)
QT_LIBS = $(shell pkg-config --libs Qt5Widgets)

# Compiler settings
CXX = g++
CXXINCLUDE = -I./include \
	-I./include/ui \
	-I./include/core/crypto \
	-I./include/core/utils
CXXFLAGS = -std=c++17 -Wall -Wextra $(CXXINCLUDE) $(QT_INCLUDE)
LDFLAGS = -lcrypto -lssl $(QT_LIBS)

# Directories
BINDIR = bin
BUILDDIR = build
ASSETSDIR = assets

# Source files
SOURCES = src/main.cpp \
	src/core/utils/utils.cpp \
	src/core/crypto/encrypt.cpp \
	src/core/crypto/decrypt.cpp \
	src/core/crypto/keyManager.cpp \
	src/core/crypto/account.cpp \
	src/ui/mainWindow.cpp

# MOC headers
MOC_HEADERS = include/ui/mainWindow.hpp
MOC_SOURCES = $(addprefix $(BUILDDIR)/moc_,$(notdir $(MOC_HEADERS:.hpp=.cpp)))

# Object files
OBJECTS = $(SOURCES:src/%.cpp=$(BUILDDIR)/%.o) $(MOC_SOURCES:.cpp=.o)

# Main target executable
TARGET = $(BINDIR)/decoder

# Create build directories
$(shell mkdir -p $(BINDIR) $(BUILDDIR)/core/utils $(BUILDDIR)/core/crypto $(BUILDDIR)/ui)

# Create assets directory in bin
$(shell mkdir -p $(BINDIR)/assets/keys)
$(shell cp -r $(ASSETSDIR)/keys/* $(BINDIR)/assets/keys/ 2>/dev/null || :)

# Default target
all: $(TARGET)

# MOC compilation rule
$(BUILDDIR)/moc_%.cpp: include/ui/%.hpp
	@echo "🔨 Generating MOC file for $<..."
	$(MOC) $(QT_INCLUDE) $< -o $@

# Linking rule
$(TARGET): $(OBJECTS)
	@echo "🔧 Linking object files..."
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "✅ Build successful! Run './$(TARGET)'"

# Compilation rules
$(BUILDDIR)/%.o: src/%.cpp
	@echo "🔨 Compiling $<..."
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILDDIR)/%.o: $(BUILDDIR)/%.cpp
	@echo "🔨 Compiling generated file $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	@echo "🗑 Cleaning build and binary directories..."
	@rm -rf $(BUILDDIR)/* $(TARGET)
	@rm -rf $(BINDIR)/assets
	@echo "✅ Clean complete!"

# Run the program
run: $(TARGET)
	@echo "🚀 Running the program..."
	@cd $(BINDIR) && ./decoder

.PHONY: all clean run