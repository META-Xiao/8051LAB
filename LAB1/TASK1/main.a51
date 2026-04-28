MOV R0, 40H ; MOV Rn Adress 
MOV R1, #50H; MOV Rn Value , Val is Adress
MOV A, @R1; pointer 
MOV C, 03H; C=20H.3
CPL C; C~=C
MOV P1.0, C; 

END