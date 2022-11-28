----------------------------------------------------
-- Wireshark dissector for SC-OBC Quality Inspection
-- Copyright 2022 Space Cubics, LLC.
-- SPDX-License-Identifier: Apache-2.0
----------------------------------------------------

set_plugin_info({
        version   = "20221127",
        author    = "Yasushi SHOJI",
        email     = "yashi@spacecubics.com",
        copyright = "Copyright (c) 2022, Space Cubics, LLC.",
        license   = "Apache-2.0 license",
        details   = "Wireshark dissector for SC-OBC Quality Inspection",
})

protocol = Proto("STPC", "SC-OBC TRCH Port Control Protocol")

CAN_ID = "84,70"

CMD_TYPE = {
   ACK = 6,

   GET_PORTA = string.byte("a"),
   GET_PORTB = string.byte("b"),
   GET_PORTC = string.byte("c"),
   GET_PORTD = string.byte("d"),
   GET_PORTE = string.byte("e"),

   SET_PORTA = string.byte("A"),
   SET_PORTB = string.byte("B"),
   SET_PORTC = string.byte("C"),
   SET_PORTD = string.byte("D"),
   SET_PORTE = string.byte("E"),

   GET_TRISA = string.byte("t"),
   GET_TRISB = string.byte("u"),
   GET_TRISC = string.byte("v"),
   GET_TRISD = string.byte("w"),
   GET_TRISE = string.byte("x"),

   SET_TRISA = string.byte("T"),
   SET_TRISB = string.byte("U"),
   SET_TRISC = string.byte("V"),
   SET_TRISD = string.byte("W"),
   SET_TRISE = string.byte("X"),

   PORT_STATUS = string.byte("p"),
}

local f = protocol.fields

f.cmd = ProtoField.uint8("stpc.cmd", "Command", base.HEX_DEC, {
			    [CMD_TYPE.ACK]       = "Ack",

			    [CMD_TYPE.GET_PORTA] = "Get Port A",
			    [CMD_TYPE.GET_PORTB] = "Get Port B",
			    [CMD_TYPE.GET_PORTC] = "Get Port C",
			    [CMD_TYPE.GET_PORTD] = "Get Port D",
			    [CMD_TYPE.GET_PORTE] = "Get Port E",

			    [CMD_TYPE.SET_PORTA] = "Set Port A",
			    [CMD_TYPE.SET_PORTB] = "Set Port B",
			    [CMD_TYPE.SET_PORTC] = "Set Port C",
			    [CMD_TYPE.SET_PORTD] = "Set Port D",
			    [CMD_TYPE.SET_PORTE] = "Set Port E",

			    [CMD_TYPE.GET_TRISA] = "Get TRIS A",
			    [CMD_TYPE.GET_TRISB] = "Get TRIS B",
			    [CMD_TYPE.GET_TRISC] = "Get TRIS C",
			    [CMD_TYPE.GET_TRISD] = "Get TRIS D",
			    [CMD_TYPE.GET_TRISE] = "Get TRIS E",

			    [CMD_TYPE.SET_TRISA] = "Set TRIS A",
			    [CMD_TYPE.SET_TRISB] = "Set TRIS B",
			    [CMD_TYPE.SET_TRISC] = "Set TRIS C",
			    [CMD_TYPE.SET_TRISD] = "Set TRIS D",
			    [CMD_TYPE.SET_TRISE] = "Set TRIS E",
			    [CMD_TYPE.PORT_STATUS] = "Port Status",
})
f.arg = ProtoField.uint8("stpc.arg", "Argument", base.HEX_DEC)

function protocol.dissector(buf, pinfo, root, data)
    local tree = root:add(protocol, buf(), "STPC")
    local subtree
    local subsubtree
    local subsubsubtree
    local offset = 0

    tree:add(f.cmd, buf:range(0, 1))
    if buf:len() == 2 then
       tree:add(f.arg, buf:range(1, 1))
    end
end

can_table = DissectorTable.get("can.id")
can_table:add(CAN_ID, protocol)
