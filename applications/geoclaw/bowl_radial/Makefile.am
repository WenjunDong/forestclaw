## Makefile.am in applications/geoclaw/bowl_radial

bin_PROGRAMS += applications/geoclaw/bowl_radial/bowl

applications_geoclaw_bowl_radial_bowl_SOURCES = \
	applications/geoclaw/bowl_radial/bowl_user.cpp \
	applications/geoclaw/bowl_radial/bowl_user.h \
	applications/geoclaw/bowl_radial/bowl.cpp

EXTRA_DIST += \
	applications/geoclaw/bowl_radial/fclaw_options.ini \
	applications/geoclaw/bowl_radial/make_data.py \
	applications/geoclaw/bowl_radial/make_plots.py \
	applications/geoclaw/bowl_radial/make_topo.py \
	applications/geoclaw/bowl_radial/afterframe.m \
	applications/geoclaw/bowl_radial/setplot2.m \
	applications/geoclaw/bowl_radial/bathy.m \
	applications/geoclaw/bowl_radial/bathy1d.m \
	applications/geoclaw/bowl_radial/map1d.m \
	applications/geoclaw/bowl_radial/compare_gauges.m

applications_geoclaw_bowl_radial_bowl_CPPFLAGS = \
        $(AM_CPPFLAGS) $(FCLAW_GEOCLAW_CPPFLAGS)

applications_geoclaw_bowl_radial_bowl_LDADD = \
        $(LDADD) $(FCLAW_GEOCLAW_LDADD) 

