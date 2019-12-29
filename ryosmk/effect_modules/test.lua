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

-- helpers

local WASD = { 41, 62, 63, 64 }

local BLUE = {red = 0x00, green = 0x00, blue = 0xff}

-- init

ryos.set_all_states(false)
if ryos.supports_color() then
  ryos.set_all_colors(BLUE)
end

ryos.activate()
ryos.send()


-- event loop

while true do
  event, sdk, pressed = ryos.get_event()

  -- The unlimited event getter returns nil on stop condition. The call to
  -- ryos.should_stop() is only for illustration. In either case end event loop.
  if ryos.should_stop() then
    break
  end

  if event == ryos.EVENT_KEY then
    if pressed then
      state, color = ryos.get_key(sdk)
      ryos.set_key_state(sdk, not state);

      if (sdk == 16) then
        ryos.set_key_state(WASD, not state);
      end

      ryos.send()
    end
  end

end

-- cleanup

ryos.deactivate()
