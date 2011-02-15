# ner: src/Makefile.am

bin_PROGRAMS = ner

AM_CXXFLAGS = $(yaml_cpp_CFLAGS) $(gmime_CFLAGS) $(gio_CFLAGS) -D_XOPEN_SOURCE_EXTENDED

ner_LDADD = $(yaml_cpp_LIBS) $(gmime_LIBS) $(gio_LIBS)

# Core
ner_SOURCES = \
	main.cc \
	ner.cc ner.hh \
	ner_config.cc ner_config.hh \
	notmuch.cc notmuch.hh \
	status_bar.cc status_bar.hh \
	view_manager.cc view_manager.hh \
	input_handler.cc input_handler.hh \
	identity_manager.cc identity_manager.hh \
	mail_store.cc mail_store.hh \
	maildir.cc maildir.hh \
	line_editor.cc line_editor.hh \
	message_part.cc message_part.hh \
	message_part_visitor.hh \
	message_part_display_visitor.cc message_part_display_visitor.hh \
	message_part_save_visitor.cc message_part_save_visitor.hh \
	message_part_text_visitor.hh

# Utility
ner_SOURCES += \
	colors.hh \
	util.cc util.hh \
	ncurses.cc ncurses.hh \
	gmime_iostream.cc gmime_iostream.hh \
	line_wrapper.cc line_wrapper.hh

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
	view_view.cc view_view.hh \
	email_edit_view.cc email_edit_view.hh \
	compose_view.cc compose_view.hh \
	reply_view.cc reply_view.hh \
	search_list_view.cc search_list_view.hh

