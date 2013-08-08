require 'mkmf'

LIBDIR      = RbConfig::CONFIG['libdir']
INCLUDEDIR  = RbConfig::CONFIG['includedir']

HEADER_DIRS = [
  # First search /opt/local for macports
  '/opt/local/include',

  # Then search /usr/local for people that installed from source
  '/usr/local/include',

  # Check the ruby install locations
  INCLUDEDIR,

  # Finally fall back to /usr
  '/usr/include',
]

LIB_DIRS = [
  # First search /opt/local for macports
  '/opt/local/lib',

  # Then search /usr/local for people that installed from source
  '/usr/local/lib',

  # Check the ruby install locations
  LIBDIR,

  # Finally fall back to /usr
  '/usr/lib',
]

dir_config('wbxml2', HEADER_DIRS, LIB_DIRS)

unless find_header('wbxml/wbxml.h')
  abort "libwbxml2 is missing.  please install libwbxml2"
end

unless find_library('wbxml2', 'wbxml_conv_xml2wbxml_run')
  abort "libwbxml2 is missing.  please install libwbxml2"
end

create_makefile('wbxml2/wbxml2')
