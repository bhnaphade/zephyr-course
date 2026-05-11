# Custom Board
We create a new board my copying the files of an identical soc and name it "our_board" and also create new board from scratch and name it "my_board"

west build -p always -b our_board  l5-task -p always
west build -p always -b my_board  l5-task -p always