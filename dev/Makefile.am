INCLUDES = -I$(top_srcdir)/include

AM_CFLAGS = $(GLOBAL_CFLAGS) $(libglib2_CFLAGS) $(libgnutls_CFLAGS) $(libtasn1_CFLAGS) $(libgthread2_CFLAGS) $(LFS_CFLAGS)
AM_LDFLAGS = $(libglib2_LIBS) $(libgnutls_LIBS) $(libtasn1_LIBS) $(libgthread2_LIBS)

bin_PROGRAMS = iphoneclient lckd-client afccheck msyncclient iphoneinfo iphonesyslog iphone_id

iphoneclient_SOURCES = main.c
iphoneclient_LDADD = ../src/libiphone.la

lckd_client_SOURCES = lckdclient.c
lckd_client_CFLAGS = $(AM_CFLAGS)
lckd_client_LDFLAGS = -lreadline $(AM_LDFLAGS)
lckd_client_LDADD = ../src/libiphone.la

afccheck_SOURCES = afccheck.c
afccheck_CFLAGS = $(AM_CFLAGS)
afccheck_LDFLAGS = $(AM_LDFLAGS)
afccheck_LDADD = ../src/libiphone.la

msyncclient_SOURCES = msyncclient.c
msyncclient_CFLAGS = $(AM_CFLAGS)
msyncclient_LDFLAGS = $(AM_LDFLAGS)
msyncclient_LDADD = ../src/libiphone.la

iphoneinfo_SOURCES = iphoneinfo.c
iphoneinfo_CFLAGS = $(AM_CFLAGS)
iphoneinfo_LDFLAGS = $(AM_LDFLAGS)
iphoneinfo_LDADD = ../src/libiphone.la

iphonesyslog_SOURCES = syslog_relay.c
iphonesyslog_CFLAGS = $(AM_CFLAGS)
iphonesyslog_LDFLAGS = $(AM_LDFLAGS)
iphonesyslog_LDADD = ../src/libiphone.la

iphone_id_SOURCES = iphone_id.c
iphone_id_CFLAGS = $(AM_CFLAGS)
iphone_id_LDFLAGS = $(AM_LDFLAGS)
iphone_id_LDADD = ../src/libiphone.la
