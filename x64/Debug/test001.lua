
window = 0

function button_test()
    StaticText(window, "クリックされたんだが", 10, 100)
    print("click")
end

function main()
    window = Frame("print from lua", -1, -1, 300,200)
    StaticText(window, "hogehoge", 10, 10)
    button = Button(window, "ぼたん", 10, 50, -1, -1)
    addClickEvent(button, "button_test")
end


