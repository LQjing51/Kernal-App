from argparse import ONE_OR_MORE
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
    #print("in findall")
    res =  [(m.start(0), m.end(0)) for m in re.finditer(pattern, text)]
    #print(res)
    return res
def checkSemicolon(index,text):
    # if index == (213,496):
    #     print(text[213:497])
    begin = index[0]
    end = index[1]
    trueBegin = begin
    trueEnd = 0
    for i in range(begin,end):
        if (text[i:i+6] == "return" or text[i:i+9] == "UNDEFINED" or text[i:i+24] == "AArch64.SystemAccessTrap"):
            trueBegin = i
            #print(text[trueBegin:trueBegin+10])
        if (text[i] == ";" and i > trueBegin):
            trueEnd = i+1;
    if trueEnd: #have ;
        return (trueBegin,trueEnd) #if not begin with return/undifine/trap, other situation will be saved all
    else: #no have; page foot
        return []
def subCondition(condition,text):#delete the conditions, leave the branch contexts
    #print(condition)
    res = []
    length = len(text)
    conditionNum = len(condition)
    if (not condition):
        single = checkSemicolon((0,length),text)
        if single:
            res.append(single)
        return res
    pre = condition[0]
    for cond in condition[1:]:
        if (cond[0]-pre[1] > 0):
            single = checkSemicolon((pre[1],cond[0]),text)
            if single:
                res.append(single)
        pre = cond

    single = checkSemicolon((condition[conditionNum-1][1],length),text)
    if single:
        res.append(single)    
    #print(res)
    return res

def newLine(text,beginI):
    describeBegin = beginI
    end = 0
    #print(text[beginI:beginI+30])
    for i in range(beginI,beginI+250):
        if (text[i] == ','):
            describeBegin = i
        if (text[i] == '\n' or text[i] == '\r'):
            end =  i
        if (describeBegin and end):
            return text[describeBegin:end]
def getName(text,beginI):
    describeBegin = beginI
    end = 0
    #print(text[beginI:beginI+30])
    for i in range(beginI,beginI+250):
        if (text[i] == ','):
            end = i
        if (describeBegin and end):
            return text[describeBegin:end]
def findIns(pageNum,pdfReader,restxt,name):
    #print(pageNum)
    global pre
    end = pageNum-50 if pageNum > 10 else 0
    for i in range(pageNum,end,-1):
        pageObj = pdfReader.getPage(i) 
        text = pageObj.extractText()
        #Title = findall_index(r'D13.[2-8].[0-9]+   ', text)
        Title = findall_index(r'(C5.2.[0-9]+   )|(D13.[2-8].[0-9]+   )', text)
        if(Title):
            register = newLine(text,Title[0][1])
            titleName = getName(text,Title[0][1])
            # if titleName == name or (name[len(name)-2:len(name)]!='02' and name[len(name)-2:len(name)]!='12'):return 1
            if titleName != name: return 0
            Rname = text[Title[0][0]:Title[0][1]] + name + register
            restxt.write(" ")
            restxt.write(str(i+1))
            restxt.write(" ")
            restxt.write(Rname)
            restxt.write("\n")
            pre = Rname
            break
    return 0
    
    
def checkAllTrapOrUndefine(contextIndexs,branchText):
    for index in contextIndexs:
        #print(branchText[index[0]:index[1]])
        if branchText[index[0]:index[0]+9] != "UNDEFINED" and branchText[index[0]:index[0]+24] != "AArch64.SystemAccessTrap" :
            #print("in")
            return 0
    return 1
def WriteBehavior(contextIndexs,branchText,ElxBehaviorTxt):
    for index in contextIndexs:
        # if branchText[index[0]:index[0]+9] != "UNDEFINED" and branchText[index[0]:index[0]+24] != "AArch64.SystemAccessTrap" :
        ElxBehaviorTxt.write(branchText[index[0]:index[1]]+"\n")
def parser(branchTexts):
    res = []
    for branchIndex in range(0,3):
        condition = findall_index(r'(if(.+?)then[ ]*)|(elsif(.+?)then[ ]*)|(else[ ]*)',branchTexts[branchIndex])
        contextIndexs = subCondition(condition,branchTexts[branchIndex])
        res.append(contextIndexs)
    return res
def parseOneMRS_MSR(branchTexts,pdfReader,i,El0UnbehaviorTxt,ElxBehaviorEqualTxt,ElxBehaviorDiffTxt,ElxTotalEqualTxt,EL12_EL02Txt):
    name = branchTexts[3]
    branchContextIndexs = parser(branchTexts)#extract the true context indexs of every branch(undifined/trap/return xx/other)
    notRedirect = 0
    for branchIndex in range(0,3):
        # print("context"+str(branchIndex))
        # for index in branchContextIndexs[branchIndex]:
        #     print(branchTexts[branchIndex][index[0]:index[1]])

        #check EL0 unbehavior
        # if not branchIndex:
        #     if (checkAllTrapOrUndefine(branchContextIndexs[0],branchTexts[0])):
        #         findIns(i,0,pdfReader,El0UnbehaviorTxt,name)
        #         break

        #check EL0/1/2's behaviors 
        if not branchIndex:
            # findIns(i,0,pdfReader,ElxBehaviorDiffTxt,name)
            notRedirect = findIns(i,0,pdfReader,EL12_EL02Txt,name)
        
        # if not notRedirect:
        #     EL12_EL02Txt.write("\n")
        #     WriteBehavior(branchContextIndexs[branchIndex],branchTexts[branchIndex],EL12_EL02Txt)        
        #     if branchIndex == 2:
        #         EL12_EL02Txt.write("\n")
        #         notRedirect = 0
def writeInst(pageNum,pdfReader,restxt,name,instContext):

    end = pageNum-50 if pageNum > 10 else 0
    for i in range(pageNum,end,-1):
        pageObj = pdfReader.getPage(i) 
        text = pageObj.extractText()
        Title = findall_index(r'(C5.2.[0-9]+   )|(D13.[2-8].[0-9]+   )', text)
        if(Title):
            register = newLine(text,Title[0][1])
            titleName = getName(text,Title[0][1])
            if titleName != name: return 0
            Rname = text[Title[0][0]:Title[0][1]] + name + register
            restxt.write(str(i+1))
            restxt.write(" ")
            restxt.write(Rname)
            restxt.write("\n")
            restxt.write(instContext)
            break
    return 0

def readOneOrTwoPage(pdfReader,i,InstContext):
    pageObj = pdfReader.getPage(i) 
    text = pageObj.extractText()
    Pre = findall_index(r'MRS <Xt>,',text)
    # Pre = findall_index(r'MSR(.+?), <Xt>',text)
    originText = text
    for singleIS in Pre:
        name = newLine(text,singleIS[1]+1)
        # name = text[singleIS[0]+4:singleIS[1]-6]
        text = text[singleIS[1]:]
        Branch0 = findall_index(r'if PSTATE.EL == EL0 then[ ]*', text)
        Branch1 = findall_index(r'elsif PSTATE.EL == EL1 then[ ]*',text)
        Branch2 = findall_index(r'elsif PSTATE.EL == EL2 then[ ]*',text)
        Branch3 = findall_index(r'elsif PSTATE.EL == EL3 then[ ]*',text)
        
        if(Branch0):
            if (not (Branch1 and Branch2 and Branch3)):
                pageObj = pdfReader.getPage(i) 
                text1 = pageObj.extractText()
                pageObj = pdfReader.getPage(i+1) 
                text2 = pageObj.extractText()
                text = text1 + text2
                text = text[singleIS[1]:]
                Branch0 = findall_index(r'if PSTATE.EL == EL0 then[ ]*', text)
                Branch1 = findall_index(r'elsif PSTATE.EL == EL1 then[ ]*',text)
                Branch2 = findall_index(r'elsif PSTATE.EL == EL2 then[ ]*',text)
                Branch3 = findall_index(r'elsif PSTATE.EL == EL3 then[ ]*',text)      
            branch0Begin = Branch0[0][0]
            branch0End = Branch1[0][0]
            branch0Text = text[branch0Begin:branch0End]

            branch1Begin = Branch1[0][0]
            branch1End = Branch2[0][0]
            branch1Text = text[branch1Begin:branch1End]

            branch2Begin = Branch2[0][0]
            branch2End = Branch3[0][0]
            branch2Text = text[branch2Begin:branch2End]
            
            instContext = text[branch0Begin:branch2End]
            writeInst(i,pdfReader,InstContext,name,instContext)

            # parseOneMRS_MSR(res,pdfReader,i,InstContext)
        text = originText

def parse_manual():
    
    InstContext = open('MSR_Inst_Contxt.txt','w+',encoding='utf-8')
    pdfFileObj = open('DDI0487G_b_armv8_arm.pdf','rb')
    pdfReader = PyPDF2.PdfFileReader(pdfFileObj)
    r = list(range(407,505)) + list(range(3048,4244))
    for i in r:
        if i != 408: continue
        readOneOrTwoPage(pdfReader,i,InstContext)
                                        
parse_manual()





















