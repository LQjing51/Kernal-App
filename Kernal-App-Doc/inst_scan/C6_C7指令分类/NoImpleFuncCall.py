from argparse import ONE_OR_MORE
from asyncore import write
from atexit import register
from bdb import checkfuncname
from gettext import find
from multiprocessing import context
from os import abort
from pydoc import describe
from sre_constants import IN
from turtle import pd
from unicodedata import name
import textract
import re
import PyPDF2

def findall_index(pattern, text):
    res =  [(m.start(0), m.end(0)) for m in re.finditer(pattern, text)]
    return res
def addToNewELTxt(pageNum,funcName):
    newELTxt = open('newNonImpleFunc.txt','a',encoding='utf-8')
    newELTxt.write(str(pageNum+1))
    newELTxt.write(" ") 
    newELTxt.write(funcName)
def notInWord(res,context):
    if res != -1:
        pre = context[res-1]
        if (pre > 'A' and pre < 'Z') or (pre > 'a' and pre < 'z') or pre == '.':
            return -1
    return res  
def notAnnotation(res,context,pageNum,funcName):
    # print("find res = "+context[res-3:res+20])
    ptr = res-1
    if res != -1:
        while(context[ptr]!='\n' and context[ptr-2:ptr+1]!="   "):
            if context[ptr] =='/' and context[ptr-1] == '/':
                # print("find a annotation")
                return -1
            ptr = ptr-1
    # if (res != -1):
    #     print(context[res-2:res+10])
    return res
def checkCall(context,pageNum,funcName):

    matchRecord = open('matchRecord_level5.txt','a',encoding='utf-8')
    ELTxt = open('NoImpleFunc.txt',encoding='utf-8')
    flag = 0
    while True:
        line = ELTxt.readline()
        if line:
            for j in range(len(line)-2,0,-1):
                if line[j] == '/':
                    line = line[j+1:len(line)-1]
                    break
            
            res1 = findall_index(r' ' + line + '\(',context)
            res2 = findall_index(r' ' + line + '\[',context)

            if res1 or res2:
                if not flag :
                    matchRecord.write(funcName)
                    flag = 1
                for res in res1:
                    if notAnnotation(res[0],context,pageNum,funcName)!= -1:
                        matchRecord.write(context[res[0]:res[1]]+"\n")
                for res in res2:
                    if notAnnotation(res[0],context,pageNum,funcName)!= -1:
                        matchRecord.write(context[res[0]:res[1]]+"\n")
                # return True
            
        else:
            break
    # return False
    if flag:
        matchRecord.write("\n")
        return True
    else:
        return False

def checkCallNonImple(pdfReader,pageNum,context,funcName):
    count = 0
    for i in range(0,len(funcName)):
        if funcName[i] == "/":
            count = count + 1
    if (count == 1):
        return 
    
    ELTxt = open('NoImpleFunc.txt',encoding='utf-8')
    ELTxtContext = ELTxt.read()
    if ELTxtContext.find(funcName) == -1:
        if checkCall(context,pageNum,funcName):
            addToNewELTxt(pageNum,funcName)

def readOnePage(pdfReader, i):
    pageObj = pdfReader.getPage(i) 
    text = pageObj.extractText()
    Pre = findall_index(r'((aarch64|shared)/(debug|exceptions|functions|instrs|translation|trace)/([^ ]+?)/([^ ]+?)\n)|((aarch64|shared)/(debug|exceptions|functions|instrs|translation|trace)[ ]*\n)',text)
    if (not Pre): return -1
    else: 
        return Pre[0][0]
def readMore(pdfReader,pageNum,currentTitleEnd,currentTitle):
    count = 0
    for i in range(0,len(currentTitle)):
        if currentTitle[i] == "/":
            count = count + 1
    if (count == 1):
        return 
    # read pages until another title
    endPage = pageNum + 1
    nextTitleBegin = readOnePage(pdfReader,endPage)
    while(nextTitleBegin == -1):
        endPage = endPage+1
        nextTitleBegin = readOnePage(pdfReader,endPage)
    pageObj = pdfReader.getPage(pageNum) 
    tmpText = pageObj.extractText()
    functionContext = tmpText[currentTitleEnd:]
    for j in range(pageNum+1,endPage+1):
        pageObj = pdfReader.getPage(j) 
        tmpText = pageObj.extractText()
        if j != endPage:
            functionContext = functionContext + tmpText
        else:
            functionContext = functionContext + tmpText[0:nextTitleBegin]
    checkCallNonImple(pdfReader,pageNum,functionContext,currentTitle)

def readOneOrMorePage(pdfReader,i):
    pageObj = pdfReader.getPage(i) 
    text = pageObj.extractText()
    Pre = findall_index(r'((aarch64|shared)/(debug|exceptions|functions|instrs|translation|trace)/([^ ]+?)/([^ ]+?)\n)|((aarch64|shared)/(debug|exceptions|functions|instrs|translation|trace)[ ]*\n)',text)#[a-zA-Z\.0-9\/\-]
    tmpPre = []
    for index in range(0,len(Pre)):
        if text[Pre[index][1]+1:Pre[index][1]+3] != "on":
            tmpPre.append(Pre[index])
    Pre = tmpPre
    if (not Pre): return  # this page's context is included in its previous pages 
    # print(len(Pre))
    if (len(Pre) == 1):
        name = text[Pre[0][0]:Pre[0][1]]
        if (i == 8381):
            functionContext = text[Pre[0][1]:]
            checkCallNonImple(pdfReader, i, functionContext,name)
        else:
            readMore(pdfReader,i,Pre[0][1],name)
    if (len(Pre) > 1):
        for j in range(0,len(Pre)):
            name = text[Pre[j][0]:Pre[j][1]]
            if (j == len(Pre)-1):
                readMore(pdfReader,i,Pre[j][1],name)
            else:
                functionContext = text[Pre[j][1]:Pre[j+1][0]]
                checkCallNonImple(pdfReader, i, functionContext,name)

def parse_manual():
    
    #originELTxt = open('functionOriginEL.txt','w+',encoding='utf-8')
    pdfFileObj = open('DDI0487G_b_armv8_arm.pdf','rb')
    pdfReader = PyPDF2.PdfFileReader(pdfFileObj)
    r = list(range(7959,8133)) + list(range(8221,8382)) #pseudocode 
    for i in r:
        # if (i != 8002): continue
        readOneOrMorePage(pdfReader,i)
                                        
parse_manual()

