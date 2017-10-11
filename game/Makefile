target=tetris
buildstamp=build.stamp


export PROJECT_ROOT  = .
INCLUDES += -I$(PROJECT_ROOT)

LD=g++
LDFLAGS = -fno-common -pthread -fopenmp
CC=gcc
CFLAGS=-std=c++11
CPPFLAGS=-march=nocona -fPIC -msse3 -mfpmath=sse -DLINUX -D_POSIX_SOURCE -pthread -pipe -fopenmp -std=c++11

ifeq ($(filter $(MAKECMDGOALS), qt), qt)
#qt version
EXT_LIBS += -lqt
CPPFLAGS += -DGAME_WIN_QT
else
#ncurses version
EXT_LIBS += -lncurses
CPPFLAGS += -DGAME_WIN_NCURSES
endif



SRC_DIR=./

SRC_C   = $(sort $(subst $(SRC_DIR)/,,$(shell find -L $(SRC_DIR) -type f -name "*.c")))
SRC     = $(sort $(subst $(SRC_DIR)/,,$(shell find -L $(SRC_DIR) -type f -name "*.cpp")))

OBJS_CPP     = $(SRC:.cpp=.o)
OBJS_C       = $(SRC_C:.c=.o)

LIB_OBJ_DIR  = ./obj
OBJ_DIRS     = $(sort $(dir $(addprefix $(LIB_OBJ_DIR)/,$(OBJS_CPP))\
							$(addprefix $(LIB_OBJ_DIR)/,$(OBJS_C))))

OBJ_STMP     = $(addsuffix $(buildstamp),$(OBJ_DIRS))
OBJ_FILES    = $(addprefix $(LIB_OBJ_DIR)/,$(OBJS_CPP) $(OBJS_C))


DEP_CPP   = $(SRC:%.cpp=%.d)
DEP_C     = $(SRC_C:%.c=%.d)

LIB_DEP_DIR = ./dep

DEP_DIRS  = $(sort $(dir $(addprefix $(LIB_DEP_DIR)/,$(DEP_CPP))\
						 $(addprefix $(LIB_DEP_DIR)/,$(DEP_C))))
DEP_STMP  = $(addsuffix $(buildstamp),$(patsubst $(LIB_DEP_DIR)/$(ASN_BASE_DIRS)/%,,$(DEP_DIRS)))

DEP_FILES = $(addprefix $(LIB_DEP_DIR)/,$(patsubst $(ASN_BASE_DIRS)/%,,$(DEP_CPP) $(DEP_C)))


#implicit rule how to make dependeces for all *.cpp files
$(LIB_DEP_DIR)/%.d:$(SRC_DIR)/%.cpp $(DEP_STMP)
#	@echo $< '->' $@
#	@echo $(CC_DEP) $(CFLAGS) $(INCLUDES) -MT $(LIB_OBJ_DIR)/$*.o -MT $@ -MM -o $@ $<
	@$(CC) $(CFLAGS) $(INCLUDES) -MT $(LIB_OBJ_DIR)/$*.o -MT $@ -MM -o $@ $<

#implicit rule how to make all *.cpp files in home library directory
$(LIB_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(obj_stamp)
	@echo $<
	@$(CC) $(CPPFLAGS) $(INCLUDES) $(EXT_FILE_FLAGS) -o $@ -c $<

all:$(target)

qt:all

$(OBJ_STMP):
	@mkdir -p $(dir $@)
	@touch $@

$(DEP_STMP):
	@mkdir -p $(dir $@)
	@touch $@


$(target):$(OBJ_STMP) $(OBJ_FILES)
	@echo ready to link target $(target)
	@$(LD) $(LDFLAGS) -o $(target) $(OBJ_FILES) $(EXT_LIBS)


clean:
	@rm -fr $(OBJ_DIRS)
	@rm -fr $(DEP_DIRS)
	@rm -fr $(target)



ifeq ($(filter $(MAKECMDGOALS), clean),)
-include $(DEP_FILES)
endif

