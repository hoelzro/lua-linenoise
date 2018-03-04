local L = require 'linenoise'
-- L.clearscreen()
print '----- Testing lua-linenoise! ------'
local prompt, history = '? ', 'history.txt'
L.historyload(history) -- load existing history
L.setcompletion(function(c,s)
   if s == 'h' then
    c:add('help') -- same as L.addcompletion(c,'help)
    L.addcompletion(c,'halt') -- same as c:add('halt')
  end
end)
L.sethints(function(s)
  if s == 'h' then
    return ' test hint'
  end
end)

L.enableutf8()

local line, err = L.linenoise(prompt)
while line do
    if #line > 0 then
        print(line:upper())
        L.historyadd(line)
        L.historysave(history) -- save every new line
    end
    line, err = L.linenoise(prompt)
end

if err then
  print('An error occurred: ' .. err)
end
