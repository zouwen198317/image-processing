## ----------------------------------------------------------------------------
## file    : Makefile
## brief   : Makefile for OpenCV projects.
## module  : image-processing
##
## author  : Teppei Kobayashi <teppei.ts@gmail.com>
## date    : 2016/11/25
## ----------------------------------------------------------------------------


# OPENCV SPECIFICATION ########################################################
OPENCV   := opencv
APPFILES  =


# COMPILE OPTION ##############################################################
CXX       = g++
CXXFLAGS  = -Wall -Wextra -std=c++11
INCLUDES := `pkg-config --cflags $(OPENCV)` -I./include
LIBS     := `pkg-config --libs $(OPENCV)`


# DIRECTORY CONFIGURATION #####################################################
SRCDIR    = ./sources
SOURCES   = $(wildcard $(SRCDIR)/*.cpp)
OBJDIR    = ./objects
OBJECTS   =$(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.cpp=.o)))

TARGET   := a.out


# TARGET CONFIGURATION ########################################################
all: clean $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) $(INCLUDES)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) -o $@ -c $< $(CXXFLAGS) $(LIBS) $(INCLUDES)

.PHONY: clean
clean:
	rm -rf $(OBJDIR) $(TARGET) $(APPFILES)
