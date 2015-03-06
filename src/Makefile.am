AM_CPPFLAGS = \
	-DGLM_FORCE_RADIANS

bin_PROGRAMS = shaderomatic

shaderomatic_SOURCES = \
    main.cpp \
	shaderomatic.cpp

noinst_HEADERS = \
	shaderomatic.h

shaderomatic_CXXFLAGS = \
    $(GLFW_CFLAGS) \
	$(OPENCV_CFLAGS) \
	$(GL_CFLAGS) \
	$(BOOST_CPPFLAGS)

shaderomatic_LDADD = \
    $(GLFW_LIBS) \
	$(OPENCV_LIBS) \
	$(GL_LIBS) \
	$(BOOST_SYSTEM_LIBS) \
	$(BOOST_FILESYSTEM_LIBS) \
	$(BOOST_CHRONO_LIBS)
