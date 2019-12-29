-- This file is part of roccat-tools.
--
-- roccat-tools is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation, either version 2 of the License, or
-- (at your option) any later version.
--
-- roccat-tools is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.

-- QUOTE Plasma algorithm inspired by a blog post by Menno van Slooten
-- http://www.mennovanslooten.nl/blog/post/72
-- Copyright (c) 2009 Menno van Slooten

-- config

local SCALE = 32
local SLOWDOWN = 100000 -- microseconds
local PLAIN_CENTER = 130
local PLAIN_RANGE = 5

-- helpers

local sdk_to_light_position = {
  [0]={x= 7, y= 2}, [1]={x=15, y= 2}, [2]={x=19, y= 2}, [3]={x=23, y= 2}, [4]={x=27, y= 2},
  [5]={x=33, y= 2}, [6]={x=37, y= 2}, [7]={x=41, y= 2}, [8]={x=45, y= 2}, [9]={x=51, y= 2},
  [10]={x=55, y= 2}, [11]={x=59, y= 2}, [12]={x=63, y= 2}, [13]={x=68, y= 2}, [14]={x=72, y= 2},
  [15]={x=76, y= 2}, [16]={x= 2, y= 7}, [17]={x= 7, y= 7}, [18]={x=11, y= 7}, [19]={x=15, y= 7},
  [20]={x=19, y= 7}, [21]={x=23, y= 7}, [22]={x=27, y= 7}, [23]={x=31, y= 7}, [24]={x=35, y= 7},
  [25]={x=39, y= 7}, [26]={x=43, y= 7}, [27]={x=47, y= 7}, [28]={x=51, y= 7}, [29]={x=55, y= 7},
  [30]={x=63, y= 7}, [31]={x=68, y= 7}, [32]={x=72, y= 7}, [33]={x=76, y= 7}, [34]={x=81, y= 7},
  [35]={x=85, y= 7}, [36]={x=89, y= 7}, [37]={x=93, y= 7}, [38]={x= 2, y=11}, [39]={x= 8, y=11},
  [40]={x=13, y=11}, [41]={x=17, y=11}, [42]={x=21, y=11}, [43]={x=25, y=11}, [44]={x=29, y=11},
  [45]={x=33, y=11}, [46]={x=37, y=11}, [47]={x=41, y=11}, [48]={x=45, y=11}, [49]={x=49, y=11},
  [50]={x=53, y=11}, [51]={x=57, y=11}, [52]={x=62, y=15}, [53]={x=68, y=11}, [54]={x=72, y=11},
  [55]={x=76, y=11}, [56]={x=81, y=11}, [57]={x=85, y=11}, [58]={x=89, y=11}, [59]={x=93, y=13},
  [60]={x= 2, y=15}, [61]={x= 9, y=15}, [62]={x=14, y=15}, [63]={x=18, y=15}, [64]={x=22, y=15},
  [65]={x=26, y=15}, [66]={x=30, y=15}, [67]={x=34, y=15}, [68]={x=38, y=15}, [69]={x=42, y=15},
  [70]={x=46, y=15}, [71]={x=50, y=15}, [72]={x=54, y=15}, [73]={x=58, y=15}, [74]={x=81, y=15},
  [75]={x=85, y=15}, [76]={x=89, y=15}, [77]={x= 2, y=19}, [78]={x= 8, y=19}, [79]={x=12, y=19},
  [80]={x=16, y=19}, [81]={x=20, y=19}, [82]={x=24, y=19}, [83]={x=28, y=19}, [84]={x=32, y=19},
  [85]={x=36, y=19}, [86]={x=40, y=19}, [87]={x=44, y=19}, [88]={x=48, y=19}, [89]={x=52, y=19},
  [90]={x=61, y=19}, [91]={x=72, y=19}, [92]={x=81, y=19}, [93]={x=85, y=19}, [94]={x=89, y=19},
  [95]={x=93, y=21}, [96]={x= 2, y=23}, [97]={x= 8, y=23}, [98]={x=13, y=23}, [99]={x=18, y=23},
  [100]={x=31, y=23}, [101]={x=49, y=23}, [102]={x=53, y=23}, [103]={x=57, y=23}, [104]={x=62, y=23},
  [105]={x=68, y=23}, [106]={x=72, y=23}, [107]={x=76, y=23}, [108]={x=83, y=23}, [109]={x=89, y=23},
}

local function distance(x1, y1, x2, y2)
  local xd = x2 - x1
  local yd = y2 - y1
  return math.sqrt(xd^2 + yd^2)
end

-- returns value in range [1, 256]
local function calc_color_index(x, y, t)
  local value -- range -2 - +2
  value = math.sin(x / 40.74 + t) + math.sin(distance(x, y, SCALE * math.sin(-t) + SCALE, SCALE * math.cos(-t) + SCALE) / 40.74)
  return math.floor((value + 2.0) * 63.75 + 1)
end

-- init

local color_table = {}
for i = 1, 256 do
  color_table[i] = {
    red = 0,
    green = math.floor((math.cos(math.pi * i / SCALE) + 1.0) * 127.5),
    blue = math.floor((math.sin(math.pi * i / SCALE) + 1.0) * 127.5)
  }
end

local t = 0

if ryos.supports_color() then
  ryos.set_all_states(true)
end

ryos.activate()

-- event loop

while true do

  if ryos.wait_for_allowance() then
    break
  end
  
  for sdk, position in pairs(sdk_to_light_position) do
    local color_index = calc_color_index(position.x, position.y, t)
    if ryos.supports_color() then
      ryos.set_key_color(sdk, color_table[color_index])
    else
      ryos.set_key_state(sdk, color_index > PLAIN_CENTER - PLAIN_RANGE and color_index < PLAIN_CENTER + PLAIN_RANGE)
    end
  end
  
  ryos.send()

  if ryos.sleep(SLOWDOWN) then
    break
  end

  t = t + 0.1  
end

-- cleanup

ryos.deactivate()
