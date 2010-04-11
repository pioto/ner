# ner: src/Makefile.am

bin_PROGRAMS = ner

AM_CXXFLAGS = $(gmime_CFLAGS)

ner_LDADD = $(gmime_LIBS)

# Core
ner_SOURCES = \
	main.cc \
	ner.cc ner.hh \
	config_file.cc config_file.hh \
	notmuch.cc notmuch.hh \
	status_bar.cc status_bar.hh \
	view_manager.cc view_manager.hh \
	input_handler.cc input_handler.hh

# Utility
ner_SOURCES += \
	colors.hh \
	util.cc util.hh \
	ncurses.cc ncurses.hh \
	gmime_iostream.cc gmime_iostream.hh

# Views
ner_SOURCES += \
	view.cc view.hh \
	window_view.cc window_view.hh \
	line_browser_view.cc line_browser_view.hh \
	search_view.cc search_view.hh \
	thread_view.cc thread_view.hh \
	email_view.cc email_view.hh \
	message_view.cc message_view.hh \
	thread_message_view.cc thread_message_view.hh \
	view_view.cc view_view.hh

