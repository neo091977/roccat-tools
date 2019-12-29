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

-- QUOTE Fire algorithm inspired by The Demo Effects Collection
-- http://demo-effects.sourceforge.net/
-- Copyright (c) 2002 W.P. van Paassen - peter@paassen.tmfweb.nl

-- config

-- The waiting time in microseconds determines the speed of the effect.
local SLOWDOWN = 100000

-- Decay determines how high the flames get. The larger the value, the smaller the
-- flames.
local DECAY = 15

-- helpers

local WIDTH = 23

-- Keyboard uses 6 rows. Additional row is invisible and contains initial on/off
-- values.
local HEIGHT = 7

-- It makes no sense to use a fine grain position table here, so keys are mapped in a simple 23x6 matrix here
local sdk_to_linear_position = {
               [0]=  1,              [1]=  3,   [2]=  4,   [3]=  5,   [4]=  6,              [5]=  8,   [6]=  9,   [7]= 10,   [8]= 11,   [9]= 12,  [10]= 13,  [11]= 14,  [12]= 15,  [13]= 16,  [14]= 17,  [15]= 18,
   [16]= 23,  [17]= 24,  [18]= 25,  [19]= 26,  [20]= 27,  [21]= 28,  [22]= 29,  [23]= 30,  [24]= 31,  [25]= 32,  [26]= 33,  [27]= 34,  [28]= 35,  [29]= 36,             [30]= 38,  [31]= 39,  [32]= 40,  [33]= 41,  [34]= 42,  [35]= 43,  [36]= 44,  [37]=  45,
   [38]= 46,  [39]= 47,  [40]= 48,  [41]= 49,  [42]= 50,  [43]= 51,  [44]= 52,  [45]= 53,  [46]= 54,  [47]= 55,  [48]= 56,  [49]= 57,  [50]= 58,  [51]= 59,             [52]= 61,  [53]= 62,  [54]= 63,  [55]= 64,  [56]= 65,  [57]= 66,  [58]= 67,  [59]=  68,
   [60]= 69,  [61]= 70,             [62]= 72,  [63]= 73,  [64]= 74,  [65]= 75,  [66]= 76,  [67]= 77,  [68]= 78,  [69]= 79,  [70]= 80,  [71]= 81,  [72]= 82,  [73]= 83,                                                         [74]= 89,  [75]= 90,  [76]=  91,
   [77]= 92,  [78]= 93,  [79]= 94,  [80]= 95,  [81]= 96,  [82]= 97,  [83]= 98,  [84]= 99,  [85]=100,  [86]=101,  [87]=102,  [88]=103,  [89]=104,                        [90]=107,             [91]=109,             [92]=111,  [93]=112,  [94]=113,  [95]= 114,
   [96]=115,  [97]=116,  [98]=117,  [99]=118,                                  [100]=122,                                  [101]=126, [102]=127, [103]=128,            [104]=130, [105]=131, [106]=132, [107]=133, [108]=134,            [109]=136,
}

-- Although Lua style prefers one based arrays, in this example I use zero based
-- Arrays since the code is translated from C.
local fire = {}
local color_table = {}

local function init_color_table()
  for i = 0, 31 do
    color_table[i] = { red = 0, green = 0, blue = i * 2 }
    color_table[i +  32] = { red = i * 8, green = 0, blue = 64 - (i * 2) }
    color_table[i +  64] = { red = 255, green = i * 8, blue = 0 }
    color_table[i +  96] = { red = 255, green = 255, blue = i * 4 }
    color_table[i + 128] = { red = 255, green = 255, blue =  64 + (i * 4) }
    color_table[i + 160] = { red = 255, green = 255, blue = 128 + (i * 4) }
    color_table[i + 192] = { red = 255, green = 255, blue = 192 + i }
    color_table[i + 224] = { red = 255, green = 255, blue = 224 + i }
  end
end

local function linear_position(row, col)
  return row * WIDTH + col
end

local function init_fire()
  for row = 0, HEIGHT - 1 do
    for col = 0, WIDTH - 1 do
      fire[linear_position(row, col)] = 0
    end
  end
end

local function calc_fire()
  for col = 0, WIDTH - 1 do
    if math.random() * 16 + 1 > 9 then
      fire[linear_position(HEIGHT - 1, col)] = 255
    else
      fire[linear_position(HEIGHT - 1, col)] = 0
    end
  end

  for row = HEIGHT - 2, 0, -1 do
    for col = 0, WIDTH - 1 do
      local temp
      if col == 0 then -- left border
        temp = fire[linear_position(row, col)]
        temp = temp + fire[linear_position(row + 1, col)]
        temp = temp + fire[linear_position(row + 1, col + 1)]
        temp = temp / 3
      elseif col == WIDTH - 1 then -- right border
        temp = fire[linear_position(row, col)]
        temp = temp + fire[linear_position(row + 1, col - 1)]
        temp = temp + fire[linear_position(row + 1, col)]
        temp = temp / 3
      else
        temp = fire[linear_position(row, col)]
        temp = temp + fire[linear_position(row + 1, col - 1)]
        temp = temp + fire[linear_position(row + 1, col)]
        temp = temp + fire[linear_position(row + 1, col + 1)]
        temp = temp / 4
      end

      if temp >= DECAY then
        temp = temp - DECAY
      end

      fire[linear_position(row, col)] = math.floor(temp)
    end
  end
end

-- init

init_color_table()
init_fire()

if ryos.supports_color() then
  ryos.set_all_states(true)
end

ryos.activate()

-- event loop

while true do

  if ryos.wait_for_allowance() then
    break
  end

  calc_fire()

  for sdk, position in pairs(sdk_to_linear_position) do
    local color_index = fire[position]
    if ryos.supports_color() then
      ryos.set_key_color(sdk, color_table[color_index])
    else
    -- Devices that do not support colors have the leds activated on flame
    -- intensity. Doesn't look spectacular.
      ryos.set_key_state(sdk, color_index > 32)
    end
  end

  ryos.send()

  if ryos.sleep(SLOWDOWN) then
    break
  end

end

-- cleanup

ryos.deactivate()
