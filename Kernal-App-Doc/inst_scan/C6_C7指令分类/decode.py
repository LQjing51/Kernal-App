from argparse import ONE_OR_MORE
from ast import Is, alias
from asyncore import write
from atexit import register
from gettext import find
from multiprocessing import context
from pydoc import describe
from sre_constants import IN
from turtle import pd
from unicodedata import name
import textract
import re
import PyPDF2

pre = "null"
def findall_index(pattern, text):
    res =  [(m.start(0), m.end(0)) for m in re.finditer(pattern, text)]
    return res

def newLine(text,beginI):
    describeBegin = beginI
    end = 0
    for i in range(beginI,beginI+250):
        if (text[i] == '\n' or text[i] == '\r'):
            end =  i
        if (describeBegin and end):
            return text[describeBegin:end]

def findIns(pageNum,pdfReader,restxt,aliasName):

    end = pageNum-50 if pageNum > 10 else 0
    for i in range(pageNum,end,-1):
        pageObj = pdfReader.getPage(i) 
        text = pageObj.extractText()
        Title = findall_index(r'(C6.2.[0-9]+   )|(C7.2.[0-9]+   )', text)
        if(Title):
            ISname = newLine(text,Title[0][1])
            Namemap = aliasName + " " + text[Title[0][0]:Title[0][1]] + ISname
            # restxt.write("\n")
            restxt.write(str(i+1))
            restxt.write(" ")
            restxt.write(Namemap)
            restxt.write("\n")
            break

def readOnePage(pdfReader, i):
    pageObj = pdfReader.getPage(i) 
    text = pageObj.extractText()
    Pre = findall_index(r'(Alias conditions)|(Assembler symbols)|(Operation)|(System)',text)
    if (not Pre): return -1
    else: 
        return Pre[0][0]
def readMore(pdfReader,pageNum,currentText):
    # read pages until another title
    endPage = pageNum + 1
    nextBlockBegin = readOnePage(pdfReader,endPage)
    while(nextBlockBegin == -1):
        endPage = endPage+1
        nextBlockBegin = readOnePage(pdfReader,endPage)
    InstContext = currentText
    for j in range(pageNum+1,endPage+1):
        pageObj = pdfReader.getPage(j) 
        tmpText = pageObj.extractText()
        if j != endPage:
            InstContext = InstContext + tmpText
        else:
            InstContext = InstContext + tmpText[0:nextBlockBegin]
    return InstContext

def extractdcContext(pageNum,pdfReader,text):

    nextBlock = findall_index(r'(Alias conditions)|(Assembler symbols)|(Operation)|(System)',text)
    if nextBlock:
        return text[0:nextBlock[0][0]]
    else:
        return readMore(pdfReader,pageNum,text)


def checkCall(pdfReader, callNonImple, context,pageNum):
    ELTxt = open('tmp_functionEL.txt',encoding='utf-8')
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
                    findIns(pageNum,pdfReader,callNonImple,"")
                    flag = 1
                for res in res1:
                    callNonImple.write(context[res[0]:res[1]]+"\n")
                for res in res2:
                    callNonImple.write(context[res[0]:res[1]]+"\n")
                # return True
        else:
            break
    # return False
    if flag:
        callNonImple.write("\n")
        return True
    else:
        return False

def notAnnotation(res,context):
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

def checkEL(context):
    
    for i in range(0,len(context)):
        if (i+9 <= len(context) and context[i:i+9] == "PSTATE.EL" and notAnnotation(i,context)!= -1):
            break
    if i != len(context)-1:
       return True
    return False

def parse_manual():
    # aliasTxt = open('alias.txt','w+',encoding='utf-8')
    # originHasELTxt = open('originELInst_decode.txt','w+',encoding='utf-8')
    callELfunction = open('callELfunction_decode.txt','w+',encoding='utf-8')
    callELfunctionInfo = open('callELfunctionInfo_decode.txt','w+',encoding='utf-8')
    #ELUnrelatedTxt = open('ELUnrelated.txt','w+',encoding='utf-8')
    # aliasNonImple = open('aliasNonImple.txt','w+',encoding='utf-8')
    # callNonImple = open('callNonImple.txt','w+',encoding='utf-8')
    # callNonImpleInfo = open('callNonImpleInfo.txt','w+',encoding='utf-8')
    pdfFileObj = open('DDI0487G_b_armv8_arm.pdf','rb')
    pdfReader = PyPDF2.PdfFileReader(pdfFileObj)
    for i in range(874,2450):
        # if (i != 1264): continue
        pageObj = pdfReader.getPage(i) 
        text = pageObj.extractText()
        Pre = findall_index(r'(Decode for all variants of this encoding|Decode for this encoding|Shared decode for all encodings)(\n|\r)',text)
        if not Pre: continue
        # print(i)
        for singlePre in Pre: 
            decodeContext = extractdcContext(i,pdfReader,text[singlePre[1]:])
            # print(decodeContext)
            
            #check operation: original EL related
            # if checkEL(decodeContext):
            #     findIns(i,pdfReader,originHasELTxt,"") 

            # check operation : call to EL functions
            if checkCall(pdfReader, callELfunctionInfo,decodeContext,i):
                findIns(i,pdfReader,callELfunction,"")
                          

parse_manual()