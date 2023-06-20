from asyncore import write
from atexit import register
from turtle import pd
import textract
import re
import PyPDF2

pre = "null"
def findall_index(pattern, text):
    #print("in findall")
    res =  [(m.start(0), m.end(0)) for m in re.finditer(pattern, text)]
    #print(res)
    return res
def newLine(text,beginI):
    for i in range(beginI,beginI+250):
        if (text[i] == '\n' or text[i] == '\r'):
            return i
    return i
def findIns(pageNum,type,pdfReader,restxt):
    global pre
    end = pageNum-50 if pageNum > 10 else 0
    for i in range(pageNum,end,-1):
        pageObj = pdfReader.getPage(i) 
        text = pageObj.extractText()
        Title = findall_index(r'C5.2.[0-9]+   ', text)
        if(Title):
            beginI = Title[0][0]
            endI = newLine(text,beginI)
            Rname = text[beginI:endI]
            if(Rname == pre):
                break
            
            #print("begin",beginI,"endI",endI)
            restxt.write("\n")
            if (type==0):
                restxt.write("0")
            elif (type==1):
                restxt.write("1")
            elif (type==2):
                restxt.write("2")
            else :
                restxt.write("3")
            #restxt.write(" page:")
            restxt.write(" ")
            restxt.write(str(i+408))
            restxt.write(" ")
            restxt.write(Rname)
            pre = Rname
            break

def parse_manual():
    #print("start")
    IStext = open('IScontext.txt', 'w+',encoding='utf-8')
    restxt = open('C5_register_res.txt','w+',encoding='utf-8')
    pdfFileObj = open('C5_2_Register.pdf','rb')
    pdfReader = PyPDF2.PdfFileReader(pdfFileObj)
    for i in range(pdfReader.numPages):
        pageObj = pdfReader.getPage(i) 
        text = pageObj.extractText()
        IStext.write(text)
        Branch0 = findall_index(r'if PSTATE.EL == EL0 then[ ]*', text)
        Branch1 = findall_index(r'elsif PSTATE.EL == EL1 then[ ]*',text)
        Branch2 = findall_index(r'elsif PSTATE.EL == EL2 then[ ]*',text)
        Branch3 = findall_index(r'elsif PSTATE.EL == EL3 then[ ]*',text)
        if(Branch0):
            if (not (Branch1 and Branch2 and Branch3)):
                #print("\nin two page:", i+1)
                tmpContext = open('tmpPage.txt', 'w+',encoding='utf-8')
                pageObj = pdfReader.getPage(i) 
                text1 = pageObj.extractText()
                tmpContext.write(text1)
                pageObj = pdfReader.getPage(i+1) 
                text2 = pageObj.extractText()
                tmpContext.write(text2)
                text3 = text1 + text2
                Branch0 = findall_index(r'if PSTATE.EL == EL0 then[ ]*', text3)
                Branch1 = findall_index(r'elsif PSTATE.EL == EL1 then[ ]*',text3)
                Branch2 = findall_index(r'elsif PSTATE.EL == EL2 then[ ]*',text3)
                Branch3 = findall_index(r'elsif PSTATE.EL == EL3 then[ ]*',text3)
                branch0Begin = Branch0[0][1]
                branch0End = Branch1[0][0]
                branch0Text = text3[branch0Begin:branch0End]

                branch1Begin = Branch1[0][1]
                branch1End = Branch2[0][0]
                branch1Text = text3[branch1Begin:branch1End]

                branch2Begin = Branch2[0][1]
                branch2End = Branch3[0][0]
                branch2Text = text3 [branch2Begin:branch2End]

                if(i == 32):
                    print("branch0\n" + branch0Text)
                    print("branch1\n" + branch1Text)
                    print("branch2\n" + branch2Text)
                
                if (branch0Text == "UNDEFINED;"):
                    if (branch1Text != "UNDEFINED;" or branch2Text != "UNDEFINED;"):
                        findIns(i,0,pdfReader,restxt)
                    else:
                        findIns(i,1,pdfReader,restxt)
                else:
                    if (branch0Text != branch1Text or branch0Text != branch2Text):
                        a = 1
                        findIns(i,2,pdfReader,restxt)
                    else:
                        findIns(i,3,pdfReader,restxt)
            else :
                #print("\nin page:",i+1)
                branch0Begin = Branch0[0][1]
                branch0End = Branch1[0][0]
                branch0Text = text[branch0Begin:branch0End]

                branch1Begin = Branch1[0][1]
                branch1End = Branch2[0][0]
                branch1Text = text[branch1Begin:branch1End]

                branch2Begin = Branch2[0][1]
                branch2End = Branch3[0][0]
                branch2Text = text[branch2Begin:branch2End]

                if (branch0Text == "UNDEFINED;"):
                    if (branch1Text != "UNDEFINED;" or branch2Text != "UNDEFINED;"): 
                        findIns(i,0,pdfReader,restxt)
                    else:
                        findIns(i,1,pdfReader,restxt)
                else:
                    if (branch0Text != branch1Text or branch0Text != branch2Text):
                        findIns(i,2,pdfReader,restxt)
                    else:
                        findIns(i,3,pdfReader,restxt)
            
parse_manual()
