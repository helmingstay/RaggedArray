require(devtools)
load_all('.', recompile=T)
test('.')
