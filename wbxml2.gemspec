Gem::Specification.new do |s|
  s.name        = 'wbxml2'
  s.version     = '1.0.0'
  s.date        = '2010-04-28'
  s.summary     = "Hola!"
  s.description = "A simple hello world gem"
  s.authors     = ["Nick Quaranto"]
  s.email       = 'nick@quaran.to'
  s.homepage    =
    'http://rubygems.org/gems/hola'
  s.files = Dir.glob('lib/**/*.rb') +
            Dir.glob('ext/**/*.{c,h,rb}')
  s.extensions = ['ext/wbxml2/extconf.rb']
end
