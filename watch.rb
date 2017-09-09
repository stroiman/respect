watch('lib\/.*\.js') do |md| 
  system("node ./lib/js/src/demo.js")
end
