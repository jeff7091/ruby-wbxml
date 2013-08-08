require "minitest/autorun"
require "wbxml2"

def hex_to_bin(s)
 s.scan(/../).map { |x| x.hex.chr }.join
end

def bin_to_hex(s)
  s.each_byte.map { |b| "%02x" % b }.join
  #s.each_byte.map { |b| b.to_s(16) }.join
end

class TestWBXML2 < MiniTest::Unit::TestCase
  def setup
    @my_wb2x = WBXML2::WBXML2XML.new :lang => :activesync, :gen_type => :canonical
    @my_x2wb = WBXML2::XML2WBXML.new :version => :v1_3, :public_id => false
  end

  def test_to_xml
    input = hex_to_bin("03016a00454e033133000101")
    result = @my_wb2x.to_xml input
    assert_equal result, '<?xml version="1.0"?><!DOCTYPE ActiveSync PUBLIC "-//MICROSOFT//DTD ActiveSync//EN" "http://www.microsoft.com/"><Sync xmlns="AirSync:"><Status>13</Status></Sync>'
  end

  def test_to_wbxml
    input = '<?xml version="1.0"?><!DOCTYPE ActiveSync PUBLIC "-//MICROSOFT//DTD ActiveSync//EN" "http://www.microsoft.com/"><Sync xmlns="AirSync:"><Status>13</Status></Sync>'
    result = @my_x2wb.to_wbxml input
    assert_equal result, hex_to_bin("03016a00454e033133000101")
  end
end
