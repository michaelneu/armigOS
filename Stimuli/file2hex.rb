#!/usr/bin/env ruby

# Copyright (C) 2016 michaelneu
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

require "optparse"


options = {
  start: 0,
  keep_cr: false
}

OptionParser.new do |opts|
  opts.banner = "Encodes the given file to the Intel hex format. \n\nUsage: file2hex.rb [options] file [> output.hex]"

  opts.on("-s", "--start ADDRESS", "start address of the file") { |s| options[:start] = s }
  opts.on("-c", "--keep-cr", "don't delete '\\r'") { |c| options[:keep_cr] = c }
end.parse!

filename = ARGV.pop

raise "no filename specified" unless filename


content = File.open(filename, "r").read
content.delete!("\r") unless options[:keep_cr]

chars = content.chars

len = chars.length
i = 0

lines = []
buffer = ""

while i < len do
  buffer << chars[i]
  i += 1

  if i % 16 == 0
    lines.push(buffer)
    buffer = ""
  end
end

output = ""
address = options[:start]

lines.each do |line|
  buffer = ""

  line.chars.each { |char| buffer << "%02X" % [char.ord] }

  output << ":%02X%04X00#{buffer}00\n" % [line.length, address]

  address += line.length
end

output << ":00000001FF\n"

puts output