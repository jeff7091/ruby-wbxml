#include <ruby.h>
#include <wbxml/wbxml.h>

static VALUE 
cWBXML2XML_allocate(VALUE klass)
{
  WBXMLConvWBXML2XML *conv = NULL;
  wbxml_conv_wbxml2xml_create(&conv);
  return Data_Wrap_Struct(klass, NULL, wbxml_conv_wbxml2xml_destroy, conv);
}

static void
mWBXML2_validate_setting(VALUE key, VALUE value, ID *key_id_ptr, ID *value_id_ptr) {
  if (T_SYMBOL != TYPE(key)) {
    rb_raise(rb_eTypeError, "single hash is the only valid argument.");
  }
  *key_id_ptr = rb_to_id(key);
  switch (TYPE(value)) {
  case T_SYMBOL:
    *value_id_ptr = rb_to_id(value);
    break;
  case T_TRUE:
    *value_id_ptr = rb_intern("true");
    break;
  case T_FALSE:
  case T_NIL:
    *value_id_ptr = rb_intern("false");
    break;
  default:
    rb_raise(rb_eTypeError, "single hash is the only valid argument.");
  }
}

static int
cWBXML2XML_proc_setting(VALUE key, VALUE value, VALUE self) {
  ID key_id = 0, value_id = 0;
  WBXMLConvWBXML2XML *conv = NULL;
  mWBXML2_validate_setting(key, value, &key_id, &value_id);
  Data_Get_Struct(self, WBXMLConvWBXML2XML, conv);
  if (rb_intern("gen_type") == key_id) {
    WBXMLGenXMLType type = WBXML_GEN_XML_COMPACT;
    if (rb_intern("compact") == value_id) {
      type = WBXML_GEN_XML_COMPACT;
    }
    else if (rb_intern("indent") == value_id) {
      type = WBXML_GEN_XML_INDENT;
    }
    else if (rb_intern("canonical") == value_id) {
      type = WBXML_GEN_XML_CANONICAL;
    }
    else {
      rb_raise(rb_eTypeError, "unknown :gen_type type.");
    }
    wbxml_conv_wbxml2xml_set_gen_type(conv, type);
  }
  else if (rb_intern("lang") == key_id) {
    WBXMLLanguage lang = WBXML_LANG_UNKNOWN;
    if (rb_intern("activesync") == value_id) {
      lang = WBXML_LANG_ACTIVESYNC;
    }
    else if (rb_intern("airsync") == value_id) {
      lang = WBXML_LANG_AIRSYNC;
    }
    // FIXME
    else {
      rb_raise(rb_eTypeError, "unknown :lang language.");
    }
    wbxml_conv_wbxml2xml_set_language(conv, lang);
  } 
  else if (rb_intern("charset") == key_id) {
    ;// FIXME
  }
  else if (rb_intern("indent") == key_id) {
    ;// FIXME
  }
  else if (rb_intern("preserve_whitespaces") == key_id) {
    ;// FIXME
  }
  else {
    rb_raise(rb_eTypeError, "unknown directive passed to new.");
  }
  return ST_CONTINUE;
}

static VALUE 
cXML2WBXML_allocate(VALUE klass)
{
  WBXMLConvXML2WBXML *conv = NULL;
  wbxml_conv_xml2wbxml_create(&conv);
  return Data_Wrap_Struct(klass, NULL, wbxml_conv_xml2wbxml_destroy, conv);
}

static int
cXML2WBXML_proc_setting(VALUE key, VALUE value, VALUE self) {
  ID key_id = 0, value_id = 0;
  WBXMLConvXML2WBXML *conv = NULL;
  mWBXML2_validate_setting(key, value, &key_id, &value_id);
  Data_Get_Struct(self, WBXMLConvXML2WBXML, conv);
  if (rb_intern("version") == key_id) {
    WBXMLVersion version = WBXML_VERSION_UNKNOWN;
    if (rb_intern("v1_0") == value_id) {
      version = WBXML_VERSION_10;
    }
    else if (rb_intern("v1_1") == value_id) {
      version = WBXML_VERSION_11;
    }
    else if (rb_intern("v1_2") == value_id) {
      version = WBXML_VERSION_12;
    }
    else if (rb_intern("v1_3") == value_id) {
      version = WBXML_VERSION_13;
    }
    else {
      rb_raise(rb_eTypeError, "unknown :version value.");
    }
    wbxml_conv_xml2wbxml_set_version(conv, version);
  }
  else if (rb_intern("preserve_whitespaces") == key_id && rb_intern("true") == value_id) {
    wbxml_conv_xml2wbxml_enable_preserve_whitespaces(conv);
  }
  else if (rb_intern("string_table") == key_id && rb_intern("false") == value_id) {
    wbxml_conv_xml2wbxml_disable_string_table(conv);
  }
  else if (rb_intern("public_id") == key_id && rb_intern("false") == value_id) {
    wbxml_conv_xml2wbxml_disable_public_id(conv);
  }
  else {
    rb_raise(rb_eTypeError, "unknown directive passed to new.");
  }
  return ST_CONTINUE;
}

static VALUE 
mWBXML2_initialize(VALUE self, VALUE args, 
		  int (*proc_setting)(VALUE key, VALUE value, VALUE self)) {
  struct RArray *array = NULL;
  Check_Type(args, T_ARRAY);
  array = RARRAY(args);
  if (0 == RARRAY_LEN(array)) {
    return self;
  }
  if (1 != RARRAY_LEN(array)) {
    goto fail;
  }
  Check_Type((RARRAY_PTR(array)[0]), T_HASH);
  rb_hash_foreach(RARRAY_PTR(array)[0], proc_setting, self);
  return self;

  fail:
  rb_raise(rb_eTypeError, "single hash is the only valid argument(1).");
}

static VALUE
cWBXML2XML_initialize(VALUE self, VALUE args) {
  return mWBXML2_initialize(self, args, cWBXML2XML_proc_setting);
}

static VALUE
cXML2WBXML_initialize(VALUE self, VALUE args) {
  return mWBXML2_initialize(self, args, cXML2WBXML_proc_setting);
}

typedef WBXMLError (*mWBXML2_run_conv)(void *conv,
				       WB_UTINY *input_buf, WB_ULONG input_len,
				       WB_UTINY **output_buf_ptr, WB_ULONG *output_len_ptr);

static VALUE
mWBXML2_convert(VALUE self, VALUE input, void *conv, mWBXML2_run_conv run_conv) {
  WBXMLError code = WBXML_OK;
  WB_UTINY *output_buf_ptr = NULL;
  WB_ULONG output_len = 0;
  VALUE retval = Qnil;
  if (WBXML_OK != (code = run_conv(conv, (WB_UTINY*)StringValuePtr(input), 
				   RSTRING_LENINT(input),
				   &output_buf_ptr,
				   &output_len))) {
    rb_raise(rb_eRuntimeError, "conversion error: %s.", wbxml_errors_string(code));
  }
  retval = rb_str_new((char*)output_buf_ptr, output_len);
  free(output_buf_ptr);
  return retval;
}

static VALUE
cXML2WBXML_to_wbxml(VALUE self, VALUE xml) {
  WBXMLConvXML2WBXML *conv = NULL;
  Data_Get_Struct(self, WBXMLConvXML2WBXML, conv);
  return mWBXML2_convert(self, xml, conv, (mWBXML2_run_conv)wbxml_conv_xml2wbxml_run);
}

static VALUE
cWBXML2XML_to_xml(VALUE self, VALUE xml) {
  WBXMLConvWBXML2XML *conv = NULL;
  Data_Get_Struct(self, WBXMLConvWBXML2XML, conv);
  return mWBXML2_convert(self, xml, conv, (mWBXML2_run_conv)wbxml_conv_wbxml2xml_run);
}

void 
Init_wbxml2()
{
  VALUE mWBXML2 = rb_define_module("WBXML2");
  {
    VALUE cWBXML2XML = rb_define_class_under(mWBXML2, "WBXML2XML", rb_cObject);
    rb_define_alloc_func(cWBXML2XML, cWBXML2XML_allocate);
    rb_define_method(cWBXML2XML, "initialize", cWBXML2XML_initialize, -2);
    rb_define_method(cWBXML2XML, "to_xml", cWBXML2XML_to_xml, 1);
  }
  {
    VALUE cXML2WBXML = rb_define_class_under(mWBXML2, "XML2WBXML", rb_cObject);
    rb_define_alloc_func(cXML2WBXML, cXML2WBXML_allocate);
    rb_define_method(cXML2WBXML, "initialize", cXML2WBXML_initialize, -2);
    rb_define_method(cXML2WBXML, "to_wbxml", cXML2WBXML_to_wbxml, 1);
  }
}
