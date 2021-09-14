# -*- coding: utf-8 -*-
"""
Created on Thu Mar 24 17:01:31 2016

@author: honza
"""


from Tkinter import *
import random
import serial
from tkMessageBox import *


#NASTAVENÍ OKNA TKINTER

hlavni_okno = Tk()

hlavni_okno.title(u"Ovládání zařízení")

hlavni_okno.minsize(width = 400, height = 300)
hlavni_okno.maxsize(width = 400, height = 300)

com_var = StringVar()   #nazev portu, na kterém je připojeno UART
sound_var = StringVar() #zahraný tón



#FUNKCE

def TonyVypis():
    s = ser.read()
    if(len(s) == 0):
        sound_var.set("")
    else:
        m = ord(s)
        if(m == 0):
            sound_var.set("Zazněl tón: C1")
        elif(m == 1):
            sound_var.set("Zazněl tón: D1")
        elif(m == 2):
            sound_var.set("Zazněl tón: E1")
        elif(m == 3):
            sound_var.set("Zazněl tón: F1")
        elif(m == 4):
            sound_var.set("Zazněl tón: G1")
        elif(m == 5):
            sound_var.set("Zazněl tón: A1")
        elif(m == 6):
            sound_var.set("Zazněl tón: H1")
        elif(m == 7):
            sound_var.set("Zazněl tón: C2")
        
            
    hlavni_okno.after(1000, TonyVypis)


def OpenSer():
    global ser
    global nazev
    ser = serial.Serial(port='COM3', baudrate=9600, timeout=1, bytesize=8, parity='N', stopbits=1, xonxoff=0, rtscts=0)
    ser_open["state"] = "disabled"
    ser_close["state"] = "normal"
    nazev = ser.name
    com_var.set(u"Komunikace je otevřena na portu: %s" % (nazev))
    TonyVypis()
    
def CloseSer():
    global ser
    global nazev
    ser.close()
    ser_close["state"] = "disabled"
    ser_open["state"] = "normal"
    nazev = u"port není otevřen"
    com_var.set(u"Komunikace je otevřena na portu: %s" % (nazev))
    
    

def ZmenaHodin():
    hod_nas = hod.get()
    minutes_nas = minutes.get()
    sec_nas = sec.get()
    
    hod_nas = int(hod_nas)
    minutes_nas = int(minutes_nas)
    sec_nas = int(sec_nas)
    
    if((hod_nas >= 0 and hod_nas <= 23) and (minutes_nas >= 0 and minutes_nas <= 59) and (sec_nas >= 0 and sec_nas <= 59) ):
        zapis_time = "<%02i%02i%02i"  % (hod_nas, minutes_nas, sec_nas)
        ser.write(zapis_time)
    else:
        showerror(u"Změna hodin", u"Pokoušíte se provést nepovolené nastavení digitálních hodin \n (min čas: 00:00:00 max: 23:59:59)")

def ZmenaTextu():
    text_nas = text_entry.get()
    zapis_text = "{%s}" % (text_nas)
    if(len(zapis_text) > 17):
        showerror(u"Změna běžícího textu", u"Pokoušíte se provést nepovolené nastavení běžícího textu \n (maximální délka řetězce může být 15 znaků)")
    else:
        ser.write(zapis_text)
       
def WriteText():
    write_nas = text_write.get()
    ser.write(write_nas)

def DeleteText():
    global ser
    global s
    ser.write(">")      #Smazání textu nastavit v ATmega
    
    
    
#Znaky pro klávesy C1 - C2
def C1(udalost):
    ser.write(chr(0))

def D1(udalost):
    ser.write(chr(1))

def E1(udalost):
    ser.write(chr(2))
    
def F1(udalost):
    ser.write(chr(3))
    
def G1(udalost):
    ser.write(chr(4))
    
def A1(udalost):
    ser.write(chr(5))
    
def H1(udalost):
    ser.write(chr(6))
    
def C2(udalost):
    ser.write(chr(7))
    
    

#FRAME
serial_frame = Frame(hlavni_okno, padx = 10, pady = 10)
serial_frame.pack()

Frame(height=2, bd=1, relief=SUNKEN).pack(fill=X, padx=5, pady=5)

all_frame = Frame(hlavni_okno, padx = 10, pady = 10, bd = 2, relief = "sunken")
all_frame.pack()

Frame(height=2, bd=1, relief=SUNKEN).pack(fill=X, padx=5, pady=5)

end_frame = Frame(hlavni_okno, padx = 10, pady = 10)
end_frame.pack()



#VSTUPY

#NASTAVENÍ SÉRIOVÉHO PORTU
ser_open = Button(serial_frame, text = u"Otevřít sériový port", command = OpenSer)
ser_open.grid(row = 0, column = 0)

ser_close = Button(serial_frame, text = u"Zavřít sériový port", command = CloseSer, state = "disabled")
ser_close.grid(row = 0, column = 1)

ser_port = Label(serial_frame, textvariable = com_var)
ser_port.grid(row = 1, column = 0, columnspan = 2, sticky = W + E)

#HODINY
hod_popis = Label(all_frame, text = "Hodiny")
hod_popis.grid(row = 0, column = 0)

min_popis = Label(all_frame, text = "Minuty")
min_popis.grid(row = 0, column = 1)

sec_popis = Label(all_frame, text = "Sekundy")
sec_popis.grid(row = 0, column = 2)


hod = Spinbox(all_frame, from_ = 0, to = 23, width = 10)
hod.grid(row = 1, column = 0)

minutes = Spinbox(all_frame, from_ = 0, to = 59, width = 10)
minutes.grid(row = 1, column = 1)

sec = Spinbox(all_frame, from_ = 0, to = 59, width = 10)
sec.grid(row = 1, column = 2)

#oddeleni tlačítka od ostatních komponent
Label(all_frame, width = 5).grid(row = 1, column = 3)

time_but = Button(all_frame, text = u"Změnit čas", command = ZmenaHodin)
time_but.grid(row = 1, column = 4, sticky = W + E)

Label(all_frame, padx = 1, pady = 1, text = "------------------------------------------").grid(row = 2, column = 0, columnspan = 5, sticky = W + E)
#Frame(all_frame, height=2, bd=1, padx=5, pady=5, relief=SUNKEN).grid(row = 2, column = 0, columnspan = 4)

#TEXT
text_entry = Entry(all_frame)
text_entry.grid(row = 3, column = 0, columnspan = 3, sticky = W + E)




text_but = Button(all_frame, text = u"Změnit běžící text", command = ZmenaTextu)
text_but.grid(row = 3, column = 4, sticky = W + E)

#ZÁPIS TEXTU
text_write = Entry(all_frame)
text_write.grid(row = 4, column = 0, columnspan = 3, sticky = W + E)

text_write_but = Button(all_frame, text = u"Zápis textu", command = WriteText)
text_write_but.grid(row = 4, column = 4, sticky = W + E)

#LABEL na oznámění tónu
sound_read = Label(all_frame, textvariable = sound_var)
sound_read.grid(row = 5, column = 0, columnspan = 3)


#KONEC, MAZÁNÍ TEXTU ATD.
delete_but = Button(end_frame, text = u"Smazat text", command = DeleteText)
delete_but.grid(row = 0, column = 0)

end_but = Button(end_frame, text = u"Ukončit program", command = hlavni_okno.destroy)
end_but.grid(row = 0, column = 1)


#UDÁLOSTI KLAVÍR
hlavni_okno.bind("<F1>", C1)
hlavni_okno.bind("<F2>", D1)
hlavni_okno.bind("<F3>", E1)
hlavni_okno.bind("<F4>", F1)
hlavni_okno.bind("<F5>", G1)
hlavni_okno.bind("<F6>", A1)
hlavni_okno.bind("<F7>", H1)
hlavni_okno.bind("<F8>", C2)

  
mainloop()

