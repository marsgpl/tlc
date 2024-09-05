local _test_age <const> = 100

local a = {[3] = {{{{{{ 0x1.fp10 }}}}}}};

a[1] = 'alo\n123"'
a[2] = "alo\n123\"'"
a.aa = '\97lo\10\04923"'
a["asd"] = [[alo
123"]]
a[4] = [===[
alo
123"]===]
local b

b = 3
b = 345
b = 0xff
b = 0xBEBADA

--[[
local c

c = 3.0
c = 3.1416
c = 314.16e-2
c = 0.31416E1
c = 34e1
c = 0x0.1E
c = 0xA23p-4
c = 0X1.921FB54442D18P+1
--]]

local function test(name: string, age: number, ...): void
    print("name", name)
    print("age:", age, ...)
end

test("Roberto", _test_age + -10)
