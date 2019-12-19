# -*- coding: utf-8 -*-
"""
Created on Wed May 29 11:50:40 2019

@author: Yannic
"""

# -*- coding: utf-8 -*-
"""
Created on Mon May  6 18:01:31 2019

@author: Yannic

druckfeld
X 220mm start 10mm
Y 250mm start 20mm nach 250mm werden schritte verloren
Z 40mm ca


"""

import tkinter as tk
from tkinter import filedialog
from tkinter import scrolledtext
from threading import Thread
import serial
import re
import numpy as np
import struct
import time
#import math
#import os

connectet = False # is true when connectet to an arduino (ends the recive thread)
abbruch = False # if ture ends the send of points to cnc

once_test = False

#manuelle bedienung
def Xpos1(event):
    global direction
    #log_txt("send: X+")
    direction = "X+"
    mov = Thread(target=move_butten)
    mov.start()
    
def Xneg1(event):
    global direction
    #log_txt("send: X-")
    direction = "X-"
    mov = Thread(target=move_butten)
    mov.start()
    
def Ypos1(event):
    global direction
    #log_txt("send: Y+")
    direction = "Y+"
    mov = Thread(target=move_butten)
    mov.start()
    
def Yneg1(event):
    global direction
    #log_txt("send: Y-")
    direction = "Y-"
    mov = Thread(target=move_butten)
    mov.start()

def Zpos1(event):
    global direction
    #log_txt("send: Z+")
    direction = "Z+"
    mov = Thread(target=move_butten)
    mov.start()
    
def Zneg1(event):
    global direction
    #log_txt("send: Z-")
    direction = "Z-"
    mov = Thread(target=move_butten)
    mov.start()
    
def Wpos1(event):
    global direction
    #log_txt("send: W+")
    direction = "W+"
    mov = Thread(target=move_butten)
    mov.start()
    
def Wneg1(event):
    global direction
    #log_txt("send: W-")
    direction = "W-"
    mov = Thread(target=move_butten)
    mov.start()
    
def move_butten():
    global direction
    step = 10000
    if "X+" in direction:
        move2(float(x_text.get()[4:])+step,float(y_text.get()[4:]),float(z_text.get()[4:]),float(w_text.get()[4:]))
    if "X-" in direction:
        move2(float(x_text.get()[4:])-step,float(y_text.get()[4:]),float(z_text.get()[4:]),float(w_text.get()[4:]))
    if "Y+" in direction:
        move2(float(x_text.get()[4:]),float(y_text.get()[4:])+step,float(z_text.get()[4:]),float(w_text.get()[4:]))
    if "Y-" in direction:    
        move2(float(x_text.get()[4:]),float(y_text.get()[4:])-step,float(z_text.get()[4:]),float(w_text.get()[4:]))
    if "Z+" in direction:
        move2(float(x_text.get()[4:]),float(y_text.get()[4:]),float(z_text.get()[4:])+step,float(w_text.get()[4:]))
    if "Z-" in direction:
        move2(float(x_text.get()[4:]),float(y_text.get()[4:]),float(z_text.get()[4:])-step,float(w_text.get()[4:]))
    if "W+" in direction:
        move2(float(x_text.get()[4:]),float(y_text.get()[4:]),float(z_text.get()[4:]),float(w_text.get()[4:])+step)
    if "W-" in direction:
        move2(float(x_text.get()[4:]),float(y_text.get()[4:]),float(z_text.get()[4:]),float(w_text.get()[4:])-step)
      
def stop_move(event):
    global direction
    direction = "stop"
    #log_txt("send: release button")
    halt()

#read the position out of the entrys and move the cnc to it
def moveto():
    B_moveto.configure(state='disabled')
    mov = Thread(target=moveing)
    mov.start()
    
def moveing():
    try:
        X = E_movetoX.get()   
        Y = E_movetoY.get()
        Z = E_movetoZ.get()
        W = E_movetoW.get()
        #log_txt("move to: X="+str(X)+" Y="+str(Y)+" Z="+str(Z)+" W="+str(W))
        move(X,Y,Z,W)
    except Exception as e:
        log_txt("fail to move ("+str(e)+")")
    B_moveto.configure(state=tk.NORMAL)

# move the cnc to the endswitches
def movehome():
    log_txt("movehome")
    t_movehome = Thread(target=tmovehome)
    t_movehome.start()
    
def tmovehome():
    getpose()
    time.sleep(0.1)
    X = float(x_text.get()[4:])
    Y = float(y_text.get()[4:])
    W = float(w_text.get()[4:])
    move(X,Y,-1000,W)
    move(1000,1000,-1000,W)


def setpos():
    global ser
    try:
        X = E_setposX.get()
        Y = E_setposY.get()
        Z = E_setposZ.get()
        W = E_setposW.get()
        log_txt("send: set pos: X="+str(X)+" Y="+str(Y)+" Z="+str(Z)+" W="+str(W))
        setposi(X,Y,Z,W)
        #wait_for_rec()
    except Exception as e:
        log_txt("fail to set pos ("+str(e)+")")
        
def setparams():    
    s = float(E_txtspeed.get())
    t = float(E_txttemp.get())
    f = float(E_filament.get())
    setparam(s,f,t)
#manuele bedienung ende


#automatic mode
  
#select an file to execute 
def choosefile():
    global E_choosefile
    global file
    file = filedialog.askopenfilename()
    log_txt("Choosen File: "+file)
    E_choosefile.insert(tk.END, file)
    B_start.configure(state=tk.NORMAL) 
    #B_loadFile.configure(state=tk.NORMAL)

# Check if the choosen file exists and if its fine to print
def check():
    global file
    abord()
    log_txt("check file")
    auto = Thread(target=checkFile)
    auto.start()
    B_start.configure(state=tk.NORMAL)    
    
def checkFile():
    pass


# encode the g-code and send it line by line to the printer
def send_to_CNC():
    global file
    global abbruch
    f = open(file,"r")
    oldF = 0
    X = 0
    Y = 0
    Z = 0
    E = 0
    F = 0
    S = 0
    while abbruch is False:
        knowComand = False
        text = f.readline()
        if text == "":
            log_txt("end file")
            break
        if re.search("^; ",text) is not None:
            continue
        if re.search("X-?\d+.?\d*",text) is not None:# x position
            res = re.search("X-?\d+.?\d*",text)
            X = float(res.group(0)[1:])
            knowComand = True
        if re.search("Y-?\d+.?\d*",text) is not None:# y position
            res = re.search("Y-?\d+.?\d*",text)
            Y = float(res.group(0)[1:])
            knowComand = True
        if re.search("Z-?\d+.?\d*",text) is not None:# z position
            res = re.search("Z-?\d+.?\d*",text)
            Z = float(res.group(0)[1:])
            knowComand = True
        if re.search("E-?\d+.?\d*",text) is not None:# e extruder position
            res = re.search("E-?\d+.?\d*",text)
            E = float(res.group(0)[1:])
            knowComand = True
        if re.search("F-?\d+.?\d*",text) is not None:# feedrate
            res = re.search("F-?\d+.?\d*",text)
            F = float(res.group(0)[1:])
            knowComand = True
        if re.search("S-?\d+.?\d*",text) is not None:# spindel speed (with m104 temp of hotend)
            res = re.search("S-?\d+.?\d*",text)
            S = float(res.group(0)[1:])
            knowComand = True
            
        if re.search("G0",text) is not None:# fast move
            F = 40#Fmax
            log_txt("G0 X"+str(X)+" Y"+str(Y)+" Z"+str(Z)+" E"+str(E)+" F"+str(F))
            setparam(s=F)
            move(X,Y,Z,E)
            knowComand = True
        if re.search("G1",text) is not None:# move with feedrate
            log_txt("G1 X"+str(X)+" Y"+str(Y)+" Z"+str(Z)+" E"+str(E)+" F"+str(F))
            if oldF is not F:
                F = F/120
                setparam(s=F)
            move(X,Y,Z,E)
            knowComand = True
        if re.search("G21",text) is not None:# use mm
            log_txt("G21")
            knowComand = True
        if re.search("G28",text) is not None:# go home
            log_txt("G28")
            getpose()
            time.sleep(0.1)
            X = float(x_text.get()[4:])
            Y = float(y_text.get()[4:])
            W = float(w_text.get()[4:])
            move(X,Y,-1000,W)
            knowComand = True
        if re.search("G90",text) is not None:# use zero point as referenz
            log_txt("G90")
            knowComand = True
        if re.search("G92",text) is not None:# set pose
            log_txt("G92 X"+str(X)+" Y"+str(Y)+" Z"+str(Z)+" E"+str(E))
            setposi(X,Y,Z,E)
            knowComand = True
        if re.search("M104",text) is not None:#set temperatur to Sxxx
            log_txt("M104 S"+str(S))
            setparam(t=S,s=F)
            knowComand = True   
        #print("\n")
        if knowComand is False:
            pass
            log_txt("unknown comand ("+text[:-1]+")")
        oldF = F #for detektion Feedrate changings
    
def start():
    global file
    global abbruch
    log_txt("start execute")
    abbruch = False
    #setposi(0,0,0,0)                                 #set position to 0 0 0
    
    auto = Thread(target=send_to_CNC)
    auto.start()
    B_pause.configure(state=tk.NORMAL)
    B_abord.configure(state=tk.NORMAL)
    log_txt("end execute")
    
def abord():
    global abbruch
    log_txt("abord")
    abbruch = True
   
#Connect and disconnect    
def connect():
    global connectet
    global ser
    try: 
        com = E_connect.get()
        log_txt("Connect to "+com)
        ser = serial.Serial(com,parity=serial.PARITY_NONE,baudrate = 9600,stopbits =serial.STOPBITS_TWO)#)
        #ser = serial.Serial("COM3",parity=serial.PARITY_NONE,baudrate = 9600,stopbits =serial.STOPBITS_TWO)#)
 
        B_connect.configure(bg="green",command=Disconnect,text="Disconnect")
        B_setspeed.configure(state=tk.NORMAL)
        B_moveto.configure(state=tk.NORMAL)
        B_setpos.configure(state=tk.NORMAL)
        B_getPose.configure(state=tk.NORMAL)
        B_wpos1.configure(state=tk.NORMAL)
        B_xpos1.configure(state=tk.NORMAL)
        B_ypos1.configure(state=tk.NORMAL)
        B_zpos1.configure(state=tk.NORMAL)
        B_wneg1.configure(state=tk.NORMAL)
        B_xneg1.configure(state=tk.NORMAL)
        B_yneg1.configure(state=tk.NORMAL)
        B_zneg1.configure(state=tk.NORMAL)  
        B_getparams.configure(state=tk.NORMAL)
        B_choosefile.configure(state=tk.NORMAL)
        B_movehome.configure(state=tk.NORMAL)

        B_loadFile.configure(state=tk.NORMAL)
        #B_start.configure(state=tk.NORMAL)
        B_pause.configure(state=tk.NORMAL)
        #B_contin.configure(state=tk.NORMAL)
        #B_abord.configure(state=tk.NORMAL)
        B_setPID.configure(state=tk.NORMAL)
        
        
        abord()
        connectet = True
        rec = Thread(target=recive)
        rec.start() 
        
    except Exception as e:
        log_txt("fail to Connect ("+str(e)+")")
        B_connect.configure(bg="yellow")
        connectet = False
        

def Disconnect():
    global ser
    global connectet
    log_txt("Disconnect")
    try:
        ser.close()
        B_connect.configure(bg="yellow",command=connect,text="Connect")
        B_setspeed.configure(state='disabled')
        B_moveto.configure(state='disabled')
        B_setpos.configure(state='disabled')
        B_getPose.configure(state='disabled')
        B_wpos1.configure(state='disabled')
        B_xpos1.configure(state='disabled')
        B_ypos1.configure(state='disabled')
        B_zpos1.configure(state='disabled')
        B_wneg1.configure(state='disabled')
        B_xneg1.configure(state='disabled')
        B_yneg1.configure(state='disabled')
        B_zneg1.configure(state='disabled')
        B_getparams.configure(state='disabled')
        B_movehome.configure(state='disabled')
        
        B_choosefile.configure(state='disabled')
        B_loadFile.configure(state='disabled')
        B_start.configure(state='disabled')
        B_pause.configure(state='disabled')
        B_contin.configure(state='disabled')
        B_abord.configure(state='disabled')
        B_setPID.configure(state='disabled')
        connectet = False
    except Exception as e:
        log_txt("fail to Disconnect ("+str(e)+")")
        B_connect.configure(bg="yellow")
        connectet = False


### Basic Funktions to send    
def move(X,Y,Z,W):
    global ser
    arr = np.array([X,Y,Z,W], dtype=np.float32).tobytes()
    new_msg = 'm'.encode("utf-8") + arr
    send_command(new_msg)
    wait_for_rec()  
    
def move2(X,Y,Z,W):
    global ser
    arr = np.array([X,Y,Z,W], dtype=np.float32).tobytes()
    new_msg = 'm'.encode("utf-8") + arr
    send_command(new_msg) 
    
def setposi(x,y,z,w):
    global ser
    try:
        arr = np.array([x,y,z,w], dtype=np.float32).tobytes()
        new_msg = 'n'.encode("utf-8") + arr
        send_command(new_msg)
    except Exception as e:
        log_txt("fail to set pos ("+str(e)+")")

def setparam(s=None,f=None,t=None):
    global ser
    if s is None:
        s=float(L_speedtext.get())
    if t is None: 
        t=float(E_txttemp.get())
    if f is None:
        f=float(E_filament.get())
        
        
    if s>70:s=70
    if s<0.01:s=0.01
    
    try:
        E_txttemp.delete(0,tk.END)
        E_txttemp.insert(0,t)
        log_txt("send: speed ="+str(s)+" temperatur ="+str(t)+" filament ="+str(f))
        
        arr = np.array([s,0,0,0], dtype=np.float32).tobytes()
        new_msg = 's'.encode("utf-8") + arr
        send_command(new_msg)
        
        arr = np.array([t,0,0,0], dtype=np.float32).tobytes()
        new_msg = 't'.encode("utf-8") + arr
        send_command(new_msg)
        
        arr = np.array([f,0,0,0], dtype=np.float32).tobytes()
        new_msg = 'f'.encode("utf-8") + arr
        send_command(new_msg)
    except Exception as e:
        log_txt("fail to set speed ("+str(e)+")") 
    
def getpose():
    global ser
    arr = np.array([0,0,0,0], dtype=np.float32).tobytes()
    new_msg = 'p'.encode("utf-8") + arr
    send_command(new_msg)   
    
def getparams():
    global ser
    arr = np.array([0,0,0,0], dtype=np.float32).tobytes()
    new_msg = 'c'.encode("utf-8") + arr
    send_command(new_msg)

def halt():
    global ser
    arr = np.array([0,0,0,0], dtype=np.float32).tobytes()
    new_msg = 'b'.encode("utf-8") + arr
    send_command(new_msg) 

def pause():
    global ser
    log_txt("send: pause")
    arr = np.array([0,0,0,0], dtype=np.float32).tobytes()
    new_msg = 'h'.encode("utf-8") + arr
    send_command(new_msg)
    B_pause.configure(state='disabled')
    B_abord.configure(state=tk.NORMAL)
    B_contin.configure(state=tk.NORMAL)

def contin():
    global ser
    log_txt("send: contin")
    arr = np.array([0,0,0,0], dtype=np.float32).tobytes()
    new_msg = 'w'.encode("utf-8") + arr
    send_command(new_msg) 
    B_pause.configure(state=tk.NORMAL)
    B_abord.configure(state=tk.NORMAL)
    B_contin.configure(state='disabled')
    
def setPID():
    global ser
    p = float(E_P.get())
    i = float(E_I.get())
    d = float(E_D.get())
    m = float(E_A.get())
    log_txt("send: set PID parameter P:"+str(p)+" I:"+str(i)+" D:"+str(d)+" M:"+str(m))
    arr = np.array([p,i,d,m], dtype=np.float32).tobytes()
    new_msg = 'r'.encode("utf-8") + arr
    send_command(new_msg)
    
def repeat_request():
    arr = np.array([0,0,0,0], dtype=np.float32).tobytes()
    new_msg = 'N'.encode("utf-8") + arr
    send_command(new_msg)
    log_txt("send repeat request")

def wait_for_rec():
    global readytosend
    readytosend = False
    while readytosend is not True:
        time.sleep(0.01)
        
        
        
def send_command(msg):
    global last_msg
    check_byte = callc_checkbyte_send(msg)
    msg = 'S'.encode("utf-8") + msg + check_byte
    ser.write(msg)
    #log_txt(str(msg)+" "+str(len(msg)))
    last_msg = msg
  
def callc_checkbyte_send(msg):
    check_byte = ord('S')
    for i in range(0,17):
        check_byte = check_byte + msg[i]
        if check_byte > 255:
            check_byte = check_byte - 256
    return check_byte.to_bytes(1,byteorder="big")
    
### Recive Funktion
    
def get_msg():
    global ser
    msg = []
    while len(msg)<18:
        x = ser.read()
        msg.append(x)
    #log_txt(str(msg)+" "+str(len(msg)))
    check_byte = callc_checkbyte_rec(msg)
        
    if check_byte == msg[17]:
        #log_txt("check success "+str(check_byte)+" "+str(msg[17]))
        return msg
    else:
        log_txt("check fail "+str(check_byte)+" "+str(msg[17]))
        repeat_request()
        return False
        #return msg
    

def callc_checkbyte_rec(msg):
    check_byte = ord('S')
    for i in range(0,17):
        #check_byte = check_byte + msg[i]
        check_byte = check_byte + int.from_bytes(msg[i], "big")
        if check_byte > 255:
            check_byte = check_byte - 256
    return (check_byte).to_bytes(1,byteorder="big")
  
        
def recive():
    global ser
    global readytosend
    global connectet
    global last_msg
    
    while connectet:
        try:
            msg = []
            x = 'x'
            while x != 'S'.encode("utf-8"):
                x = ser.read()
            #log_txt("read: "+str(x)+" equal: "+str('S'.encode("utf-8")))
            msg = get_msg()
            if msg == False:
                continue
            command = msg[0:1][0].decode("utf-8")
            #log_txt("rec: command "+str(command))
            value1 = struct.unpack('f', b''.join(msg[1:5]))[0]
            value2 = struct.unpack('f', b''.join(msg[5:9]))[0]
            value3 = struct.unpack('f', b''.join(msg[9:13]))[0]
            value4 = struct.unpack('f', b''.join(msg[13:17]))[0]
            
            if command == 'N':
                send_command(last_msg[:17])
                log_txt("ERROR in transmision, send last message again: ."+str(last_msg[:17]))
            elif command == 'y':
                log_txt("rec: step dif to zero X:"+str(round(value1,2))+" Y:"+str(round(value2,2))+" Z:"+str(round(value3,2))+" W:"+str(round(value4,2)))
            elif command == 'q':
                #log_txt("rec: settings debug:"+str(round(value1,2))+" T:"+str(round(value2,2))+" soll_T:"+str(round(value3,2))+" W:"+str(round(value4,2)))
                L_speedtext.set(str(round(value1,2)))
                L_temptext.set(str(round(value2,2)))
                L_filamenttext.set(str(round(value3,2)))
                L_PID_agr_text.set(str(round(value4,0)))
                try:
                    with open("heat_valus.txt", 'a') as f:
                        f.write("<debug>"+str(value1) + '</debug><T>' + str(value2) + '</T><soll_T>'+str(value3) + '</soll_T>\n')
                        f.close()
                except:
                    log_txt("fail to log values")
                #log_txt("rec: params speed:"+str(round(value1,2))+" fil:"+str(round(value2,2))+" temp:"+str(round(value3,2))+" W:"+str(round(value4,2)))

            elif command == 'm':
                x_text.set("X = "+str(round(value1,2)))
                y_text.set("Y = "+str(round(value2,2)))
                z_text.set("Z = "+str(round(value3,2)))
                w_text.set("W = "+str(round(value4,2)))
                #log_txt("rec: act pose X:"+str(round(value1,2))+" Y:"+str(round(value2,2))+" Z:"+str(round(value3,2))+" W:"+str(round(value4,2)))
                readytosend = True
            elif command == 'j':
                log_txt("rec: soll pose X:"+str(round(value1,2))+" Y:"+str(round(value2,2))+" Z:"+str(round(value3,2))+" W:"+str(round(value4,2)))
            elif command == 'k':
                log_txt("rec: soll step X:"+str(round(value1,2))+" Y:"+str(round(value2,2))+" Z:"+str(round(value3,2))+" W:"+str(round(value4,2)))
            elif command == 'w':
                log_txt("rec: time p step X:"+str(round(value1,2))+" Y:"+str(round(value2,2))+" Z:"+str(round(value3,2))+" W:"+str(round(value4,2)))
            elif command == 'b':
                log_txt("rec: motorstep X:"+str(round(value1,2))+" Y:"+str(round(value2,2))+" Z:"+str(round(value3,2))+" W:"+str(round(value4,2)))
            elif command == 'e':
                if value1 < 0.5:
                    L_lblX.config(bg="green")
                else:
                    L_lblX.config(bg="red")
                if value2 < 0.5:
                    L_lblY.config(bg="green")
                else:
                    L_lblY.config(bg="red")
                if value3 < 0.5:
                    L_lblZ.config(bg="green")
                else:
                    L_lblZ.config(bg="red")
                #log_txt("endswitch X:"+str(round(value1,2))+" Y:"+str(round(value2,2))+" Z:"+str(round(value3,2))+" W:"+str(round(value4,2)))
            else:
                log_txt("rec: find no command ("+command+")")
                pass
        except Exception as e:
            try:
                pass
                log_txt("rec: fehler: "+str(e))
                log_txt("rec: fail to read command "+str(command))
            except Exception as e:
                pass
                log_txt("rec: fail to read command and cant decodey massage "+str(e))
                

#begind der main loop

root = tk.Tk()
root.title("Hildegart V3.1")

manuelframe = tk.Frame(root)
manuelframe.grid( column=0, row=1 )
autoframe = tk.Frame(root)
autoframe.grid( column=1, row=0 ,rowspan=2 )
steuerung = tk.Frame(root)
steuerung.grid(column=0, row=0 )
           

B_wpos1 = tk.Button(steuerung,state='disabled', text="W+1")
B_wpos1.grid(column=0, row=1)
B_wpos1.bind('<ButtonPress>',Wpos1)
B_wpos1.bind('<ButtonRelease>',stop_move)
B_wneg1 = tk.Button(steuerung,state='disabled', text="W-1")
B_wneg1.grid(column=0, row=3)
B_wneg1.bind('<ButtonPress>',Wneg1)
B_wneg1.bind('<ButtonRelease>',stop_move)
B_xpos1 = tk.Button(steuerung,state='disabled', text="X+1")
B_xpos1.grid(column=2, row=1)
B_xpos1.bind('<ButtonPress>',Xpos1)
B_xpos1.bind('<ButtonRelease>',stop_move)
B_xneg1 = tk.Button(steuerung,state='disabled', text="X-1")
B_xneg1.grid(column=2, row=3)
B_xneg1.bind('<ButtonPress>',Xneg1)
B_xneg1.bind('<ButtonRelease>',stop_move)
B_ypos1 = tk.Button(steuerung,state='disabled', text="Y+1")
B_ypos1.grid(column=1, row=2)
B_ypos1.bind('<ButtonPress>',Ypos1)
B_ypos1.bind('<ButtonRelease>',stop_move)
B_yneg1 = tk.Button(steuerung,state='disabled', text="Y-1")
B_yneg1.grid(column=3, row=2)
B_yneg1 .bind('<ButtonPress>',Yneg1)
B_yneg1 .bind('<ButtonRelease>',stop_move)
B_zpos1 = tk.Button(steuerung,state='disabled', text="Z+1")
B_zpos1.grid(column=5, row=1)
B_zpos1.bind('<ButtonPress>',Zpos1)
B_zpos1.bind('<ButtonRelease>',stop_move)
B_zneg1 = tk.Button(steuerung,state='disabled', text="Z-1")
B_zneg1.grid(column=5, row=3)
B_zneg1.bind('<ButtonPress>',Zneg1)
B_zneg1.bind('<ButtonRelease>',stop_move)


B_movehome = tk.Button(steuerung,state='disabled', text="move home", command=movehome)
B_movehome.grid(column=8, row=1,pady=2)
B_getPose = tk.Button(steuerung,state='disabled', text="get Pose", command=getpose)
B_getPose.grid(column=8, row=2,pady=2)
x_text = tk.StringVar()
L_lblX = tk.Label(steuerung,width=10,textvariable=x_text)
L_lblX.grid(column=6, row=1)
x_text.set("X = 0")
y_text = tk.StringVar()
L_lblY = tk.Label(steuerung,width=10,textvariable=y_text)
L_lblY.grid(column=6, row=2)
y_text.set("Y = 0")
z_text = tk.StringVar()
L_lblZ = tk.Label(steuerung,width=10,textvariable=z_text)
L_lblZ.grid(column=6, row=3)
z_text.set("Z = 0")
w_text = tk.StringVar()
L_lblW = tk.Label(steuerung,width=10,textvariable=w_text)
L_lblW.grid(column=6, row=4)
w_text.set("W = 0")

B_setpos = tk.Button(manuelframe,state='disabled', text="set pos", command=setpos,width=8)
B_setpos.grid(column=0, row=0,pady=2)
L_setposlabelX = tk.Label(manuelframe, text="X =",width=6)
L_setposlabelX.grid(column=1, row=0)
E_setposX = tk.Entry(manuelframe,width=6)
E_setposX.grid(column=2, row=0)
E_setposX.insert(tk.END, "0.0")
L_setposlabelY = tk.Label(manuelframe, text="Y =",width=6)
L_setposlabelY.grid(column=3, row=0)
E_setposY = tk.Entry(manuelframe,width=6)
E_setposY.grid(column=4, row=0)
E_setposY.insert(tk.END, "0.0")
L_setposlabelZ = tk.Label(manuelframe, text="Z =",width=6)
L_setposlabelZ.grid(column=5, row=0)
E_setposZ = tk.Entry(manuelframe,width=6)
E_setposZ.grid(column=6, row=0)
E_setposZ.insert(tk.END, "0.0")
L_setposlabelW = tk.Label(manuelframe, text="W =",width=6)
L_setposlabelW.grid(column=7, row=0)
E_setposW = tk.Entry(manuelframe,width=6)
E_setposW.grid(column=8, row=0)
E_setposW.insert(tk.END, "0.0")


B_moveto = tk.Button(manuelframe,state='disabled', text="move to", command=moveto,width=8)
B_moveto.grid(column=0, row=1,pady=2)
L_movetolabelX = tk.Label(manuelframe, text="X =",width=6)
L_movetolabelX.grid(column=1, row=1)
E_movetoX = tk.Entry(manuelframe,width=6)
E_movetoX.grid(column=2, row=1)
E_movetoX.insert(tk.END, "0.0")
L_movetolabelY = tk.Label(manuelframe, text="Y =",width=6)
L_movetolabelY.grid(column=3, row=1)
E_movetoY = tk.Entry(manuelframe,width=6)
E_movetoY.grid(column=4, row=1)
E_movetoY.insert(tk.END, "0.0")
L_movetolabelZ = tk.Label(manuelframe, text="Z =",width=6)
L_movetolabelZ.grid(column=5, row=1)
E_movetoZ = tk.Entry(manuelframe,width=6)
E_movetoZ.grid(column=6, row=1)
E_movetoZ.insert(tk.END, "0.0")
L_movetolabelW = tk.Label(manuelframe, text="W =",width=6)
L_movetolabelW.grid(column=7, row=1)
E_movetoW = tk.Entry(manuelframe,width=6)
E_movetoW.grid(column=8, row=1)
E_movetoW.insert(tk.END, "0.0")
    

B_setspeed = tk.Button(manuelframe,state='disabled', text="set param", command=setparams,width=8)
B_setspeed.grid(column=0, row=3,pady=2)
B_getparams = tk.Button(manuelframe,state='disabled', text="get param", command=getparams,width=8)
B_getparams.grid(column=0, row=4,pady=2)

L_speed = tk.Label(manuelframe, text="mm/min",width=6)
L_speed.grid(column=2, row=2)
L_speed = tk.Label(manuelframe, text="%",width=6)
L_speed.grid(column=1, row=3)
E_txtspeed = tk.Entry(manuelframe,width=6)
E_txtspeed.grid(column=2, row=3)
E_txtspeed.insert(tk.END, "5")

L_temp = tk.Label(manuelframe, text="C° =",width=6)
L_temp.grid(column=4, row=2)
L_temp = tk.Label(manuelframe, text="soll",width=6)
L_temp.grid(column=3, row=3)
E_txttemp = tk.Entry(manuelframe,width=6)
E_txttemp.grid(column=4, row=3)
E_txttemp.insert(tk.END, "0")

L_filament = tk.Label(manuelframe, text="tick/mm",width=6)
L_filament.grid(column=6, row=2)
E_filament = tk.Entry(manuelframe,width=6)
E_filament.grid(column=6, row=3)
E_filament.insert(tk.END, "37")

#L_actparam = tk.Label(manuelframe, text="param",width=6)
#L_actparam.grid(column=0, row=4)

L_speedtext = tk.StringVar()
L_speedact = tk.Label(manuelframe, textvariable=L_speedtext,width=6)
L_speedact.grid(column=2, row=4)

L_temptext = tk.StringVar()
L_tempact = tk.Label(manuelframe, textvariable=L_temptext,width=6)
L_tempact.grid(column=4, row=4)

L_filamenttext = tk.StringVar()
L_filamentact = tk.Label(manuelframe, textvariable=L_filamenttext,width=6)
L_filamentact.grid(column=6, row=4)


L_PID = tk.Label(manuelframe, text="PID agr" ,width=6)
L_PID.grid(column=8, row=4)
L_PID_agr_text = tk.StringVar()
L_PID_agr = tk.Label(manuelframe, textvariable=L_PID_agr_text,width=6)
L_PID_agr.grid(column=8, row=4)

    
B_connect = tk.Button(manuelframe, text="Connect", command=connect,width=8,bg="yellow")
B_connect.grid(column=0, row=5,padx=2,pady=2)
L_connect = tk.Label(manuelframe, text="COM",width=6)
L_connect.grid(column=1, row=5)
E_connect = tk.Entry(manuelframe,width=6)
E_connect.grid(column=2, row=5)
E_connect.insert(tk.END, "COM3")

    
B_choosefile = tk.Button(manuelframe, text="choosefile",state='disabled', command=choosefile,width=8)
B_choosefile.grid(column=0, row=6,padx=2,pady=2)
E_choosefile = tk.Entry(manuelframe,width=40)
E_choosefile.grid(column=1, row=6 ,columnspan=10,padx=2)
        
B_loadFile = tk.Button(manuelframe, text="check",state='disabled', command=check,width=8)
B_loadFile.grid(column=0, row=7,padx=2,pady=2)

B_start = tk.Button(manuelframe, text="start",state='disabled', command=start,width=8)
B_start.grid(column=1, row=7,padx=2,pady=2)

B_pause = tk.Button(manuelframe, text="pause",state='disabled', command=pause,width=8)
B_pause.grid(column=2, row=7,padx=2,pady=2)

B_contin = tk.Button(manuelframe, text="contin",state='disabled', command=contin,width=8)
B_contin.grid(column=3, row=7,padx=2,pady=2)

B_abord = tk.Button(manuelframe, text="abord",state='disabled', command=abord,width=8)
B_abord.grid(column=4, row=7,padx=2,pady=2)


B_setPID = tk.Button(manuelframe,state='disabled', text="set PID", command=setPID,width=8)
B_setPID.grid(column=0, row=8,pady=2)

L_P = tk.Label(manuelframe, text="P",width=6)
L_P.grid(column=1, row=8)
E_P = tk.Entry(manuelframe,width=6)
E_P.grid(column=2, row=8)
E_P.insert(tk.END, "35")

L_I = tk.Label(manuelframe, text="I",width=6)
L_I.grid(column=3, row=8)
E_I = tk.Entry(manuelframe,width=6)
E_I.grid(column=4, row=8)
E_I.insert(tk.END, "0.2")

L_D = tk.Label(manuelframe, text="D",width=6)
L_D.grid(column=5, row=8)
E_D = tk.Entry(manuelframe,width=6)
E_D.grid(column=6, row=8)
E_D.insert(tk.END, "750")

L_A = tk.Label(manuelframe, text="Manuel",width=6)
L_A.grid(column=7, row=8)
E_A = tk.Entry(manuelframe,width=6)
E_A.grid(column=8, row=8)
E_A.insert(tk.END, "0")

####Logging
def log_txt(msg):
    log.insert(tk.INSERT,msg+"\n")
    log.see(tk.END)
 
log = scrolledtext.ScrolledText(autoframe,width=60,height=20)
log.grid(column=0,row=0)


connect()
#try:
    #os.remove("heat_valus")
#except:
    #pass
root.mainloop()