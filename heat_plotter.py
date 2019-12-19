# -*- coding: utf-8 -*-
"""
Created on Sun Jul  7 12:56:10 2019

@author: Yannic
"""

import matplotlib.pyplot as plt
import re

show_temp = True


#class Index(object):
#    def showtemp(self, event):
#        global show_temp
#        show_temp = False
#        plt.clf()
#        plt.close()

#l, = plt.plot(0, 0,lw=3)
#print("try print")
plt.plot(0,0)
plt.show()

while show_temp:
    T_list =[]
    time=[]
    soll_T_list=[]
    debug_list=[]
    try:
        with open("heat_valus.txt", 'r') as f:
            my_list = [line.rstrip('\n') for line in f]
            f.close()
        if len(my_list)>1000:
            my_list = my_list[-1000:]
        index = 0
        print(my_list[-1])
        for element in my_list:
            try:
                T = re.search("(<T>)(-*\d*.\d*)(</T>)",element)
                soll_T = re.search("(<soll_T>)(-*\d*.\d*)(</soll_T>)",element)
                debug = re.search("(<debug>)(-*\d*.\d*)(</debug>)",element)
                T_list.append(round(float(T.group(2)),2))
                soll_T_list.append(round(float(soll_T.group(2)),2))
                debug_list.append((round(float(debug.group(2)),2))/25)
                index += 1
                time.append(index)
            except:
                if len(T_list)>len(soll_T_list):
                    del T_list[-1]
                if len(soll_T_list)>len(debug_list):
                    del T_list[-1]
                    del soll_T_list[-1]
                
          #print(element,T_list[-1],soll_T_list[-1],debug[-1])
           
           
                
    except Exception as e:
        print(str(e))
    #l.set_ydata(heat)
    #l.set_xdata(heattime)
    #print(heat)
    #print(heattime)
    plt.plot(time,T_list,time,soll_T_list,time,debug_list)
    plt.grid()
    plt.draw()
    plt.pause(1)
    plt.clf()
#    fig, ax = plt.subplots()
#    plt.subplots_adjust(bottom=0.15)
#    callback = Index()
#    axnext = plt.axes([0.8, 0.01, 0.1, 0.075])
#    bnext = Button(axnext, 'End')
#    bnext.on_clicked(callback.showtemp)  

#x = []
#for i in range(0,100):
#    x.append(i)
#    plt.plot(x)
#    plt.draw()
#    plt.pause(1)
#    plt.clf()