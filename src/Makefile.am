bin_PROGRAMS = shaderomatic

shaderomatic_SOURCES = \
    main.cpp \
	shaderomatic.cpp

shaderomatic_CXXFLAGS = \
    $(LIBGLFW_CFLAGS) \
	$(OPENCV_CFLAGS) \
	$(GL_CFLAGS) \
	$(BOOST_CPPFLAGS) \
	$(GLIB_CFLAGS)

shaderomatic_LDADD = \
    $(LIBGLFW_LIBS) \
	$(OPENCV_LIBS) \
	$(GL_LIBS) \
	$(BOOST_SYSTEM_LIBS) \
	$(BOOST_FILESYSTEM_LIBS) \
	$(BOOST_CHRONO_LIBS) \
	$(GLIB_LIBS)
