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

-- config

-- Color capable devices use this color for the wave.
local COLOR = { red = 0xff, green = 0x00, blue = 0x00 }

-- Color capable devices can fade the base color dependent on distance to the center.
-- The bigger this value, the smaller the fade effect.
local FADE = 25

-- The waiting time in microseconds between single steps determines the speed of the wave.
local SLOWDOWN = 20000

-- helpers

-- For the wave to be as circular as possible in respect to the very small resolution
-- of the keyboard I build a larger matrix with accurate led positions and the leds
-- consisting of a circular form.

local MATRIX_ROWS = 25
local MATRIX_COLS = 95
local KEY_WIDTH = 4

local light_position_matrix = {}

-- The sdk values indicate Ryos keys as documented in Roccats Talk FX SDK version 2.1.1

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

local function init_light_position_matrix()
  for sdk, position in pairs(sdk_to_light_position) do
    local base = position.x - 2 + (position.y - 2) * MATRIX_COLS
    
    light_position_matrix[base + 1] = sdk
    light_position_matrix[base + 2] = sdk

    base = base + MATRIX_COLS

    light_position_matrix[base    ] = sdk
    light_position_matrix[base + 1] = sdk
    light_position_matrix[base + 2] = sdk
    light_position_matrix[base + 3] = sdk

    base = base + MATRIX_COLS

    light_position_matrix[base    ] = sdk
    light_position_matrix[base + 1] = sdk
    light_position_matrix[base + 2] = sdk
    light_position_matrix[base + 3] = sdk

    base = base + MATRIX_COLS

    light_position_matrix[base + 1] = sdk
    light_position_matrix[base + 2] = sdk
  end
end

local center
local radius
local last_leds
local leds
local running

local function init_variables(center_arg)
  center = center_arg
  radius = 1
  last_leds = nil
  leds = {}
  running = false
end

-- Sets corresponding key in array if any.
local function set_led(leds, x, y)
  if y >= MATRIX_ROWS or y < 0 or x >= MATRIX_COLS or x < 0 then
    return
  end

  sdk = light_position_matrix[x + y * MATRIX_COLS]
  if sdk then
    table.insert(leds, sdk)
  end
end

-- QUOTE Midpoint circle algorithm inspired by Perone's programming pad
-- http://www.willperone.net/Code/codecircle.php
-- Copyright (c) Will Perone
--
-- Returns set of sdk key index to set/clear.
local function draw_circle(center, radius)
  local radius = radius * KEY_WIDTH
  local leds = {}

  if radius < 1 then
    return leds
  end

  local x = 0
  local y = radius
  local d = 1 - radius
  local delta_e = 3
  local delta_se = 5 - radius * 2

  set_led(leds, center.x,     center.y - y)
  set_led(leds, center.x,     center.y + y)
  set_led(leds, center.x - y, center.y    )
  set_led(leds, center.x + y, center.y    )

  while y > x do
    if d < 0 then
      d = d + delta_e
      delta_se = delta_se + 2
    else
      d = d + delta_se
      delta_se = delta_se + 4
      y = y - 1
    end
    delta_e = delta_e + 2
    x = x + 1

    set_led(leds, center.x - x, center.y - y)
    set_led(leds, center.x - y, center.y - x)
    set_led(leds, center.x + y, center.y - x)
    set_led(leds, center.x + x, center.y - y)
    set_led(leds, center.x - x, center.y + y)
    set_led(leds, center.x - y, center.y + x)
    set_led(leds, center.x + y, center.y + x)
    set_led(leds, center.x + x, center.y + y)
  end

  return leds
end

-- init

init_light_position_matrix()
init_variables({ x = 0, y = 0 })

-- event loop

while true do

  if not running then
    -- No actual wave running - wait indefinitely for a event.
    event, sdk, pressed = ryos.get_event()
  else
    -- Continue wave after waiting time if no event occured.
    event, sdk, pressed = ryos.get_event_timed(SLOWDOWN)
  end

  -- Not testing event to be nil here, as ryos.get_event_timed() also returns nil on
  -- timeout.
  if ryos.should_stop() then
    break
  end

  -- If a matching event occured we start/restart effect with this key.
  if (event == ryos.EVENT_KEY) and pressed then
    init_variables(sdk_to_light_position[sdk])
    ryos.set_all_states(false)
    running = true
    ryos.activate()
  end

  if running then

    -- The waves run in the following pattern:
  -- *
  -- **
  --  *
  --  **
    if last_leds then
    -- Clear keys of second to last iteration.
      ryos.set_key_state(last_leds, false)
      last_leds = nil
    else
      last_leds = leds
      leds = draw_circle(center, radius)

      if #last_leds == 0 and #leds == 0 then
        -- Wave left the boundaries and we have nothing more to do.
        running = false
        ryos.deactivate()
      else
        -- Set keys of actual iteration.
        ryos.set_key_state(leds, true)

        if ryos.supports_color() then
          ryos.set_key_color(leds, {
              red = math.floor(COLOR.red * (FADE - radius) / FADE),
              green = math.floor(COLOR.green * (FADE - radius) / FADE),
              blue = math.floor(COLOR.blue * (FADE - radius) / FADE)
          })
        end

        radius = radius + 1
      end
    end

    ryos.send()

  end
end

--cleanup
