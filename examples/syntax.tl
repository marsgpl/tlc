local test_name <const> = "Roberto"
local test_age = 100

local function test(name: string, age: number): void
    print("name", name)
    print("age:", age)
end

test(test_name, test_age)
