
# C Compiler include & library path setting
CC          = gcc ${CC_CFLAGS} ${CC_IPATH}
LN          = ${CC_COMPILER}
PC          = ${PC_COMPILER} ${PC_FLAGS}  ${PC_IPATH} ${PC_LPATH}
LB          = ar -r
CP          = cp
RM          = rm -f
MV          = mv


#-------------------------------------------------

.SUFFIXES:
.SUFFIXES: .o .c
.SUFFIXES: .o .cpp
.SUFFIXES: .o .pc
.SUFFIXES: .obj .c
.SUFFIXES: .obj .cpp
.SUFFIXES: .obj .pc
.SUFFIXES: .c .pc

net.o: net.c fmacros.h net.h hiredis.h
async.o: async.c async.h hiredis.h sds.h dict.c dict.h
hiredis.o: hiredis.c fmacros.h hiredis.h net.h sds.h
sds.o: sds.c sds.h

#-------------------------------------------------

TARGET      = _data

AIM_PATH    = $(DIST_AIM_PATH)

SHARE_OBJS  =

NEW_OBJS    =   async.o hiredis.o net.o  sds.o


#-------------------------------------------------

all: $(TARGET)

$(TARGET): build
#	$(LN) $(NEW_OBJS) $(SHARE_OBJS) ${CC_LPATH} -o $@

build: $(NEW_OBJS)

clear:
	@$(RM) $(TARGET) $(NEW_OBJS) *.o *.obj *.lis *.pdb *.ilk

setup: $(TARGET)
	@$(CP) $(SHARE_OBJS) $(NEW_OBJS) $(AIM_PATH)

clean:
	@$(RM) $(TARGET) $(NEW_OBJS)


#-------------------------------------------------
